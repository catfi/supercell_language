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

#include "utility/archive/Archive.h"
#include "language/stage/bundle/ThorScriptBundleStage.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/ASTNodeSerialization.h"


namespace zillians { namespace language { namespace stage {

#define THORSCRIPT_BITCODE_EXTENSION	".bc"
#define THORSCRIPT_AST_EXTENSION		".ast"
#define THORSCRIPT_DEFAULT_BUNDLE_NAME	"output.bundle"


//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////

void initializeZipInfo(zip_fileinfo& zip_info)
{
	std::memset(&zip_info, 0, sizeof(zip_fileinfo));

	// TODO: Fill the dates
}

std::string getRandomFileName(std::string extension)
{
	UUID filename;
	filename.random();
	return (std::string)filename + extension;
}


//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptBundleStage::ThorScriptBundleStage() : Stage(), output_file(THORSCRIPT_DEFAULT_BUNDLE_NAME)
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
		("output,o", po::value<std::string>(), "output bundle file name");

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
		for (int i = 0; i < inputs.size(); i++)
		{
			boost::filesystem::path file_path(inputs[i]);
			std::string extension = file_path.extension().generic_string();

			if (extension == THORSCRIPT_BITCODE_EXTENSION)
			{
				bitcode_files.push_back(inputs[i]);
			}
			else if (extension == THORSCRIPT_AST_EXTENSION)
			{
				ast_files.push_back(inputs[i]);
			}
		}
	}

	if (vm.count("output"))
	{
		output_file = vm["output"].as<std::string>();
	}

	return true;
}

bool ThorScriptBundleStage::execute(bool& continue_execution)
{
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
	getManifestBuffer(manifest_item.buffer);

	// Add to archive
	ar.open();
	if (bc_item.buffer.size() > 0) ar.add(bc_item);
	if (ast_item.buffer.size() > 0) ar.add(ast_item);
	if (manifest_item.buffer.size() > 0) ar.add(manifest_item);
	ar.close();

	return true;
}

void ThorScriptBundleStage::getMergeBitCodeBuffer(std::vector<unsigned char>& buffer)
{

}

void ThorScriptBundleStage::getMergeASTBuffer(std::vector<unsigned char>& buffer)
{
	using namespace tree;
    ASTNode* program = NULL;

    for (int i = 0; i < ast_files.size(); i++)
    {
    	ASTNode* current_program = NULL;

        std::ifstream ifs(ast_files[i]);
        boost::archive::text_iarchive ia(ifs);
        ia >> current_program;

        if (program == NULL)
        {
        	program = current_program;
        }
        else
        {
        	// Merge with the previous program

        }
    }

    // Serialize to ostream

}

void ThorScriptBundleStage::getManifestBuffer(std::vector<unsigned char>& buffer)
{
	std::ifstream file(manifest_file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	buffer.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read((char*)&buffer[0], buffer.size());
	file.close();
}

} } }
