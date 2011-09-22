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
public:
	StageBuilder()
	{ }

	virtual ~StageBuilder()
	{ }

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

public:
	template<typename TypeVector>
	void addDefaultMode()
	{
		mDefaultMode = [this]{
			BuilderAppender<TypeVector>::build(this);
		};
	}

	template<typename TypeVector>
	void addMode(const std::string& key, const std::string& description)
	{
		std::function<void()> f = [this]{
			BuilderAppender<TypeVector>::build(this);
		};
		mModes[key] = std::make_pair(description, f);
	}

	virtual int main(int argc, const char** argv)
	{
		po::variables_map vm;

		// parse the arguments and append enabled stage accordingly
		foreach(i, mModes)
		{
			mOptionDesc.add_options()(i->first.c_str(), i->second.first.c_str());
		}

	    try
	    {
	    	po::store(po::command_line_parser(argc, argv).options(mOptionDesc).positional(mPositionalOptionDesc).allow_unregistered().run(), vm);
	    	po::notify(vm);
	    }
	    catch(...)
	    {
	    	std::cerr << "failed to parse command line" << std::endl;
	    	// append default stages
	    	if(mDefaultMode) mDefaultMode();
	    	std::cerr << mOptionDesc << std::endl;
	    	return -1;
	    }

	    if(vm.count("help") > 0 || argc < 2)
	    {
	    	std::cout << "command line options: " << std::endl << std::endl;
	    	// append all stages
	    	if(mDefaultMode) mDefaultMode();
	    	std::cout << mOptionDesc << std::endl;
	    	return 0;
	    }

	    bool matched = true;
	    foreach(i, mModes)
	    {
	    	if(vm.count(i->first) > 0)
	    	{
	    		if(matched)
	    		{
	    			std::cerr << "duplicate compilation mode specified" << std::endl;
	    			std::cerr << mOptionDesc << std::endl;
	    			return -1;
	    		}

	    		// invoke the builder to append stages
	    		i->second.second();

	    		matched = true;
	    	}
	    }

	    if(!matched)
	    {
	    	if(mDefaultMode)
	    		mDefaultMode();
	    }

		// call stage conductor main
	    return StageConductor::main(argc, argv);
	}

private:
	std::function<void()> mDefaultMode;
	std::tr1::unordered_map<std::type_info, std::function<void()>> mAllStages;
	std::map<std::string /*key*/, std::pair<std::string /*desc*/, std::function<void()> /*builder lambda*/ > > mModes;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_STAGEBUILDER_H_ */
