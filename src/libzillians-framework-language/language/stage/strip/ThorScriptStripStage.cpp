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

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#include <algorithm>
#include <vector>
#include <set>
#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include "language/stage/strip/ThorScriptStripStage.h"
#include "utility/UnicodeUtil.h"

namespace zillians { namespace language { namespace stage {

//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptStripStage::ThorScriptStripStage()
{ }

ThorScriptStripStage::~ThorScriptStripStage()
{ }

const char* ThorScriptStripStage::name()
{
	return "thor_script_strip_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptStripStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options() ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options() ;

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptStripStage::parseOptions(po::variables_map& vm)
{
    if(vm.count("input"))
    {
        inputFiles = vm["input"].as<std::vector<std::string>>();
        return true;
    }
    else
    {
        //std::set<std::string> inputFileSet = globAllTsFiles(".");
        //inputFiles.assign(inputFileSet.begin(), inputFileSet.end());
        return true;
    }
}

bool ThorScriptStripStage::execute(bool& continue_execution)
{
    // strip function definition in ast
    foreach(i, inputFiles)
    {

    }

    return true;
}

} } }
