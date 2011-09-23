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

#ifndef ZILLIANS_LANGUAGE_STAGE_STAGEBUILDER_H_
#define ZILLIANS_LANGUAGE_STAGE_STAGEBUILDER_H_

#include "core/Prerequisite.h"
#include "language/stage/Stage.h"
#include "language/stage/StageConductor.h"
#include "utility/Foreach.h"
#include <functional>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/vector.hpp>

namespace zillians { namespace language { namespace stage {

class StageBuilder : public StageConductor
{
private:
	template<int N, typename Types>
	struct BuilderAppenderImpl : public BuilderAppenderImpl<N-1, Types>
	{
		static void build(StageConductor* conductor)
		{
			BuilderAppenderImpl<N-1, Types>::build(conductor);
			typedef typename boost::mpl::at<Types, boost::mpl::int_<N> >::type T;
			conductor->appendStage(shared_ptr<Stage>(new T()));
		}
	};

	template<typename Types>
	struct BuilderAppenderImpl<0, Types>
	{
		static void build(StageConductor* conductor)
		{
			typedef typename boost::mpl::at<Types, boost::mpl::int_<0> >::type T;
			conductor->appendStage(shared_ptr<Stage>(new T()));
		}
	};

	template<typename Types>
	struct BuilderAppender : public BuilderAppenderImpl< boost::mpl::size<Types>::value - 1, Types>
	{
		static void build(StageConductor* conductor)
		{
			BuilderAppenderImpl< boost::mpl::size<Types>::value - 1, Types>::build(conductor);
		}
	};

private:
	template<int N, typename Types>
	struct AllStageAppenderImpl : public AllStageAppenderImpl<N-1, Types>
	{
		static void append(std::map<std::string, std::function<void()>>& m, StageConductor* conductor)
		{
			AllStageAppenderImpl<N-1, Types>::append(m, conductor);
			typedef typename boost::mpl::at<Types, boost::mpl::int_<N> >::type T;
			std::string s(typeid(T).name());
			if(m.find(s) == m.end())
			{
				std::function<void()> f = [=]{
					conductor->appendStage(shared_ptr<Stage>(new T()));
				};
				m.insert(std::make_pair(s, f));
			}
		}
	};

	template<typename Types>
	struct AllStageAppenderImpl<0, Types>
	{
		static void append(std::map<std::string, std::function<void()>>& m, StageConductor* conductor)
		{
			typedef typename boost::mpl::at<Types, boost::mpl::int_<0> >::type T;
			std::string s(typeid(T).name());
			if(m.find(s) == m.end())
			{
				std::function<void()> f = [=]{
					conductor->appendStage(shared_ptr<Stage>(new T()));
				};
				m.insert(std::make_pair(s, f));
			}
		}
	};

	template<typename Types>
	struct AllStageAppender : public AllStageAppenderImpl< boost::mpl::size<Types>::value - 1, Types>
	{
		static void append(std::map<std::string, std::function<void()>>& m, StageConductor* conductor)
		{
			AllStageAppenderImpl< boost::mpl::size<Types>::value - 1, Types>::append(m, conductor);
		}
	};

public:
	template<typename TypeVector>
	void addDefaultMode()
	{
		mDefaultMode = [=]{
			BuilderAppender<TypeVector>::build(this);
		};

		AllStageAppender<TypeVector>::append(mAllStages, this);
	}

	template<typename TypeVector>
	void addMode(const std::string& key, const std::string& description)
	{
		std::function<void()> f = [=]{
			BuilderAppender<TypeVector>::build(this);
		};
		mModes[key] = std::make_pair(description, f);
		AllStageAppender<TypeVector>::append(mAllStages, this);
	}

	virtual int main(int argc, const char** argv)
	{
		po::options_description options_desc;
		po::options_description options_desc_modes;

		// construct the options description
		foreach(i, mModes)
		{
			mOptionDescGlobal.add_options()(i->first.c_str(), i->second.first.c_str());
		}
		options_desc.add(options_desc_modes);
		options_desc.add(mOptionDescGlobal);

		// parse the arguments
		po::variables_map vm;
	    try
	    {
	    	po::store(po::command_line_parser(argc, argv).options(options_desc).positional(mPositionalOptionDesc).allow_unregistered().run(), vm);
	    	po::notify(vm);
	    }
	    catch(const boost::program_options::error& e)
	    {
	    	std::cerr << "failed to parse command line: " << e.what() << std::endl;

	    	// append all stages
	    	appendAllStages();
	    	appendOptionsFromAllStages(options_desc);

	    	std::cerr << options_desc << std::endl;
	    	return -1;
	    }

	    // if help option is specified, print the options and exit
	    if(vm.count("help") > 0 || argc < 2)
	    {
	    	appendAllStages();
	    	appendOptionsFromAllStages(options_desc);

	    	std::cout << "command line options: " << std::endl << std::endl;
	    	std::cout << options_desc << std::endl;
	    	return 0;
	    }

	    bool matched = false;
	    foreach(i, mModes)
	    {
	    	if(vm.count(i->first) > 0)
	    	{
	    		if(matched)
	    		{
	    			std::cerr << "duplicate compilation mode specified" << std::endl;
	    			std::cerr << options_desc << std::endl;
	    			return -1;
	    		}

	    		// invoke the builder to append stages
	    		i->second.second();
	    		LOG4CXX_DEBUG(LoggerWrapper::Compiler, "using " << i->second.first << " mode");

	    		matched = true;
	    	}
	    }

	    // if no mode is specified, use default mode
	    if(!matched)
	    {
	    	LOG4CXX_DEBUG(LoggerWrapper::Compiler, "using default mode");
	    	mDefaultMode();
	    }

		// call stage conductor main
	    return StageConductor::main(argc, argv);
	}

private:
	void appendAllStages()
	{
		foreach(i, mAllStages)
		{
			i->second();
		}
	}

private:
	std::function<void()> mDefaultMode;
	std::map<std::string, std::function<void()>> mAllStages;
	std::map<std::string /*key*/, std::pair<std::string /*desc*/, std::function<void()> /*builder lambda*/ > > mModes;
	po::options_description mOptionDescMode;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_STAGEBUILDER_H_ */
