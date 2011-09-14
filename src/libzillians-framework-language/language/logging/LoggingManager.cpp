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
#include "language/logging/LoggingManager.h"
#include "language/logging/StringTable.h"

#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>


namespace zl = zillians::language;

namespace zillians { namespace language  {

log4cxx::LoggerPtr LoggingManager::Compiler(log4cxx::Logger::getLogger("compiler"));
log4cxx::LoggerPtr LoggingManager::Resolver(log4cxx::Logger::getLogger("compiler.resolver"));
log4cxx::LoggerPtr LoggingManager::CompilerLogger(log4cxx::Logger::getLogger("compiler.compilelogger"));
log4cxx::LoggerPtr LoggingManager::ParserStage(log4cxx::Logger::getLogger("compiler.parser"));
log4cxx::LoggerPtr LoggingManager::TransformerStage(log4cxx::Logger::getLogger("compiler.transformer"));
log4cxx::LoggerPtr LoggingManager::GeneratorStage(log4cxx::Logger::getLogger("compiler.generator"));
log4cxx::LoggerPtr LoggingManager::DebugInfoGeneratorStage(log4cxx::Logger::getLogger("compiler.debuginfogenerator"));
log4cxx::LoggerPtr LoggingManager::VM(log4cxx::Logger::getLogger("vm"));


LoggingManager::LoggingManager() :
		mWarningDegree(0)
{
	mStringTable = new StringTable();
	mLogger = new Logger();

	mLogger->setLogger(this);
	mLogger->setLocale(get_default_locale());
	mLogger->setStringTable(mStringTable);
}

LoggingManager::~LoggingManager()
{
	mLogger->setStringTable(NULL);

	SAFE_DELETE(mStringTable);
	SAFE_DELETE(mLogger);
}

void LoggingManager::initialize()
{
	log4cxx::LogManager::getLoggerRepository()->setConfigured(true);
	log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
	// the default format is "%r [%t] %p %c %x - %m%n"
	// %r -> time
	// %t -> name of the thread
	// %p -> log level
	// %c -> logger name
	// %x -> nested context (usuallh null)
	// %m -> message
	// %n -> new line
	static const log4cxx::LogString default_pattern(LOG4CXX_STR("[%p] [%c] %m%n"));
	log4cxx::LayoutPtr layout(new log4cxx::PatternLayout(default_pattern));
	log4cxx::AppenderPtr appender(new log4cxx::ConsoleAppender(layout));
	root->addAppender(appender);

	// log level: TRACE -> DEBUG -> INFO -> WARN -> ERROR -> FATAL
	root->setLevel(log4cxx::Level::getDebug());
}

uint32 LoggingManager::getCurrentWarningDegree()
{
	return mWarningDegree;
}

void LoggingManager::setWarningDegree(uint32 degree)
{
	mWarningDegree = degree;
}

Logger* LoggingManager::getLogger()
{
	return mLogger;
}

void LoggingManager::log(const uint32 id, const std::wstring& file, const uint32 line, std::wstring& message)
{
	StringTable::log_id log_id = static_cast<StringTable::log_id>(id);
	BOOST_ASSERT(mStringTable->attribute_table.find(log_id) != mStringTable->attribute_table.end() && "Given wrong Log ID");
	StringTable::attribute_table_t& attribute_table = mStringTable->attribute_table[log_id];

	// Decide the level
	log4cxx::LevelPtr level;
	switch (attribute_table.level)
	{
	case StringTable::LEVEL_WARNING:
	{
		level = log4cxx::Level::getWarn();
		break;
	}
	case StringTable::LEVEL_FATAL:
	{
		level = log4cxx::Level::getFatal();
		break;
	}
	case StringTable::LEVEL_ERROR:
	{
		level = log4cxx::Level::getError();
		break;
	}
	default:
		BOOST_ASSERT(false && "Not defined log type");
		break;
	}

	LOG4CXX_LOG(CompilerLogger, level, file << ":" << line << ": " << message);
}

}}

