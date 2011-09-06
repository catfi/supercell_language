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
#include "language/logging/logging-manager/LoggingManager.h"

namespace zillians { namespace language { namespace logging {

LoggingManager::LoggingManager()
{
	mLogger.setLocale(get_default_locale());
	mLogger.setStringTable(mStringTable);
}

LoggingManager::~LoggingManager()
{}


void LoggingManager::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{

}

bool LoggingManager::parseOptions(po::variables_map& vm)
{

}

Logger& LoggingManager::getLogger()
{
	return mLogger;
}

}}}

