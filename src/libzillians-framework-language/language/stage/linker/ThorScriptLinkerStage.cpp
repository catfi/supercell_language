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
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/stage/linker/ThorScriptLinkerStage.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/ASTNodeSerialization.h"

#include "llvm/LLVMContext.h"
#include "llvm/Linker.h"
#include "llvm/Support/PathV1.h"
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"


namespace zillians { namespace language { namespace stage {


//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptLinkerStage::ThorScriptLinkerStage() : Stage()
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
		("output,o", po::value<std::string>(), "output file name");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptLinkerStage::parseOptions(po::variables_map& vm)
{
	if (vm.count("output"))
	{
		output_file = vm["output"].as<std::string>();
	}

	return true;
}

bool ThorScriptLinkerStage::execute(bool& continue_execution)
{
	return true;
}

} } }
