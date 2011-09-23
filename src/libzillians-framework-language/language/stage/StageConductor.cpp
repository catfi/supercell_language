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

#include "language/stage/StageConductor.h"
#include "language/logging/LoggerWrapper.h"
#include "utility/Foreach.h"
#include "language/tree/ASTNode.h"
#include "language/context/ConfigurationContext.h"

namespace zillians { namespace language { namespace stage {

StageConductor::StageConductor() : mOptionDescGlobal("General Options")
{
	// make sure logger is initialized;
	LoggerWrapper::instance();

	mOptionDescGlobal.add_options()("help,h", "show help")("input", po::value<std::vector<std::string>>(), "input files");;
	mPositionalOptionDesc.add("input", -1);
}

StageConductor::~StageConductor()
{ }

void StageConductor::appendStage(shared_ptr<Stage> stage)
{
	mStages.push_back(stage);
}

void StageConductor::appendOptionsFromAllStages(po::options_description& options_desc)
{
	foreach(i, mStages)
	{
		std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> options = (*i)->getOptions();
		options_desc.add(*options.first);
	}
}

int StageConductor::main(int argc, const char** argv)
{
	// initialize the global configuration context
	ConfigurationContext* config = new ConfigurationContext();
	setConfigurationContext(config);

	if(true)
	{
		po::options_description options_desc_pub("Usage");
		po::options_description options_desc_pri("Usage");

		// construct public and private options
		options_desc_pub.add(mOptionDescGlobal);
		options_desc_pri.add(mOptionDescGlobal);
		foreach(i, mStages)
		{
			std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> options = (*i)->getOptions();
			if(options.first->options().size() > 0)
				options_desc_pub.add(*options.first);
			if(options.second->options().size() > 0)
				options_desc_pri.add(*options.second);
		}

		// try to parse the command line
		po::variables_map vm;
	    try
	    {
	    	po::store(po::command_line_parser(argc, argv).options(options_desc_pri).positional(mPositionalOptionDesc).run(), vm);
	    	po::notify(vm);
	    }
	    catch(const boost::program_options::error& e)
	    {
	    	std::cerr << "failed to parse command line: " << e.what() << std::endl;
	    	std::cerr << options_desc_pub << std::endl;
	    	return -1;
	    }

	    // if help option is specified, print the options and exit
	    if(vm.count("help") > 0 || argc < 2)
	    {
	    	std::cout << "command line options: " << std::endl << std::endl;
	    	std::cout << options_desc_pub << std::endl;
	    	return 0;
	    }

	    // otherwise, process the given commands through each staged execution

	    // let each stage determine what to do
		foreach(stage, mStages)
		{
			if(!(*stage)->parseOptions(vm))
			{
				std::cerr << "failed to process command option for \"" << (*stage)->name() << "\"" << std::endl;
				std::cerr << options_desc_pub << std::endl;
				return -1;
			}
		}

		// perform the execution
		foreach(stage, mStages)
		{
			bool c = true;

			if(!(*stage)->execute(c))
			{
				std::cerr << "execution failed at stage: " << (*stage)->name() << std::endl;
				return -1;
			}

			if(!c) break;
		}
	}

	return 0;
}

} } }
