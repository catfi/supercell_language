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

#ifndef ZILLIANS_LANGUAGE_STAGE_GENERATOR_LLVMGLOBALDISPATCHGENERATORSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_GENERATOR_LLVMGLOBALDISPATCHGENERATORSTAGE_H_

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Support/IRBuilder.h"
#include "language/stage/Stage.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

class LLVMGlobalDispatchGeneratorStage : public Stage
{
public:
	LLVMGlobalDispatchGeneratorStage(llvm::LLVMContext& llvmContext, llvm::Module& module, zillians::language::ParserContext* parserContext);
	virtual ~LLVMGlobalDispatchGeneratorStage();

public:
	virtual const char* name();
	virtual void initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc);
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

private:
    void generateGlobalDispatcher(std::vector<zillians::language::tree::FunctionDecl*>& serverFunctions);
    void generateAllServerFunctions(llvm::SwitchInst* llvmSwitch, llvm::BasicBlock* finalBlock, std::vector<zillians::language::tree::FunctionDecl*>& serverFunctions);
    void generateOneServerFunction(llvm::SwitchInst* llvmSwitch, llvm::BasicBlock* finalBlock, zillians::language::tree::FunctionDecl& func, const uint64 id);

private:
    bool mEnabled;
    llvm::LLVMContext& mLLVMContext;
    llvm::Module& mLLVMModule;
    llvm::IRBuilder<> mBuilder;
    zillians::language::ParserContext* mParserContext;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_GENERATOR_LLVMGLOBALDISPATCHGENERATORSTAGE_H_ */
