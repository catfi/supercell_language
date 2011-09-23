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
#include "language/stage/generator/visitor/LLVMGeneratorPreambleVisitor.h"
#include "language/stage/generator/visitor/LLVMGeneratorVisitor.h"
#include "language/context/ParserContext.h"
#include "language/context/GeneratorContext.h"

namespace zillians { namespace language { namespace stage {

LLVMGeneratorStage::LLVMGeneratorStage() : enabled(true)
{ }

LLVMGeneratorStage::~LLVMGeneratorStage()
{ }

const char* LLVMGeneratorStage::name()
{
	return "LLVM IR Generation Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> LLVMGeneratorStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description("LLVM IR Generator Option"));
	shared_ptr<po::options_description> option_desc_private(new po::options_description("LLVM IR Generator Option"));

	option_desc_public->add_options()
		("no-llvm", "disable LLVM IR generation");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options()
		("llvm-module-name", po::value<std::string>(),	"llvm module name");

	return std::make_pair(option_desc_public, option_desc_private);
}

bool LLVMGeneratorStage::parseOptions(po::variables_map& vm)
{
	enabled = !(vm.count("no-llvm") > 0);

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

bool LLVMGeneratorStage::execute(bool& continue_execution)
{
	if(!enabled)
		return true;

	setGeneratorContext(new GeneratorContext());

	//llvm::LLVMContext& context = llvm::getGlobalContext();
	llvm::LLVMContext* context = new llvm::LLVMContext();
	llvm::Module* module = new llvm::Module(llvm_module_name, *context);

	// create visitor to walk through the entire tree and generate instructions accordingly
	if(getParserContext().program)
	{
		// emit preamble code (declare all LLVM functions)
		visitor::LLVMGeneratorPreambleVisitor preamble_visitor(*context, *module);
		preamble_visitor.visit(*getParserContext().program);

		// emit actual code into each function
		visitor::LLVMGeneratorVisitor visitor(*context, *module);
		visitor.visit(*getParserContext().program);
	}

	getGeneratorContext().modules.push_back(module);
	getGeneratorContext().context = context;

	return true;
}

} } }
