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

#ifndef ZILLIANS_LANGUAGE_LOGGER_H_
#define ZILLIANS_LANGUAGE_LOGGER_H_

#include "core/Prerequisite.h"
#include "core/Singleton.h"

namespace zillians { namespace compiler {

template<typename Iterator, typename Code>
struct Logger : Singleton<Logger<Iterator, Code>, SingletonInitialization::automatic>
{
	struct Level
	{
		enum type {
			FATAL   = 0,
			ERROR   = 1,
			WARNING = 2,
			INFO    = 3,
			DEBUG   = 4,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case FATAL: return L"fatal";
			case ERROR: return L"error";
			case WARNING: return L"warning";
			case INFO: return L"info";
			case DEBUG: return L"debug";
			}
		}
	};

	Logger()
	{ }

	void setLevel(Level::type level)
	{
		mCurrentLevel = level;
	}

	Logger<Iterator, Code>& operator() (Level::type level, Iterator& iterator, const std::wstring& message)
	{
		if(level <= mCurrentLevel)
		{
			mMessages.push_back(std::make_pair(iterator, message));
		}
		return *this;
	}

private:
	Level::type mCurrentLevel;
	std::vector<std::pair<Iterator, std::wstring> > mMessages;
};

} }

#endif /* ZILLIANS_LANGUAGE_LOGGER_H_ */
