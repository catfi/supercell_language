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

#include <fstream>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "utility/archive/Archive.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/stage/linker/ThorScriptLinkerStage.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/ASTNodeSerialization.h"

#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/LLVMContext.h"
#include "llvm/Linker.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/PathV1.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Program.h"

namespace zillians { namespace language { namespace stage {

#define THORSCRIPT_ASM_EXTENSION		".s"
#define THORSCRIPT_BUNDLE_EXTENSION		".bundle"
#define THORSCRIPT_BITCODE_EXTENSION	".bc"
#define THORSCRIPT_AST_EXTENSION		".ast"
#define THORSCRIPT_MANIFEST_EXTENSION	".xml"
#define THORSCRIPT_DEFAULT_OUTPUT		"ts.so"
#define NATIVE_COMPILER					"g++"

//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptLinkerStage::ThorScriptLinkerStage() : Stage(), output_file(THORSCRIPT_DEFAULT_OUTPUT)
{ }

ThorScriptLinkerStage::~ThorScriptLinkerStage()
{ }

const char* ThorScriptLinkerStage::name()
{
	return "thor_script_linker_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptLinkerStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("link,l", po::value<std::vector<std::string>>(), "link native library")
		("lpath,L", po::value<std::vector<std::string>>(), "link search path")
		("rpath,R", po::value<std::vector<std::string>>(), "runtime library search path")
		("output,o", po::value<std::string>(), "output file name")
		("debug,g", "[TODO] support debug")
		("optimization,O", "[TODO] Optimization");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptLinkerStage::parseOptions(po::variables_map& vm)
{
	if (vm.count("link"))
	{
		link_libraries = vm["link"].as< std::vector<std::string> >();
	}

	if (vm.count("input"))
	{
		// Separate files with extension name
		std::vector< std::string > inputs = vm["input"].as< std::vector<std::string> >();
		for (size_t i = 0; i < inputs.size(); i++)
		{
			boost::filesystem::path file_path(inputs[i]);
			std::string extension = file_path.extension().generic_string();

			if (extension == THORSCRIPT_BUNDLE_EXTENSION)
			{
				bundle_file = inputs[i];
			}
			else
			{
				native_files.push_back(inputs[i]);
			}
		}
	}

	if (vm.count("output"))
	{
		output_file = vm["output"].as<std::string>();
	}

	if (vm.count("lpath"))
	{
		link_search_paths = vm["lpath"].as< std::vector<std::string> >();
	}

	if (vm.count("rpath"))
	{
		runtime_search_paths = vm["rpath"].as< std::vector<std::string> >();
	}

	return true;
}

bool ThorScriptLinkerStage::execute(bool& continue_execution)
{
	// Unzip the bundle file
	std::string bc_file;
	std::string ast_file;
	std::string manifest_file;
	extractFilesFromBundle(bc_file, ast_file, manifest_file);

	// Compile to assembly file
	std::string asm_file;
	buildAssemblyCode(bc_file, asm_file);

	// Use native compiler to generate native code
	buildNativeCode(asm_file);

	// Delete the temporarily files
	std::remove(bc_file.c_str());
	std::remove(ast_file.c_str());
	std::remove(manifest_file.c_str());
	std::remove(asm_file.c_str());

	return true;
}

bool ThorScriptLinkerStage::extractFilesFromBundle(std::string& bc_file, std::string& ast_file, std::string& manifest_file)
{
	// Extract all archive items into memory
	Archive ar(bundle_file, ArchiveMode::ARCHIVE_FILE_DECOMPRESS);

	std::vector<ArchiveItem_t> items;
	ar.open();
	ar.extractAllToFolder(items);
	ar.close();

	// Well, return the filename back
	for (size_t i = 0; i < items.size(); i++)
	{
		boost::filesystem::path file_path(items[i].filename);
		std::string extension = file_path.extension().generic_string();

		if (extension == THORSCRIPT_BITCODE_EXTENSION) bc_file = items[i].filename;
		if (extension == THORSCRIPT_AST_EXTENSION)	ast_file = items[i].filename;
		if (extension == THORSCRIPT_MANIFEST_EXTENSION) manifest_file = items[i].filename;
	}

	std::cout << "bc: " << bc_file << std::endl;
	std::cout << "ast: " << ast_file << std::endl;
	std::cout << "manifest: " << manifest_file << std::endl;

	return true;
}

bool ThorScriptLinkerStage::buildAssemblyCode(const std::string& bc_file, std::string& asm_file)
{
	asm_file = bc_file + std::string(THORSCRIPT_ASM_EXTENSION);
	std::cout << "Assembly file: " << asm_file << std::endl;

	llvm::SMDiagnostic err;
	llvm::LLVMContext& context = llvm::getGlobalContext();

	// Initialize all targets
	llvm::InitializeAllTargets();
	llvm::InitializeAllAsmPrinters();
	llvm::InitializeAllAsmParsers();

	// Parse input bitcode file
	shared_ptr<llvm::Module> module(llvm::ParseIRFile(bc_file, err, context));

	if (module.get() == NULL)
	{
		std::cout << "Fail to parse bitcode file" << std::endl;
		return false;
	}

	// Retrieve architecture setting (ref: llc --march)
	llvm::Triple arch_setting( module->getTargetTriple() );
	if (arch_setting.getTriple().empty())
		arch_setting.setTriple(llvm::sys::getHostTriple());

	// Create the target machine
	std::string lookup_error;
	const llvm::Target *target = llvm::TargetRegistry::lookupTarget(arch_setting.getTriple(), lookup_error);

	if (target == NULL)
	{
		std::cout << "Fail to find the target : " << lookup_error << std::endl;
	}

	std::string feature;
	shared_ptr<llvm::TargetMachine> target_machine(target->createTargetMachine(arch_setting.getTriple(), feature));

	if (target_machine.get() == NULL)
	{
		std::cout << "Fail to create target machine" << std::endl;
	}

	// Set relocation model to pic
	target_machine->setRelocationModel(llvm::Reloc::PIC_);

	// Add the target data from the target machine, if it exists, or the module.
	llvm::PassManager pm;
	if (const llvm::TargetData* td = target_machine->getTargetData())
		pm.add(new llvm::TargetData(*td));
	else
		pm.add(new llvm::TargetData(module.get()));

	// Specify output file
	std::string out_error;
	shared_ptr<llvm::tool_output_file> out_file(new llvm::tool_output_file(asm_file.c_str(), out_error, llvm::raw_fd_ostream::F_Binary));
	out_file->keep();

	llvm::formatted_raw_ostream fos(out_file->os());
	target_machine->addPassesToEmitFile(pm, fos, llvm::TargetMachine::CGFT_AssemblyFile, llvm::CodeGenOpt::None, true);
	pm.run(*module);

	return true;
}

bool ThorScriptLinkerStage::buildNativeCode(std::string& asm_file)
{
	// TODO: How about windows!?
	llvm::sys::Path native_compiler = llvm::sys::Program::FindProgramByName(NATIVE_COMPILER);
	if (native_compiler.isEmpty())
	{
		std::cout << "Cannot find " << NATIVE_COMPILER << std::endl;
		return false;
	}

	// Prepare parameters to invoke gcc. Use std::string to easily concate string
	std::string args = native_compiler.c_str();
	args += " -shared -fPIC -o " + output_file;

	for (size_t i = 0; i < link_libraries.size(); i++)
		args += " -l" + link_libraries[i];

	// search path
	for (size_t i = 0; i < link_search_paths.size(); i++)
		args += " -L" + link_search_paths[i];

	for (size_t i = 0; i < runtime_search_paths.size(); i++)
		args += " -Wl,-rpath=" + runtime_search_paths[i];

	// input files
	args += " " + asm_file;
	for (size_t i = 0; i < native_files.size(); i++)
		args += " " + native_files[i];

	// TODO: Remove it. Output the command. I suppose we will play this one for a while.
	std::cout << args << std::endl;

	// Iovoke gcc
	int return_code = std::system(args.c_str());
	std::cout << "return code from " << NATIVE_COMPILER << ": " << return_code << std::endl;

	return true;
}

} } }
