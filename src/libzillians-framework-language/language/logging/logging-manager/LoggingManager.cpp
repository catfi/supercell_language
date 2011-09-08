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

#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>


namespace zl = zillians::language;

namespace zillians { namespace language { namespace logging {

LoggingManager::LoggingManager() :
		mWarningDegree(0)
{
	mLogger.setLogger(this);
	mLogger.setLocale(get_default_locale());
	mLogger.setStringTable(mStringTable);
}

LoggingManager::~LoggingManager()
{}


uint32 LoggingManager::getCurrentWarningDegree()
{
	return mWarningDegree;
}

void LoggingManager::setWarningDegree(uint32 degree)
{
	mWarningDegree = degree;
}

Logger& LoggingManager::getLogger()
{
	return mLogger;
}

void LoggingManager::logging(uint32 id, std::wstring message)
{
	logging::StringTable::log_id log_id = static_cast<logging::StringTable::log_id>(id);
	BOOST_ASSERT(mStringTable.attribute_table.find(log_id) != mStringTable.attribute_table.end() && "Given wrong Log ID");
	logging::StringTable::attribute_table_t& attribute_table = mStringTable.attribute_table[log_id];

	// Decide the level
	log4cxx::LevelPtr level;
	switch (attribute_table.level)
	{
	case logging::StringTable::LOG_LEVEL_WARNING:
	{
		level = log4cxx::Level::getWarn();
		break;
	}
	case logging::StringTable::LOG_LEVEL_FATAL:
	{
		level = log4cxx::Level::getFatal();
		break;
	}
	case logging::StringTable::LOG_LEVEL_ERROR:
	{
		level = log4cxx::Level::getError();
		break;
	}
	default:
		BOOST_ASSERT(false && "Not defined log type");
		break;
	}

	LOG4CXX_LOG(zl::Logger::CompilerLogger, level, message);
}

}}}

