/**
 * Zillians MMO
 * Copyright (C) 2007-2011 Zillians.com, Inc.
 * For more information see http://www.zillians.com
 *
 * Zillians MMO is the library and runtime for massive multiplayer online game
 * development in utility computing model, which runs as a service for every
 * developer to build their virtual world running on our GPU-assisted machines.
 *
 * This is a close source library intended to be used solely within Zillians.com
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <vector>
#include "language/tree/ASTNodeFactory.h"
#include "language/stage/generator/context/SynthesizedFunctionContext.h"
#include "language/stage/generator/LLVMGlobalDispatchGeneratorStage.h"
#include "language/stage/generator/visitor/LLVMGlobalDispatchGeneratorVisitor.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/LLVMContext.h"

namespace zillians { namespace language { namespace stage {

LLVMGlobalDispatchGeneratorStage::LLVMGlobalDispatchGeneratorStage(llvm::LLVMContext& llvmContext, llvm::Module& module, zillians::language::ParserContext* parserContext)
    : mEnabled(false), mLLVMContext(llvmContext), mBuilder(llvmContext), mLLVMModule(module), mParserContext(parserContext)
{ }

LLVMGlobalDispatchGeneratorStage::~LLVMGlobalDispatchGeneratorStage()
{ }

const char* LLVMGlobalDispatchGeneratorStage::name()
{
	return "LLVM Global Dispatcher Generation Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> LLVMGlobalDispatchGeneratorStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("no-dispatcher", "disable global dispatcher generation");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool LLVMGlobalDispatchGeneratorStage::parseOptions(po::variables_map& vm)
{
	mEnabled = !(vm.count("no-dispatcher") > 0);
	return true;
}

bool LLVMGlobalDispatchGeneratorStage::execute(bool& continue_execution)
{
	if (!mEnabled)
		return true;

	visitor::LLVMGlobalDispatchGeneratorVisitor visitor;

	if(mParserContext->program)
	{
		visitor.visit(*mParserContext->program);
		this->generateGlobalDispatcher(visitor.mServerFunctions);
	}


	return true;
}

void LLVMGlobalDispatchGeneratorStage::generateGlobalDispatcher(std::vector<FunctionDecl*>& serverFunctions)
{
    using namespace llvm;
    // module & function

    Constant* c = mLLVMModule.getOrInsertFunction("global_dispatcher_func",
                                                  IntegerType::get(mLLVMContext, 32), // return value
                                                  IntegerType::get(mLLVMContext, 32), // arg_0 function id
                                                  NULL);
    Function *func = ::llvm::cast<Function>(c);
    // entry blocks
    BasicBlock* entry = BasicBlock::Create(mLLVMContext, "entry", func);
    // switch on arg_0
    Function::arg_iterator args = func->arg_begin();
    Value* arg_0 = args;
    arg_0->setName("function_id");

    // switch default block & final block
    BasicBlock* defaultBlock = BasicBlock::Create(mLLVMContext, "default_block", func);
    BasicBlock* finalBlock = BasicBlock::Create(mLLVMContext, "final_block", func);
    // create switch
    mBuilder.SetInsertPoint(entry);
    SwitchInst* llvmSwitch = mBuilder.CreateSwitch(arg_0, defaultBlock, serverFunctions.size());

    // generate all server functions
    generateAllServerFunctions(llvmSwitch, finalBlock, serverFunctions);
}

void LLVMGlobalDispatchGeneratorStage::generateAllServerFunctions(llvm::SwitchInst* llvmSwitch, llvm::BasicBlock* finalBlock, std::vector<FunctionDecl*>& serverFunctions)
{
    uint64 id;
    foreach(f, serverFunctions)
    {
        generateOneServerFunction(llvmSwitch, finalBlock, **f, id);
        ++id;
    }
}

void LLVMGlobalDispatchGeneratorStage::generateOneServerFunction(llvm::SwitchInst* llvmSwitch, llvm::BasicBlock* finalBlock, FunctionDecl& func, const uint64 id)
{
    using namespace llvm;
    Value* caseValue = NULL;
    BasicBlock* caseBlock = BasicBlock::Create(mLLVMContext, "case_block");
    mBuilder.SetInsertPoint(caseBlock);
    Function* llvmDispatchedFunction = zillians::language::stage::SynthesizedFunctionContext::get(&func)->f;
    mBuilder.CreateCall(llvmDispatchedFunction, "caseDispatchFunction");
    mBuilder.CreateBr(finalBlock);
    ConstantInt* constCaseValue = ConstantInt::get(mLLVMContext, APInt(32, id));
    llvmSwitch->addCase(constCaseValue, caseBlock);
}

} } } // namespace zillians language stage
