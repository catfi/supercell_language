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

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#include "threading/JoinFunctionModule.h"
#include <algorithm>
#include <vector>
#include <set>
#include <iterator>
#include <boost/any.hpp>
#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/filesystem.hpp>
#include "language/stage/make/ThorScriptMakeStage.h"
#include "language/stage/dep/ThorScriptSourceTangleGraph.h"
#include "utility/UnicodeUtil.h"
#include "utility/sha1.h"

namespace zillians { namespace language { namespace stage {

//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////

typedef std::set<boost::graph_traits<TangleGraphType>::vertex_descriptor> VertexSet;

static VertexSet findFirstBuildTargets(const TangleGraphType& g)
{
    VertexSet result;
    for(auto vi = boost::vertices(g); vi.first != vi.second; ++vi.first)
    {
        //boost::graph_traits<TangleGraphType>::vertex_descriptor vd = *vi.first;
        if(boost::out_degree(*vi.first, g) == 0)
        {
            result.insert(*vi.first);
        }
    }
    return result;
}

static std::string tangleFilesConcate(boost::graph_traits<TangleGraphType>::vertex_descriptor v, TangleGraphType& g)
{
    std::string result;
    const std::set<std::string>& files = g[v];
    foreach(f, files)
    {
        result += *f + "&";
    }
    return result;
}

static std::string tangleFileName(boost::graph_traits<TangleGraphType>::vertex_descriptor v, TangleGraphType& g)
{
    std::string result = sha1::sha1(tangleFilesConcate(v, g));
    std::set<std::string>& files = g[v];
    foreach(f, files)
    {
        std::string stemName = boost::filesystem::path(*f).stem().string();
        result += "_" + stemName;
    }
    return result;
}

static bool allSourceAreAst(const std::set<std::string>& sourceFiles)
{
    foreach(i, sourceFiles)
    {
        if (boost::filesystem::path(*i).extension() != ".ast")
        {
            return false;
        }
    }
    return true;
}

static std::string genCompileCmd(boost::graph_traits<TangleGraphType>::vertex_descriptor v, TangleGraphType& g)
{
    std::string cmd = "ts-compile ";

    // TODO pass the buildType to ts-compile
    //if(buildType == BUILD_TYPE::DEBUG)
    //{
    //    cmd += "--debug ";
    //}
    //else
    //{
    //    cmd += "--release ";
    //}

    // source files
    std::set<std::string>& sourceFiles = g[v];
    if(allSourceAreAst(sourceFiles))
    {
        return "";
    }

    foreach(i, sourceFiles)
    {
        cmd += "'" + *i + "' ";
    }

    // ast files
    boost::graph_traits<TangleGraphType>::out_edge_iterator ei, ei_end;
    for(boost::tie(ei, ei_end) = boost::out_edges(v, g); ei != ei_end; ++ei)
    {
        boost::graph_traits<TangleGraphType>::vertex_descriptor target = boost::target(*ei, g);
        const std::string& astfile = tangleFileName(target, g);
        cmd += "'" + astfile + ".ast' ";
    }

    // output files
    std::string outputFileName = tangleFileName(v, g);
    cmd += "--emit-ast=build/" + outputFileName + ".ast ";
    cmd += "--emit-llvm=build/" + outputFileName + ".bc ";

    return cmd;
}

//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptMakeStage::ThorScriptMakeStage() : dumpCompileCommand(false), rootDir("./"), logger(log4cxx::Logger::getLogger("ts-make"))
{
    log4cxx::BasicConfigurator::configure();
    logger->setLevel(log4cxx::Level::getAll());
}

ThorScriptMakeStage::~ThorScriptMakeStage()
{ }

const char* ThorScriptMakeStage::name()
{
	return "thor_script_make_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptMakeStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
        ("dump-compile-command", "dump compile command")
        ("root-dir", po::value<std::string>())
        ("debug", "debug build")
        ("release", "release build")
    ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options() ;

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptMakeStage::parseOptions(po::variables_map& vm)
{
    if(vm.count("dump-compile-command"))
    {
        dumpCompileCommand = true;
    }
    if(vm.count("root-dir"))
    {
        rootDir = boost::filesystem::path(vm["root-dir"].as<std::string>());
    }
    if(vm.count("debug") && vm.count("release"))
    {
        LOG4CXX_ERROR(logger, "Can not specify `--debug` and `--release` at the same time");
        return false;
    }
    if(vm.count("debug"))
    {
        buildType = BUILD_TYPE::DEBUG;
    }
    else if(vm.count("release"))
    {
        buildType = BUILD_TYPE::RELEASE;
    }
    return true;
}

struct Shell
{
    Shell(const std::string& cmd_) : cmd(cmd_) {}
    int operator()(int) {
        return system(cmd.c_str());
    }
    std::string cmd;
};

bool ThorScriptMakeStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

    // precondition
    if(!boost::filesystem::exists(rootDir))
    {
        LOG4CXX_ERROR(logger, "Root directory `" << rootDir.string() << "` does not exists.");
        return false;
    }

    boost::filesystem::current_path(rootDir);

    // restore file dependency
    boost::filesystem::path depFilePath("build/ts.dep");
    if(!boost::filesystem::exists(depFilePath))
    {
        LOG4CXX_ERROR(logger, "Dependency file `" << depFilePath.string() << "` does not exists.");
        return false;
    }

    std::ifstream fin(depFilePath.string().c_str());
    if(!fin.is_open())
    {
        LOG4CXX_ERROR(logger, "Can not open dependency file: `" << depFilePath.string() << "`.");
        return false;
    }
    boost::archive::text_iarchive ia(fin);
    TangleGraphType tangleRestored;
    ia >> tangleRestored;
    fin.close();

    // create braodcast node
    tbb::flow::graph g;
    tbb::flow::broadcast_node<int> start;

    // create internal node
    std::vector<zillians::JoinFunctionModule> moduleVec;
    for(auto vi = boost::vertices(tangleRestored); vi.first != vi.second; ++vi.first)
    {
        std::string cmd = genCompileCmd(*vi.first, tangleRestored);
        if(dumpCompileCommand)
        {
            std::cout << "compile-command: " << cmd << std::endl ;
        }
        int inputNum = boost::out_degree(*vi.first, tangleRestored);
        if(inputNum == 0)
        {
            inputNum = 1;
        }
        zillians::JoinFunctionModule m(g, Shell(cmd), inputNum);
        moduleVec.push_back(m);
    }

    // connect internal node
    boost::graph_traits<TangleGraphType>::edge_iterator ei, ei_end;
    for(boost::tie(ei, ei_end) = boost::edges(tangleRestored); ei != ei_end; ++ei)
    {
        int sourceNode = boost::source(*ei, tangleRestored);
        int targetNode = boost::target(*ei, tangleRestored);
        tbb::flow::make_edge(moduleVec[targetNode].getOutputPort(), moduleVec[sourceNode].getNextInputPort());
    }

    // connect breadcast node and beginning nodes
    VertexSet beginNodes = findFirstBuildTargets(tangleRestored);
    foreach(v, beginNodes)
    {
        tbb::flow::make_edge(start, moduleVec[*v].getNextInputPort());
    }

    start.try_put(1);
    g.wait_for_all();

    return true;
}

} } }
