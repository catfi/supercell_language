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
 *
 * @date Nov 14, 2011 yoco - Initial version created.
 */

#include <queue>
#include "language/logging/LoggerWrapper.h"
#include "language/ThorScriptDriver.h"

#define BOOST_TEST_MODULE ThorScriptDriverTest_Test
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( ThorScriptDriverTest_HappyPathTest )

class MockDriver : public zillians::language::ThorScriptDriver
{
protected:
    virtual int shell(const std::string& cmd)
    {
        BOOST_ASSERT(!shellReturns_.empty());
        cmds.push_back(cmd);

        auto expect = shellReturns_.front();
        shellReturns_.pop();
        if(expect.second)
        {
            system(cmd.c_str());
        }
        return expect.first;
    }

private:
    std::queue<std::pair<int, bool>> shellReturns_;

public:
    void expectCallShell(const int v, const bool callSystem)
    {
        shellReturns_.push({v, callSystem});
    }
    std::vector<std::string> cmds;
};

using boost::unit_test::framework::master_test_suite;

BOOST_AUTO_TEST_CASE( ThorScriptDriverTest_HappyPathTestCase1 )
{
    std::vector<std::string> argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);

	MockDriver driver;
    driver.expectCallShell(0, true);
    driver.expectCallShell(0, true);
    driver.expectCallShell(0, true);
    driver.expectCallShell(0, false);

    argv[1] += "happy1";
    argv[2] += "happy1/build";
	driver.main(argv);

    BOOST_CHECK_EQUAL(driver.cmds.size(), 4);

    BOOST_CHECK_EQUAL(driver.cmds[0].find("ts-bundle -d inbundle/lib1.bundle"), 0);
    BOOST_CHECK_EQUAL(driver.cmds[1].find("ts-dep")                           , 0);
    BOOST_CHECK_EQUAL(driver.cmds[2].find("ts-make")                          , 0);
    BOOST_CHECK_EQUAL(driver.cmds[3].find("ts-link")                          , 0);

    BOOST_CHECK_NE(driver.cmds[3].find("_a.bc"            ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_b.bc"            ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_c1_c2.bc"        ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_c3.bc"           ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_d1.bc"           ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_e11_e12.bc"      ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_e13.bc"          ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("happ1.so"         ), std::string::npos);

    BOOST_CHECK_NE(driver.cmds[3].find("native/native1.o" ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("native/native2.so"), std::string::npos);
}

BOOST_AUTO_TEST_CASE( ThorScriptDriverTest_NoSourceFiles )
{
    std::vector<std::string> argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);
    argv[1] += "no_source_file";
    argv[2] += "no_source_file/build";
	MockDriver driver;
    driver.expectCallShell(3, false);
	driver.main(argv);

    BOOST_CHECK_EQUAL(driver.cmds.size(), 1);
    BOOST_CHECK_EQUAL(driver.cmds[0].find("ts-dep"), 0);
}

BOOST_AUTO_TEST_SUITE_END()
