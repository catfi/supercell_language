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

#include "utility/UnicodeUtil.h"
#include "language/logging/Logger.h"
#include "language/logging/logging-manager/LoggingManager.h"

namespace zillians { namespace language { namespace logging {

LoggingManager::LoggingManager() :
		mWarningLevel(0)
{
	mLogger.setLogger(this);
	mLogger.setLocale(get_default_locale());
	mLogger.setStringTable(mStringTable);
}

LoggingManager::~LoggingManager()
{}


uint32 LoggingManager::getCurrentWarningLevel()
{
	return mWarningLevel;
}

void LoggingManager::setWarningLevel(uint32 level)
{
	mWarningLevel = level;
}

Logger& LoggingManager::getLogger()
{
	return mLogger;
}

void LoggingManager::logging(std::wstring message)
{
	LOG4CXX_DEBUG(zillians::language::Logger::CompilerLogger, message);
}

}}}

