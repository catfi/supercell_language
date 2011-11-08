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

#include "language/ThorScriptDriver.h"
#include "language/stage/driver/ThorScriptDriverStage.h"

using namespace zillians::language::stage;

namespace zillians { namespace language {

//////////////////////////////////////////////////////////////////////////////
// Stataic function
//////////////////////////////////////////////////////////////////////////////

//static bool isDepOnly(const std::string argv)
//{
//    return false;
//}

//static bool isCompileOnly(const std::string argv)
//{
//    return false;
//}

//////////////////////////////////////////////////////////////////////////////
// public member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptDriver::ThorScriptDriver() : stage::StageBuilder(false)
{
	addDefaultMode<
		boost::mpl::vector<
			ThorScriptDriverStage>>();
}

ThorScriptDriver::~ThorScriptDriver()
{ }

void ThorScriptDriver::initialize()
{
}

void ThorScriptDriver::finalize()
{
}

//////////////////////////////////////////////////////////////////////////////
// private member function
//////////////////////////////////////////////////////////////////////////////

bool ThorScriptDriver::dep(const std::string& argv)
{
    UNUSED_ARGUMENT(argv);
    if (system("./ts-dep") == 0) return true;
    return false;
}

bool ThorScriptDriver::make(const std::string& argv)
{
    UNUSED_ARGUMENT(argv);
    if (system("./ts-make") == 0) return true;
    return false;
}

bool ThorScriptDriver::link(const std::string& argv)
{
    UNUSED_ARGUMENT(argv);
    if (system("./ts-link") == 0) return true;
    return false;
}

} }
