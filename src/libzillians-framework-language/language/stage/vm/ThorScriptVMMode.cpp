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


#include <boost/filesystem.hpp>
#include "llvm/Support/DynamicLibrary.h"
#include "language/stage/vm/ThorScriptVMMode.h"
#include "utility/StringUtil.h"
#include "utility/sha1.h"
#include "utility/Foreach.h"
#include "utility/archive/Archive.h"

#define THORSCRIPT_CLIENT_VM_CACHE		"vm-client-cache"
#define THORSCRIPT_SERVER_VM_CACHE		"vm-server-cache"

#define THORSCRIPT_SO_EXTENSION		".so"
#define THORSCRIPT_BC_EXTENSION		".bc"
#define THORSCRIPT_AST_EXTENSION	".ast"

namespace zillians { namespace language { namespace stage {


//////////////////////////////////////////////////////////////////////////////
// Static Functions
//////////////////////////////////////////////////////////////////////////////
static std::string getRandomFileName(const std::string extension)
{
	UUID filename;
	filename.random();
	return (std::string)filename + extension;
}

static std::string getSharedLibraryFromBundle(const std::string& cache_folder, const std::string& bundle)
{
	// Extract the bundle first
	boost::filesystem::path cachePath(cache_folder);
    std::string bundleSha1Name = sha1::sha1(bundle);
    boost::filesystem::path extract_path= cachePath / bundleSha1Name;

    Archive ar(bundle, ArchiveMode::ARCHIVE_FILE_DECOMPRESS);
    std::vector<ArchiveItem_t> archiveItems;
    ar.extractAllToFolder(archiveItems, extract_path.string());

    // Check if we have .so file, and we also collect .ast and .bc files
    // TODO: how to know which so is the entry file?
    std::vector<std::string> ast_files;
    std::vector<std::string> bc_files;
    foreach(item, archiveItems)
    {
		boost::filesystem::path file_name((*item).filename);
		std::string extension = file_name.extension().generic_string();
		boost::filesystem::path full_path = extract_path / file_name;

		if (extension == THORSCRIPT_SO_EXTENSION)
		{
			return full_path.generic_string();
		}
		else if (extension == THORSCRIPT_AST_EXTENSION)
		{
			ast_files.push_back(full_path.generic_string());
		}
		else if (extension == THORSCRIPT_BC_EXTENSION)
		{
			bc_files.push_back(full_path.generic_string());
		}
    }

    // Cannot find a so, so we need to compile it with bc (todo: or using ast)
    boost::filesystem::path generate_so = extract_path / getRandomFileName(THORSCRIPT_SO_EXTENSION);
    std::string cmd = "ts-link --output " + generate_so.generic_string();
    foreach(i, bc_files)
    {
    	cmd += " " + *i;
    }
    std::cout << "Command: " << cmd << std::endl;
    system(cmd.c_str());
    std::cout << "Generate the file " << generate_so << std::endl;

    return generate_so.generic_string();
}

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

		bundle_name = inputs[0];
		entry_symbol = getManglingName(inputs[1]);

		std::cout << "bundle name = " << bundle_name << std::endl;
		std::cout << "entry symbol = " << entry_symbol << std::endl;
	}
	return true;
}

bool ThorScriptClientVM::execute()
{
	typedef void (*function_handle_t)();
	bool fail = false;

	// Find out module name
	std::string module_name = getSharedLibraryFromBundle(THORSCRIPT_CLIENT_VM_CACHE, bundle_name);

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
