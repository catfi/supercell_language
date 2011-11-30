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
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <apr_general.h>
#include <apr_thread_proc.h>

#include "utility/archive/Archive.h"
#include "language/stage/bundle/ThorScriptBundleStage.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/ASTNodeSerialization.h"
#include "language/stage/strip/visitor/ThorScriptStripStageVisitor.h"

#include "llvm/LLVMContext.h"
#include "llvm/Linker.h"
#include "llvm/Support/PathV1.h"
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"

#include "utility/sha1.h"


namespace zillians { namespace language { namespace stage {

#define THORSCRIPT_BITCODE_EXTENSION	".bc"
#define THORSCRIPT_AST_EXTENSION		".ast"
#define THORSCRIPT_DEFAULT_BUNDLE_NAME	"output.bundle"
#define THORSCRIPT_AST_TEMP_MERGED_FILE "ast_merged_tmp"
#define THORSCRIPT_BC_TMP_MERGED_FILE	"bc_merged_tmp"

//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////

static void initializeZipInfo(zip_fileinfo& zip_info)
{
	std::memset(&zip_info, 0, sizeof(zip_fileinfo));

	// TODO: Fill the dates
}

static std::string getRandomFileName(const std::string extension)
{
	UUID filename;
	filename.random();
	return (std::string)filename + extension;
}

static bool getBufferFromFile(const std::string& file_name, std::vector<unsigned char>& buffer)
{
	std::ifstream file(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	buffer.resize( file.tellg() );
	file.seekg (0, std::ios::beg);
	file.read((char*)&buffer[0], buffer.size());
	file.close();

	return true;
}


//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptBundleStage::ThorScriptBundleStage() : Stage(), output_file(THORSCRIPT_DEFAULT_BUNDLE_NAME), stripped(false)
{ }

ThorScriptBundleStage::~ThorScriptBundleStage()
{ }

const char* ThorScriptBundleStage::name()
{
	return "thor_script_bundle_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptBundleStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("manifest,m", po::value<std::string>(), "specify manifest file")
		("output,o", po::value<std::string>(), "output bundle file name")
		("extract,d", po::value<std::vector<std::string>>(), "extract bundle to build directory")
		("build-path", po::value<std::string>(), "build directory")
		("strip", "build stripped bundle")
    ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptBundleStage::parseOptions(po::variables_map& vm)
{
	if (vm.count("manifest"))
	{
		manifest_file = vm["manifest"].as<std::string>();
	}

	if (vm.count("input"))
	{
		// Separate files with extension name
		std::vector< std::string > inputs = vm["input"].as< std::vector<std::string> >();
		foreach(i, inputs)
		{
			boost::filesystem::path file_path(*i);
			std::string extension = file_path.extension().generic_string();

			if (extension == THORSCRIPT_BITCODE_EXTENSION)
			{
				bitcode_files.push_back(*i);
			}
			else if (extension == THORSCRIPT_AST_EXTENSION)
			{
				ast_files.push_back(*i);
			}
			else
			{
				other_files.push_back(*i);
			}
		}
	}

	if (vm.count("output"))
	{
		output_file = vm["output"].as<std::string>();
	}

    if (vm.count("extract"))
    {
        bundleDependency = vm["extract"].as<std::vector<std::string>>();
    }

    if (vm.count("build-path"))
    {
        buildPath = vm["build-path"].as<std::string>();
    }

    if (vm.count("strip"))
    {
        stripped = true;
    }

	return true;
}

bool ThorScriptBundleStage::extract(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

    foreach(i, bundleDependency)
    {
        if(!boost::filesystem::exists(*i))
        {
            std::cerr << "Missing bundle file `" << *i << "`" << std::endl;
            return false;
        }
        Archive ar(*i, ArchiveMode::ARCHIVE_FILE_DECOMPRESS);
        ar.open();
        std::vector<ArchiveItem_t> archiveItems;
        std::string bundleSha1Name = sha1::sha1(*i);
        boost::filesystem::path bundlePath = buildPath / bundleSha1Name;
        ar.extractAllToFolder(archiveItems, bundlePath.string());
        ar.close();
    }
    return true;
}

bool ThorScriptBundleStage::compress(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

	// Create bundle
	Archive ar(output_file, ArchiveMode::ARCHIVE_FILE_COMPRESS);

	ArchiveItem_t bc_item;
	ArchiveItem_t ast_item;
	ArchiveItem_t manifest_item;

	// Assign in-zip file name
	bc_item.filename = getRandomFileName(THORSCRIPT_BITCODE_EXTENSION);
	ast_item.filename = getRandomFileName(THORSCRIPT_AST_EXTENSION);
	manifest_item.filename = manifest_file;

	// Initialize zip information
	initializeZipInfo(bc_item.zip_info);
	initializeZipInfo(ast_item.zip_info);
	initializeZipInfo(manifest_item.zip_info);

	// Get buffers from input files
	getMergeBitCodeBuffer(bc_item.buffer);
	getMergeASTBuffer(ast_item.buffer);
	getFileBuffer(manifest_file, manifest_item.buffer);

	// Add to archive
	if (bc_item.buffer.size() > 0) ar.add(bc_item);
	if (ast_item.buffer.size() > 0) ar.add(ast_item);
	if (manifest_item.buffer.size() > 0) ar.add(manifest_item);

	// Archive the rest files
	foreach(i, other_files)
	{
		ArchiveItem_t other_item;

		other_item.filename = *i;
		initializeZipInfo(other_item.zip_info);
		getFileBuffer(*i, other_item.buffer);
		if (other_item.buffer.size() > 0)
		{
			ar.add(other_item);
		}
	}

	return true;
}

bool ThorScriptBundleStage::execute(bool& continue_execution)
{
    if(!bundleDependency.empty())
    {
        return extract(continue_execution);
    }
    else
    {
        return compress(continue_execution);
    }
}

void ThorScriptBundleStage::getMergeBitCodeBuffer(std::vector<unsigned char>& buffer)
{
	/**
	 * The message is output directly from Linker class. If we need to mute it, need to
	 * pass ControlFlags::QuietWarnings|ControlFlags::QuietErrors
	 */
	llvm::LLVMContext& context = llvm::getGlobalContext();
	llvm::Linker linker("ts-bundle", output_file.c_str(), context);

	linker.addSystemPaths();

	// Convert to llvm::sys::Path
	std::vector<llvm::sys::Path> files;
	for (size_t i = 0; i < bitcode_files.size(); i++)
		files.push_back( llvm::sys::Path(bitcode_files[i]) );

	bool error = linker.LinkInFiles(files);
	if (error == true) return;

	// Well, the linker had linked all the bc files, and only the module contained within interests us.
	shared_ptr<llvm::Module> composite(linker.releaseModule());

	llvm::BitstreamWriter stream(buffer);
	llvm::WriteBitcodeToStream(composite.get(), stream);
}

Tangle* ThorScriptBundleStage::getMergedAST(const std::vector<std::string>& ast_files)
{
	using namespace tree;
    Tangle* tangle = NULL;

    for (size_t i = 0; i < ast_files.size(); i++)
    {
        ASTNode* deserialized = ASTSerializationHelper::deserialize(ast_files[i]);

		if(!deserialized || !isa<Tangle>(deserialized)) continue;
       	Tangle* current_package = cast<Tangle>(deserialized);
        if (tangle == NULL)
        {
        	tangle = current_package;
        }
        else
        {
        	// Merge with the previous program
        	tangle->merge(*current_package);
        }
    }
    return tangle;
}

void ThorScriptBundleStage::getMergeASTBuffer(std::vector<unsigned char>& buffer)
{
	using namespace tree;
    Tangle* tangle = getMergedAST(ast_files);

    // TODO: No write to disk
    if (tangle)
    {
        if (stripped)
        {
            zillians::language::stage::visitor::ThorScriptStripStageVisitor stripVisitor;
            stripVisitor.visit(*tangle);
        }

    	std::string temp_filename = THORSCRIPT_AST_TEMP_MERGED_FILE;
		ASTSerializationHelper::serialize(temp_filename, tangle);

		// Read the source buffer
		getBufferFromFile(temp_filename, buffer);
		std::remove(temp_filename.c_str());
    }
}

void ThorScriptBundleStage::getFileBuffer(const std::string& path, std::vector<unsigned char>& buffer)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		buffer.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read((char*)&buffer[0], buffer.size());
		file.close();
	}
}

} } }
