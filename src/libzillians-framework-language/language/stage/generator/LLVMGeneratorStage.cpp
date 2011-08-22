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

#include "language/stage/generator/LLVMGeneratorStage.h"
#include "language/stage/generator/detail/LLVMForeach.h"
#include "language/context/ParserContext.h"
#include "language/context/GeneratorContext.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/PassManager.h>
#include <llvm/CallingConv.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>

namespace zillians { namespace language { namespace stage {

LLVMGeneratorStage::LLVMGeneratorStage()
{ }

LLVMGeneratorStage::~LLVMGeneratorStage()
{ }

const char* LLVMGeneratorStage::name()
{
	return "llvm_generator";
}

void LLVMGeneratorStage::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{
    option_desc.add_options()
    ("llvm-module-name", po::value<std::string>(),	"llvm module name");
}

bool LLVMGeneratorStage::parseOptions(po::variables_map& vm)
{
	if(vm.count("llvm-module-name") == 0)
	{
		llvm_module_name = "default_module";
	}
	else if(vm.count("llvm-module-name") == 1)
	{
		llvm_module_name = vm["llvm-module-name"].as<std::string>();
	}
	else
	{
		return false;
	}

	return true;
}

bool LLVMGeneratorStage::execute()
{
	llvm::Module* module = new llvm::Module(llvm_module_name, llvm::getGlobalContext());
	// TODO make a visitor to walk through the entire tree and generate instructions accordingly
	getGeneratorContext().modules.push_back(module);
	return true;
}

} } }
