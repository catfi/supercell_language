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

#include <algorithm>
#include <vector>
#include <set>
#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/topological_sort.hpp>
#include "language/stage/dep/ThorScriptDepStage.h"
#include "utility/sha1.h"
#include "language/stage/dep/ThorScriptSourceTangleGraph.h"
#include "language/grammar/ThorScriptPackageDependencyGrammar.h"
#include "language/ThorScriptManifest.h"
#include "utility/UnicodeUtil.h"

//////////////////////////////////////////////////////////////////////////////
// declaration for boost graph 'named_graph'
//////////////////////////////////////////////////////////////////////////////

namespace zillians { namespace language { namespace stage { namespace detail {

/**
 * About boost::graph named_graph:
 * see:
 * - http://stackoverflow.com/questions/7315687/boost-graph-library-named-graph-and-remove-vertex
 * - BOOST_SOURCE_DIR/libs/graph/test/named_vertices_test.cpp
 */
struct vertex_info
{
    std::string name;
    vertex_info(const std::string& name_) : name(name_) { }
    vertex_info() : name("uninitialized-name") { }
};

} } } }

namespace boost { namespace graph {
    template<typename Type>
    struct vertex_name_extractor {
        typedef Type type;
        typedef const std::string& result_type;
        result_type operator()(const Type& v) const {
            return v.name;
        }
    } ;

    template<>
    struct internal_vertex_name<zillians::language::stage::detail::vertex_info> {
        typedef vertex_name_extractor<zillians::language::stage::detail::vertex_info> type;
    } ;

    template<>
    struct internal_vertex_constructor<zillians::language::stage::detail::vertex_info> {
        typedef vertex_from_name<zillians::language::stage::detail::vertex_info> type;
    } ;
} }

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, zillians::language::stage::detail::vertex_info> FileGraphType;

