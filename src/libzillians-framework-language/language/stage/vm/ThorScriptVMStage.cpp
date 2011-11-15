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

namespace zillians { namespace language { namespace stage {


//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////


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

	option_desc_public->add_options()
		("entry,e", po::value<std::string>(), "Specify the entry function")
    ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptVMStage::parseOptions(po::variables_map& vm)
{

	if (vm.count("input"))
	{
	}

	return true;
}

bool ThorScriptVMStage::execute(bool& continue_execution)
{
	return true;
}


} } }
