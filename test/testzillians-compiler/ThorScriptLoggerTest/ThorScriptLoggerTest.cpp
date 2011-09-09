/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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

#include "core/Prerequisite.h"
#include "language/logging/LoggingManager.h"
#include "language/logging/StringTable.h"
#include "language/tree/basic/Block.h"
#include <iostream>
#include <string>
#include <limits>

using namespace zillians::language;

int main()
{
	// TODO: We temporarily use setlocale to force the application to use system default locale.
	// TODO: To make compile logger support utf8 encoding.
	setlocale(LC_ALL, "");
	log4cxx::BasicConfigurator::configure();

	LoggingManager log_manager;
	Logger* logger = log_manager.getLogger();

	// Create a fake ASTNode
	zillians::language::tree::Block node;

	logger->log_undefined_variable(_node = node, _ID="mString", _FILE="Super.cpp", _LINE=3);
	logger->log_undefined_variable(_FILE="Super.cpp", _LINE=3, _node = node, _ID="mString");

	logger->log_unused_variable(_ID="x", _node = node);

	return 0;
}
