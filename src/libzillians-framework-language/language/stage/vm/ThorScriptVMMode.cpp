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


#include "llvm/Support/DynamicLibrary.h"
#include "language/stage/vm/ThorScriptVMMode.h"
#include "utility/StringUtil.h"

namespace zillians { namespace language { namespace stage {

//////////////////////////////////////////////////////////////////////////////
// ThorScriptBaseVM
//////////////////////////////////////////////////////////////////////////////
ThorScriptBaseVM::ThorScriptBaseVM()
{}

ThorScriptBaseVM::~ThorScriptBaseVM()
{}


//////////////////////////////////////////////////////////////////////////////
// ThorScriptServerStandaloneVM
//////////////////////////////////////////////////////////////////////////////
ThorScriptServerStandaloneVM::ThorScriptServerStandaloneVM()
{}

ThorScriptServerStandaloneVM::~ThorScriptServerStandaloneVM()
{}

bool ThorScriptServerStandaloneVM::parseOptions(boost::program_options::variables_map& vm)
{
	return true;
}

bool ThorScriptServerStandaloneVM::execute()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// ThorScriptServerSharedVM
//////////////////////////////////////////////////////////////////////////////
ThorScriptServerSharedVM::ThorScriptServerSharedVM()
{}

ThorScriptServerSharedVM::~ThorScriptServerSharedVM()
{}

bool ThorScriptServerSharedVM::parseOptions(boost::program_options::variables_map& vm)
{
	return true;
}

bool ThorScriptServerSharedVM::execute()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// ThorScriptClientVM
//////////////////////////////////////////////////////////////////////////////
ThorScriptClientVM::ThorScriptClientVM()
{}

ThorScriptClientVM::~ThorScriptClientVM()
{}

bool ThorScriptClientVM::parseOptions(boost::program_options::variables_map& vm)
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

bool ThorScriptClientVM::execute()
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

std::string ThorScriptClientVM::getManglingName(std::string& name)
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
