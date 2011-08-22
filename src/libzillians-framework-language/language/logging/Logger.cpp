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

#include "language/logging/Logger.h"

#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>

namespace zillians { namespace language {

log4cxx::LoggerPtr Logger::Compiler(log4cxx::Logger::getLogger("compiler"));
log4cxx::LoggerPtr Logger::Resolver(log4cxx::Logger::getLogger("resolver"));
log4cxx::LoggerPtr Logger::ParserStage(log4cxx::Logger::getLogger("compiler.parser"));
log4cxx::LoggerPtr Logger::TransformerStage(log4cxx::Logger::getLogger("compiler.transformer"));
log4cxx::LoggerPtr Logger::GeneratorStage(log4cxx::Logger::getLogger("compiler.generator"));
log4cxx::LoggerPtr Logger::VM(log4cxx::Logger::getLogger("vm"));

void Logger::initialize()
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
	static const log4cxx::LogString default_pattern(LOG4CXX_STR("%m%n"));
	log4cxx::LayoutPtr layout(new log4cxx::PatternLayout(default_pattern));
	log4cxx::AppenderPtr appender(new log4cxx::ConsoleAppender(layout));
	root->addAppender(appender);
}

} }
