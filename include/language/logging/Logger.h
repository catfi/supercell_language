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

namespace zillians { namespace language {

struct Logger
{
	static void initialize();

	static log4cxx::LoggerPtr Compiler;
	static log4cxx::LoggerPtr Resolver;
	static log4cxx::LoggerPtr BasicStage;
	static log4cxx::LoggerPtr ParserStage;
	static log4cxx::LoggerPtr TransformerStage;
	static log4cxx::LoggerPtr GeneratorStage;
	static log4cxx::LoggerPtr VM;
};

} }

#endif /* ZILLIANS_LANGUAGE_LOGGER_H_ */
