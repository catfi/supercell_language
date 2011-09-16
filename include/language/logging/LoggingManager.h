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
 
#ifndef ZILLIANS_LANGUAGE_LOGGING_MANAGER_H_
#define ZILLIANS_LANGUAGE_LOGGING_MANAGER_H_

#include "core/Prerequisite.h"
#include "core/Singleton.h"

namespace zillians { namespace language {

class StringTable;
class Logger;

/**
 * LoggingManager provides the following features:
 *
 * 1. Parse options: which includes,
 *    a. Set ignore warning types
 *    b. Set warning degree
 * 2. Query the level for specific warning message
 * 3. Compile stage assert if the parser mis-used the specific the function which comes from the message type.
 * 4. Output the specific warning or error messages according to the different locale setting
 *
 */
class LoggingManager : public Singleton<LoggingManager, SingletonInitialization::automatic>
{
	friend class Singleton;

public:
	LoggingManager();
	virtual ~LoggingManager();
	uint32 getCurrentWarningDegree();
	void setWarningDegree(uint32 level);

	Logger* getLogger();

public:
	virtual void log(const uint32 id, const std::wstring& file, const uint32 line, std::wstring& message);

public:
	static void initialize();

	static log4cxx::LoggerPtr Compiler;
	static log4cxx::LoggerPtr Resolver;
	static log4cxx::LoggerPtr CompilerLogger;
	static log4cxx::LoggerPtr BasicStage;
	static log4cxx::LoggerPtr ParserStage;
	static log4cxx::LoggerPtr TransformerStage;
	static log4cxx::LoggerPtr GeneratorStage;
	static log4cxx::LoggerPtr DebugInfoGeneratorStage;
	static log4cxx::LoggerPtr VM;

private:
	StringTable* mStringTable;
	Logger* mLogger;
	uint32 mWarningDegree;
};

}}

#endif /* ZILLIANS_LANAGUAGE_LOGGING_MANAGER_H_ */
