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
public:
    enum class RunSystem { RUN, DONT_RUN } ;

public:

    /**
     * @brief             Register an expected @p system() call event
     * @param cmd         The command string to expected to be called
     * @param returnValue The return value of the @p system() call.
     * @param run         Actually apply the @p system() call or not.
     */
    void expectCallShell(const std::string& cmd, const int returnValue, const RunSystem run)
    {
        ExpectCallRecord e(cmd, returnValue, run);
        expectedCalls.push(e);
    }

    std::vector<std::string> cmds;

protected:
    virtual int shell(const std::string& cmd)
    {
        BOOST_ASSERT(!expectedCalls.empty());
        cmds.push_back(cmd);

        ExpectCallRecord expectRecord = expectedCalls.front();
        expectedCalls.pop();

        BOOST_CHECK_EQUAL(cmd.find(expectRecord.cmd), 0);

        if(expectRecord.run == RunSystem::RUN)
        {
            system(cmd.c_str());
        }

        return expectRecord.returnValue;
    }

private:
    struct ExpectCallRecord
    {
        ExpectCallRecord(const std::string& cmd_, const int returnValue_, const RunSystem run_) : cmd(cmd_), returnValue(returnValue_), run(run_) {}
        const std::string  cmd;
        const int          returnValue;
        const RunSystem    run;
    } ;

    std::queue<ExpectCallRecord> expectedCalls;
};

using boost::unit_test::framework::master_test_suite;

BOOST_AUTO_TEST_CASE( ThorScriptDriverTest_HappyPathTestCase1 )
{
    // build lib1
    {
        std::vector<std::string> lib1Argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);
        lib1Argv[1] += "lib1";
        lib1Argv[2] += "lib1/build";
        zillians::language::ThorScriptDriver lib1Driver;
        lib1Driver.main(lib1Argv);
    }

    // generate lib1.bundle
    {
        std::vector<std::string> lib1Argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);
        lib1Argv[1] += "lib1";
        lib1Argv[2] += "lib1/build";
        zillians::language::ThorScriptDriver lib1Driver;
        lib1Argv.push_back("generate");
        lib1Argv.push_back("bundle");
        lib1Driver.main(lib1Argv);
    }

    // copy lib1.bundle to project path of happytest
    {
        std::vector<std::string> argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);
        boost::filesystem::path lib1ProjectPath(argv[1].substr(15) + "lib1");
        boost::filesystem::path lib1BuildPath(argv[2].substr(13) + "lib1/build");
        boost::filesystem::create_directories(lib1ProjectPath / ".." / "happy1" / "inbundle");
        // should use boost::filesystem::copy_file(), however, this function has different prototype in c++0x,
        // see http://boost.2283326.n4.nabble.com/Filesystem-problems-with-g-std-c-0x-td2639716.html
        //boost::filesystem::copy_file(lib1ProjectPath / "build" / "bin" / "lib1.bundle", lib1ProjectPath / ".." / "happy1" / "inbundle" / "lib1.bundle");
        std::string cmd = "cp " + lib1BuildPath.string() + "/bin/lib1.bundle " + lib1ProjectPath.string() + "/../happy1/inbundle/lib1.bundle";
        system(cmd.c_str());
    }

    //////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);
    argv[1] += "happy1";
    argv[2] += "happy1/build";

	MockDriver driver;
    driver.expectCallShell("ts-bundle -d inbundle/lib1.bundle", 0, MockDriver::RunSystem::RUN);
    driver.expectCallShell("ts-dep"                           , 0, MockDriver::RunSystem::RUN);
    driver.expectCallShell("ts-make"                          , 0, MockDriver::RunSystem::RUN);
    driver.expectCallShell("ts-link"                          , 0, MockDriver::RunSystem::DONT_RUN);

	driver.main(argv);

    BOOST_CHECK_EQUAL(driver.cmds.size(), 4);

    BOOST_CHECK_NE(driver.cmds[3].find("_a.bc"            ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_b.bc"            ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_c1_c2.bc"        ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_c3.bc"           ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_d1.bc"           ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_e11_e12.bc"      ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("_e13.bc"          ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("happy1.so"        ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("native/native1.o" ), std::string::npos);
    BOOST_CHECK_NE(driver.cmds[3].find("native/native2.so"), std::string::npos);
}

BOOST_AUTO_TEST_CASE( ThorScriptDriverTest_NoSourceFiles )
{
    std::vector<std::string> argv(master_test_suite().argv, master_test_suite().argv + master_test_suite().argc);
    argv[1] += "no_source_file";
    argv[2] += "no_source_file/build";

	MockDriver driver;
    driver.expectCallShell("ts-dep", 3, MockDriver::RunSystem::DONT_RUN);

	driver.main(argv);

    BOOST_CHECK_EQUAL(driver.cmds.size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
