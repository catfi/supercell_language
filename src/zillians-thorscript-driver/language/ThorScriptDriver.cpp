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

#include <iostream>
#include <fstream>

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "core/Types.h"
#include "utility/Foreach.h"
#include "language/ThorScriptManifest.h"
#include "language/ThorScriptDriver.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/stage/bundle/ThorScriptBundleStage.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"

namespace zillians { namespace language {

//////////////////////////////////////////////////////////////////////////////
// static function
//////////////////////////////////////////////////////////////////////////////

static void saveCache(const std::string& s)
{
    std::ofstream fout("cache");
    fout << s ;
    fout.close();
}

static std::string readCache()
{
    std::ifstream fin("cache");
    if (fin.is_open()) {
        std::string s;
        fin >> s ;
        fin.close();
        return s;
    }
    else
    {
        return "debug";
    }
}

static std::string joinArgs(const std::vector<std::string>& argv)
{
    std::string result;
    for (size_t i = 1; i != argv.size(); ++i)
    {
        if(i != 1) result += " ";
        result += argv[i];
    }
    return result;
}

static boost::filesystem::path upToManifestDir()
{
    boost::filesystem::path p(".");
    p = boost::filesystem::absolute(p);
    while(!p.empty())
    {
        if(boost::filesystem::exists(p / "manifest.xml"))
        {
            return p;
        }
        else
        {
            p = p.parent_path();
        }
    }
    return "";
}

static void printHelpPage()
{
    std::cout << "tsc project create app_name     create a folder named \"app_name\" and also\n"
                 "                                the basic structure for it.\n"
                 "                                build, src/, manifest.xml, README\n"
                 "                                future work, init git repo\n"
                 "\n"
                 "tsc build release               by defalut tsc will read manifest, invoke\n"
                 "                                ts-dep, ts-make, ts-compile, and ts-link to\n"
                 "                                generate the final shared object\n"
                 "                                (in release mode)\n"
                 "\n"
                 "tsc build debug                 by defalut tsc will read manifest, invoke\n"
                 "                                ts-dep, ts-make, ts-compile, and ts-link to\n"
                 "                                generate the final shared object\n"
                 "                                (in debug mode)\n"
                 "\n"
                 "ts build                        by default will build the last configuration;\n"
                 "                                if there's no last build, it build debug\n"
                 "                                version; so if we issue tsc build release and\n"
                 "                                the issue tsc build, it will equal to tsc build\n"
                 "                                release\n"
                 "\n"
                 "tsc                             by default will invoke tsc build\n"
                 "\n"
                 "tsc generate bundle [--strip]  create bundle file\n"
                 "\n"
                 "tsc generate client-stub [java|c++|...]\n"
                 "\n"
                 "tsc generate server-stub\n"
                 "\n"
                 "The result of deployment is managed by a git server; for each instance, we\n"
                 "will create a git url got it so that user can set remote and push to that\n"
                 "remote to deploy his code.\n"
    ;
}

//////////////////////////////////////////////////////////////////////////////
// public member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptDriver::ThorScriptDriver() : dumpCommand(false)
{
}

//////////////////////////////////////////////////////////////////////////////
// protected member function
//////////////////////////////////////////////////////////////////////////////

int ThorScriptDriver::shell(const std::string& cmd)
{
    if(dumpCommand)
    {
        std::cerr << "tsc call shell: `" << cmd << "`" << std::endl ;
    }

    return system(cmd.c_str());
}

//////////////////////////////////////////////////////////////////////////////
// private member function
//////////////////////////////////////////////////////////////////////////////

std::vector<std::string> ThorScriptDriver::getAstUnderBuild()
{
    std::vector<std::string> result;
    namespace fs = boost::filesystem;
    for(auto i = fs::directory_iterator(buildPath); i != fs::directory_iterator(); ++i)
    {
        if(i->path().extension() == ".ast")
        {
            result.push_back(i->path().string());
        }
    }
    return result;
}

bool ThorScriptDriver::setProjectPathAndBuildPath(std::vector<std::string>& argv)
{
    const std::string dStr = "--dump-command";
    projectPath = upToManifestDir();
    foreach(i, argv)
    {
        if(i->find(dStr) == 0)
        {
            dumpCommand = true;
            argv.erase(i);
            break;
        }
    }

    const std::string pStr = "--project-path=";
    projectPath = upToManifestDir();
    foreach(i, argv)
    {
        if(i->find(pStr) == 0)
        {
            projectPath = i->substr(pStr.size());
            argv.erase(i);
            break;
        }
    }

    if(projectPath == "")
    {
        std::cerr << "Not under a project directory" << std::endl;
        return false;
    }

    buildPath = "./build";
    const std::string bStr = "--build-path=";
    foreach(i, argv)
    {
        if(i->find(bStr) == 0)
        {
            buildPath = i->substr(bStr.size());
            buildPath = boost::filesystem::absolute(buildPath);
            argv.erase(i);
            break;
        }
    }

    boost::filesystem::create_directories(buildPath);
    std::cout << "Project director: " << this->projectPath << std::endl ;
    std::cout << "Build   director: " << this->buildPath << std::endl ;
    boost::filesystem::current_path(projectPath);
    return true;
}

bool ThorScriptDriver::main(std::vector<std::string> argv)
{
    if(argv[1] == "--help")
    {
        printHelpPage();
        return true;
    }

    if(argv.size() == 4 &&
       argv[1] == "project" &&
       argv[2] == "create")
    {
        return createProjectSkeleton(argv[3]);
    }

    if(!setProjectPathAndBuildPath(argv))
    {
        return false;
    }

    pm.load("manifest.xml");

    std::string arg = joinArgs(argv);

    if(arg == "")
    {
        return build();
    }
    else if(arg == "build")
    {
        return build();
    }
    else if(arg == "build debug")
    {
        return buildDebug();
    }
    else if(arg == "build release")
    {
        return buildRelease();
    }
    else if(arg == "generate bundle")
    {
        return generateBundle(ThorScriptDriver::STRIP_TYPE::NO_STRIP);
    }
    else if(arg == "generate bundle --strip")
    {
        return generateBundle(ThorScriptDriver::STRIP_TYPE::STRIP);
    }
    else if(arg == "generate client-stub java")
    {
        return generateClientStub(ThorScriptDriver::STUB_LANG::JAVA);
    }
    else if(arg == "generate client-stub c++")
    {
        return generateClientStub(ThorScriptDriver::STUB_LANG::CPP);
    }
    else if(arg == "generate server-stub")
    {
        return generateServerStub();
    }
    else
    {
        printHelpPage();
        return false;
    }
}

bool ThorScriptDriver::createProjectSkeleton(const std::string& projectName)
{
    // check if current dir is a project dir already
    if(boost::filesystem::exists(projectName))
    {
        std::cerr << "directory `" << projectName << "` already exists" << std::endl;
        return false;
    }

    boost::filesystem::path projectPath = projectName;
    boost::filesystem::create_directory(projectPath);
    boost::filesystem::path projectSrcPath = projectPath / "src";
    boost::filesystem::create_directory(projectSrcPath);

    boost::filesystem::path manifestPath = projectPath / "manifest.xml";
    boost::filesystem::ofstream fout(manifestPath);

    if(!fout.is_open())
    {
        std::cerr << "Can not open file `" << manifestPath.string() << "` to write" << std::endl;
        return false;
    }

    fout << "<project name=\"" << projectName << "\" author=\"author\" version=\"0.0.0.1\">\n"
         << "    <dependency>\n"
         << "        <!-- bundle         path=\"./in_bundle/a.bundle\" -->\n"
         << "        <!-- native_object  path=\"./native/b.o\"         -->\n"
         << "        <!-- native_library path=\"./native/c.a\"         -->\n"
         << "    </dependency>\n"
         << "</project>\n" ;

    fout.close();

    return true;
}

bool ThorScriptDriver::buildDebug()
{
    saveCache("debug");

    if (!unbundle()                               ) return false;
    if (!dep()                                    ) return false;
    if (!make(ThorScriptDriver::BUILD_TYPE::DEBUG)) return false;
    if (!link()                                   ) return false;

    return true;
}

bool ThorScriptDriver::buildRelease()
{
    saveCache("release");

    if (!unbundle()                                 ) return false;
    if (!dep()                                      ) return false;
    if (!make(ThorScriptDriver::BUILD_TYPE::RELEASE)) return false;
    if (!link()                                     ) return false;

    return true;
}

bool ThorScriptDriver::build()
{
    std::string s = readCache();
    if (s == "release")
    {
        buildRelease();
    }
    else
    {
        buildDebug();
    }
    return true;
}

bool ThorScriptDriver::generateBundle(const ThorScriptDriver::STRIP_TYPE isStrip)
{
    UNUSED_ARGUMENT(isStrip);

    std::string cmd("ts-bundle -m manifest.xml");
    cmd += " -o " + (buildPath / "bin" / (pm.name + ".bundle")).string();
    std::vector<std::string> astFiles = getAstUnderBuild();
    foreach(i, astFiles)
    {
        cmd += " ";
        cmd += *i;
    }

    if(isStrip == ThorScriptDriver::STRIP_TYPE::STRIP)
    {
        cmd += " --strip";
    }

    if(shell(cmd) == 0) return true;
    else                         return false;
}

bool ThorScriptDriver::generateStub(const std::vector<std::string>& stubTypes)
{
    boost::filesystem::path stubPath = buildPath / "stub";
    boost::filesystem::create_directories(stubPath);
    stage::ThorScriptBundleStage bundler;
    tree::Tangle* tangle = bundler.getMergedAST(getAstUnderBuild());
    boost::filesystem::path stubAstPath = buildPath / ("stub-" + pm.name + ".ast");
    stage::ASTSerializationHelper::serialize(stubAstPath.string(), tangle);

    foreach(i, stubTypes)
    {
        std::string cmd = "ts-stub " +
                          stubAstPath.string() +
                          " --output-path=" + stubPath.string() +
                          " --stub-type=" + *i +
                          " --game-name=" + pm.name;
        if(shell(cmd))
        {
            return false;
        }
    }
    return true;
}

bool ThorScriptDriver::generateClientStub(const ThorScriptDriver::STUB_LANG lang)
{
    UNUSED_ARGUMENT(lang);

    return generateStub({"CLIENT_CLIENTSTUB_H",
                         "CLIENT_GAMESERVICE_CPP",
                         "CLIENT_GAMESERVICE_H"});
}

bool ThorScriptDriver::generateServerStub()
{
    return generateStub({"GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H",
                         "GATEWAY_GAMECOMMAND_CLOUDCOMMANDOBJECT_H",
                         "GATEWAY_GAMECOMMAND_GAMECOMMANDTRANSLATOR_CPP",
                         "GATEWAY_GAMECOMMAND_GAMEMODULE_MODULE"});
    return true;
}

//////////////////////////////////////////////////////////////////////////////

bool ThorScriptDriver::unbundle()
{
    zillians::language::ProjectManifest projectManifest;
    projectManifest.load("manifest.xml");

    foreach(i, projectManifest.dep.bundles)
    {
        std::string cmd = "ts-bundle -d " + *i + " --build-path=" + buildPath.string();
        if(shell(cmd) != 0)
        {
            std::cerr << "ERROR unbundle step fail: " << cmd << std::endl;
            return false;
        }
    }

    return true;
}

bool ThorScriptDriver::dep()
{
    if(shell("ts-dep --build-path=" + buildPath.string()) != 0)
    {
        std::cerr << "ERROR dep step fail: " << "ts-dep" << std::endl;
        return false;
    }
    return true;
}

bool ThorScriptDriver::make(const ThorScriptDriver::BUILD_TYPE type)
{
    // TODO ts-make should take option --debug --release
    std::string cmd = "ts-make --build-path=" + buildPath.string();
    if(dumpCommand)
    {
        cmd += " --dump-command";
    }
    if(type == ThorScriptDriver::BUILD_TYPE::DEBUG)
    {
        cmd += " --debug";
    }
    else
    {
        cmd += " --release";
    }
    if(shell(cmd) != 0)
    {
        std::cerr << "ERROR make step fail: " << cmd << std::endl;
        return false;
    }
    return true;
}

bool ThorScriptDriver::bundle()
{
    boost::filesystem::create_directories(buildPath / "bin");
    boost::filesystem::path bundlePath = buildPath / "bin" / (pm.name + ".bundle");
    std::string cmd = "ts-bundle -m manifest -o " + bundlePath.string();;
    if(shell(cmd) != 0)
    {
        std::cerr << "ERROR bundle step fail: " << "ts-bundle" << std::endl;
        return false;
    }
    return true;
}

bool ThorScriptDriver::strip()
{
    if(shell("ts-strip") != 0)
    {
        std::cerr << "ERROR strip step fail: " << "ts-strip" << std::endl;
        return false;
    }
    return true;
}

bool ThorScriptDriver::link()
{
    namespace fs = boost::filesystem;

    fs::create_directories(buildPath / "bin");
    fs::path soPath = buildPath / "bin" / (pm.name + ".so");
    std::string cmd = "ts-link -o " + soPath.string();
    for(auto i = fs::directory_iterator(buildPath); i != fs::directory_iterator(); ++i)
    {
        if(i->path().extension() == ".bc")
        {
            cmd += " ";
            cmd += i->path().string();
        }
    }
    foreach(i, this->pm.dep.native_objects)
    {
        cmd += " ";
        cmd += *i;
    }
    foreach(i, this->pm.dep.native_libraries)
    {
        cmd += " ";
        cmd += *i;
    }
    if(shell(cmd) != 0)
    {
        return false;
    }
    return true;
}

} }
