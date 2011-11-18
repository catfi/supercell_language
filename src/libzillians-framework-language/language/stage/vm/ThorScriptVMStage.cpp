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


#include "language/stage/vm/ThorScriptVMStage.h"
#include "utility/StringUtil.h"
#include "llvm/Support/DynamicLibrary.h"

namespace zillians { namespace language { namespace stage {

//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptVMStage::ThorScriptVMStage() : Stage()
{ }

ThorScriptVMStage::~ThorScriptVMStage()
{ }

const char* ThorScriptVMStage::name()
{
	return "thor_script_vm_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptVMStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	// TODO: Well, not exactly know the input options
	option_desc_public->add_options()
    ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptVMStage::parseOptions(po::variables_map& vm)
{
	if (vm.count("input"))
	{
		std::vector< std::string > inputs = vm["input"].as< std::vector<std::string> >();
		BOOST_ASSERT(inputs.size() >= 2 && "The input parameters are wrong");

		module_name = inputs[0];
		entry_symbol = getManglingName(inputs[1]);

		std::cout << "module name = " << module_name << std::endl;
		std::cout << "entry symbol = " << entry_symbol << std::endl;
	}

	return true;
}

bool ThorScriptVMStage::execute(bool& continue_execution)
{
	typedef void (*function_handle_t)();
	bool fail = false;

	std::string error;
	fail = llvm::sys::DynamicLibrary::LoadLibraryPermanently(module_name.c_str(), &error);

	if (fail)
	{
		std::cerr << error << std::endl;
		return false;
	}

	function_handle_t entry_function = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(entry_symbol);
	if (entry_function)
		entry_function();
	else
	{
		std::cerr << "Fail to locate symbol: " << entry_symbol << std::endl;
		return false;
	}

	return true;
}

std::string ThorScriptVMStage::getManglingName(std::string& name)
{
	/*
	 * Well, we assume the entry function is void xxxx(void)
	 */

	std::vector<std::string> tokens = StringUtil::tokenize(name, ".");
	BOOST_ASSERT(tokens.size() != 0);

	std::string mangling;
	std::string prefix;
	std::string postfix;

	if (tokens.size() == 1)
	{
		// Well, this should be global function
		prefix = "_Z";
		postfix = "v";
	}
	else
	{
		// Well, the function resides in namespace or is a class static function, or both.
		prefix = "_ZN";
		postfix = "Ev";
	}

	mangling = prefix;
	for (int i = 0; i < tokens.size(); i++)
	{
		mangling += StringUtil::itoa(tokens[i].size(), 10) + tokens[i] ;
	}
	mangling += postfix;

	return mangling;
}

} } }