namespace zillians { namespace language { namespace stage {

//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////

static std::set<std::string> globAllTsFiles(const std::string& dirPath)
{
    std::set<std::string> result;
    if(!boost::filesystem::exists(dirPath))
    {
        return result;
    }
    namespace fs = boost::filesystem;
    fs::path p(dirPath);
    for (auto i = fs::recursive_directory_iterator(p); i != fs::recursive_directory_iterator(); ++i)
    {
        if(fs::exists(i->path()) &&
           fs::is_regular_file(i->path()) &&
           i->path().string().substr(i->path().string().size()-2) == ".t")
        {
            if(i->path().string().substr(0, 2) == "./")
            {
                result.insert(i->path().string().substr(2));
            }
            else
            {
                result.insert(i->path().string());
            }
        }
    }
    return result;
}

static bool isDirectory(const std::string& pathString)
{
    namespace fs = boost::filesystem;
    fs::path p(pathString);
    return fs::exists(p) && fs::is_directory(p);
}

static bool isRegularFile(const std::string& pathString)
{
    namespace fs = boost::filesystem;
    fs::path p(pathString);
    return fs::exists(p) && fs::is_regular_file(p);
}

static bool parseFileImportedPackages(const std::string& tsFileName, std::set<std::wstring>& packages)
{
    std::string filename = tsFileName ;
    std::ifstream in(filename, std::ios_base::in);

    if(!in.good())
    {
        LOG4CXX_ERROR(LoggerWrapper::ParserStage, "failed to open file: " << filename);
        return false;
    }

    // ignore the BOM marking the beginning of a UTF-8 file in Windows
    if(in.peek() == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        if (s != std::string("\xef\xbb\xbf"))
        {
            std::cerr << "parser error: unexpected characters from input file: " << filename << std::endl;
            return false;
        }
    }

    std::string source_code_raw;
    in.unsetf(std::ios::skipws); // disable white space skipping
    std::copy(std::istream_iterator<char>(in), std::istream_iterator<char>(), std::back_inserter(source_code_raw));

    // convert is frem UTF8 into UCS4 as a string by using u8_to_u32_iterator
    std::wstring source_code;
    utf8_to_ucs4(source_code_raw, source_code);

    // enable correct locale so that we can print UCS4 characters
    enable_default_locale(std::wcout);

    // try to parse
    std::vector<std::wstring> resultVec;
    typedef boost::spirit::classic::position_iterator2<std::wstring::iterator> pos_iterator_type;
    try
    {
        pos_iterator_type begin(source_code.begin(), source_code.end(), s_to_ws(filename));
        pos_iterator_type end;

        zillians::language::grammar::getImportedPackages(begin, end, resultVec);
    }
    catch (std::exception& e)
    {
        std::wcerr << "ts-dep parsing error!" << std::endl ;
        return false;
    }
    packages.insert(resultVec.begin(), resultVec.end());
    return true;
}

static std::string packageNameToPathName(const std::wstring& packageName)
{
    std::string result = ws_to_s(packageName);
    std::replace(result.begin(), result.end(), '.', '/');
    result = "src/" + result;
    return result;
}

/**
 * @brief Add dependency of a ThorScript source file @p tsFileName to @p fileGraph.
 * @param[in] pathString Package path to add.
 * @param[in,out] fileGraph Data structure to store dependency.
 */
static bool addFileDependency(std::string tsFileName, const std::map<std::wstring, std::string>& packagesInAsts, FileGraphType& fileGraph, log4cxx::LoggerPtr logger)
{
    // precondition
    BOOST_ASSERT(boost::filesystem::exists(tsFileName));

    // if tsFileName is processed already, skip it.
    boost::optional<boost::graph_traits<FileGraphType>::vertex_descriptor> currentVertexOpt = boost::graph::find_vertex(tsFileName, fileGraph);
    if(currentVertexOpt && boost::out_degree(*currentVertexOpt, fileGraph) > 0)
    {
        return true;
    }

    /**
     * @note
     * It might be a flaw of boost graph.
     * When using name_graph. e.g. <tt>add_vertex(name_of_vertex, graph)</tt>.
     * Use <tt>boost::add_vertex()</tt> will cause strange compile error message.
     * So please DO NOT USE <tt>boost::add_vertex()</tt> here,
     * use <tt>boost::graph::add_vertex()</tt> instead.
     * so is <tt>>boost::graph::add_edge()</tt>.
     */
    boost::graph::add_vertex(tsFileName, fileGraph);

    namespace fs = boost::filesystem;
    // parse ThorScript source file to get import packages
    std::set<std::wstring> fileImportedPackages;
    parseFileImportedPackages(tsFileName, fileImportedPackages);

    // transform import packages to path name
    // aka "a.b.c" -> "a/b/c"
    std::vector<std::string> pathNameOfImportedPackages(fileImportedPackages.size());
    std::transform(fileImportedPackages.begin(),
                   fileImportedPackages.end(),
                   pathNameOfImportedPackages.begin(),
                   [] (const std::wstring& packageName) {
                        std::string result = ws_to_s(packageName);
                        std::replace(result.begin(), result.end(), '.', '/');
                        result = "src/" + result;
                        return result;
                   });
    // for each dep packages, collect dependent .t files.
    foreach(i, fileImportedPackages)
    {
        std::string pathName = packageNameToPathName(*i);
        // if is in imported bundle(.ast), add the .ast to fileGraph
        auto ast = packagesInAsts.find(*i);
        if(ast != packagesInAsts.end())
        {
            boost::graph::add_edge(tsFileName, ast->second, fileGraph);
        }
        // if is regular .t file, directly add to fileGraph
        else if (isRegularFile(pathName + ".t"))
        {
            boost::graph::add_edge(tsFileName, pathName + ".t", fileGraph);
        }
        // if is package directory, recusively add all files under the dir.
        else if(isDirectory(pathName))
        {
            std::set<std::string> allFilesUnderDir = globAllTsFiles(pathName);
            for(auto f = allFilesUnderDir.begin(); f != allFilesUnderDir.end(); ++f)
            {
                boost::graph::add_edge(tsFileName, *f, fileGraph);
            }
        }
        // can not find package in folder and imported ast
        else
        {
            // TODO: print *i in uniocde?
            LOG4CXX_ERROR(logger, "Can not find package `" << ws_to_s(*i) << "` in all imported ast.");
            return false;
        }
    }

    // for each dependent file, recursive apply addFileDependency().
    typedef boost::property_map<FileGraphType, boost::vertex_index_t>::type IndexMap;
    IndexMap index = boost::get(boost::vertex_index, fileGraph);
    boost::graph_traits<FileGraphType>::out_edge_iterator ei, ei_end;
    boost::graph_traits<FileGraphType>::vertex_descriptor sourceVertexD = boost::graph::add_vertex(tsFileName, fileGraph);
    for(boost::tie(ei, ei_end) = boost::out_edges(sourceVertexD, fileGraph); ei != ei_end; ++ei)
    {
        const std::string& targetFileName = fileGraph[index[boost::target(*ei, fileGraph)]].name;
        if(boost::filesystem::path(targetFileName).extension() == ".ast") continue;
        addFileDependency(targetFileName, packagesInAsts, fileGraph, logger);
    }
    return true;
}

// Customize boost::graph write graphviz vertex.

/**
 * @brief Customize boost::graph write graphviz vertex.
 *
 * @see http://www.boost.org/doc/libs/release/libs/graph/doc/adjacency_list.html#serialization
 */
template<typename GraphType>
class VertexWriter
{
public:

     /**
      * @brief Initialize the VertexWrite with the graph. hold the graph as a reference.
      * @param g Graph to write out.
      */
    VertexWriter(const GraphType& g) : _g(g) {}
    template<typename Vertex>
    void operator()(std::ostream& out, const Vertex& v)
    {
        out << "[fontname=\"monospace\", shape=\"rectangle\", style=\"filled\", color=\"lightsteelblue1\", label=\"";
        for(auto i = _g[v].begin(); i != _g[v].end(); ++i)
        {
            if(i != _g[v].begin())
            {
                out << "\\n";
            }
            out << *i;
        }
        out << "\"]";
    }
private:
    const GraphType& _g;
};

static bool analyzeTangle(FileGraphType& g)
{
    // calculate strong connected components
    std::vector<int> component(boost::num_vertices(g));
    strong_components(g, &component[0]);

    // init tangle graph
    zillians::int32 numTangles = (*std::max_element(component.begin(), component.end())) + 1;
    TangleGraphType tangleG(numTangles);

    // add files to tangle vertex
    for(size_t i = 0; i != boost::num_vertices(g); ++i)
    {
        tangleG[component[i]].insert(g[i].name);
    }

    // add tangle edges
    boost::property_map<TangleGraphType, boost::vertex_index_t>::type index = get(boost::vertex_index, g);
    for(auto range = edges(g); range.first != range.second; ++range.first)
    {
        int sourceVertex = index[source(*range.first, g)];
        int targetVertex = index[target(*range.first, g)];
        int sourceTangle = component[sourceVertex];
        int targetTangle = component[targetVertex];
        // filter self to self edge
        if(sourceTangle != targetTangle)
        {
            add_edge(sourceTangle, targetTangle, tangleG);
        }
    }

    // create and change to build folder, if not exists.
    if(!boost::filesystem::exists("build"))
    {
        boost::filesystem::create_directory("build");
        assert(boost::filesystem::exists("build"));
    }

    // serialization
    std::ofstream fout("build/ts.dep");
    boost::archive::text_oarchive oa(fout);
    oa << tangleG ;
    fout.close();

    // and restore
    std::ifstream fin("build/ts.dep");
    boost::archive::text_iarchive ia(fin);
    TangleGraphType tangleRestored;
    ia >> tangleRestored;
    fin.close();

    // write graphviz
    fout.open("build/ts.graphviz");
    boost::write_graphviz(fout, tangleRestored, VertexWriter<TangleGraphType>(tangleRestored));
    fout.close();

    return true;
}

static void scanBundlePackage(const boost::filesystem::path& astPath, std::multimap<std::string, std::wstring>& allBundlePackage)
{
    std::ifstream fin(astPath.string().c_str());
    boost::archive::text_iarchive ia(fin);
    zillians::language::tree::ASTNode* node = NULL;
    ia >> node;
    zillians::language::tree::Tangle* tangle = zillians::language::tree::cast<zillians::language::tree::Tangle>(node);
    BOOST_ASSERT(tangle != NULL);

    foreach(i, tangle->sources)
    {
        allBundlePackage.insert(std::make_pair(astPath.string(), i->first->toString()));
    }
}

static boost::filesystem::path getAstPath(const boost::filesystem::path& bundleFolder)
{
    namespace fs = boost::filesystem;
    for(auto i = fs::directory_iterator(bundleFolder); i != fs::directory_iterator(); ++i)
    {
        if(i->path().extension().string() == ".ast")
        {
            return i->path();
        }
    }
    UNREACHABLE_CODE();
}

static std::multimap<std::string, std::wstring> scanAllBundlePackage()
{
    std::multimap<std::string, std::wstring> allBundlePackage;
    namespace fs = boost::filesystem;
    zillians::language::ProjectManifest pm;
    pm.load("manifest.xml");
    foreach(i, pm.dep.bundles)
    {
        fs::path bundleFolder = "build";
        bundleFolder /= sha1::sha1(*i);
        fs::path astPath = getAstPath(bundleFolder);
        BOOST_ASSERT(fs::exists(astPath));
        scanBundlePackage(astPath, allBundlePackage);
    }
    return allBundlePackage;
}

//////////////////////////////////////////////////////////////////////////////
// class member function
//////////////////////////////////////////////////////////////////////////////

ThorScriptDepStage::ThorScriptDepStage() : rootDir("./"), logger(log4cxx::Logger::getLogger("ts-dep"))
{
    log4cxx::BasicConfigurator::configure();
    logger->setLevel(log4cxx::Level::getAll());
}

ThorScriptDepStage::~ThorScriptDepStage()
{ }

const char* ThorScriptDepStage::name()
{
	return "thor_script_dep_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptDepStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
        ("root-dir", po::value<std::string>())
        ("input", po::value<std::vector<std::string>>(), "input file")
    ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options() ;

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptDepStage::parseOptions(po::variables_map& vm)
{
    if(vm.count("root-dir"))
    {
        rootDir = vm["root-dir"].as<std::string>();
    }
    if(vm.count("input"))
    {
        inputFiles = vm["input"].as<std::vector<std::string>>();
        return true;
    }
    else
    {
        std::set<std::string> inputFileSet = globAllTsFiles("src");
        inputFiles.assign(inputFileSet.begin(), inputFileSet.end());
        return true;
    }
}

bool ThorScriptDepStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

    // precondition
    if(!boost::filesystem::exists(rootDir))
    {
        LOG4CXX_ERROR(logger, "Root directory `" << rootDir.string() << "` does not exists.");
        return false;
    }

    boost::filesystem::current_path(rootDir);

    if(!boost::filesystem::exists("src"))
    {
        LOG4CXX_ERROR(logger, "source directory `src` does not exists.");
        return false;
    }

    foreach(i, inputFiles)
    {
        if(!boost::filesystem::exists(*i))
        {
            LOG4CXX_ERROR(logger, "input file `" << *i << "` does not exists.");
            return false;
        }
    }

    std::multimap<std::string, std::wstring> bundlePackages = scanAllBundlePackage();
    std::map<std::wstring, std::string> packagesInAsts;
    foreach(i, bundlePackages)
    {
        packagesInAsts.insert(std::make_pair(i->second, i->first));
    }

    // get file dependency
    if(inputFiles.empty())
    {
        LOG4CXX_ERROR(logger, "source directory is empty.");
        return false;
    }
    FileGraphType fileGraph;
    foreach(i, inputFiles)
    {
        addFileDependency(*i, packagesInAsts, fileGraph, logger);
    }

    // analyze source file tangles (strong connected components)
    analyzeTangle(fileGraph);

    return true;
}

} } }
