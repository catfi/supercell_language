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
//#include <boost/bimap.hpp>
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
#include "language/stage/dep/ThorScriptSourceTangleGraph.h"
#include "language/grammar/ThorScriptPackageDependencyGrammar.h"
#include "utility/UnicodeUtil.h"

//namespace zillians { namespace language { namespace stage {

struct vertex_info
{
    std::string name;
    vertex_info(const std::string& name_) : name(name_) { }
    vertex_info() : name("uninitialized-name") { }
};

//} } }

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
    struct internal_vertex_name<vertex_info> {
        typedef vertex_name_extractor<vertex_info> type;
    } ;

    template<>
    struct internal_vertex_constructor<vertex_info> {
        typedef vertex_from_name<vertex_info> type;
    } ;
} }

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, vertex_info> FileGraphType;

namespace zillians { namespace language { namespace stage {

ThorScriptDepStage::ThorScriptDepStage()
{ }

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

	option_desc_public->add_options() ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options() ;

	return std::make_pair(option_desc_public, option_desc_private);
}

std::set<std::string> glogAllTsFiles(const std::string& dirPath)
{
    std::set<std::string> result;
    namespace fs = boost::filesystem;
    fs::path p(dirPath);
    for (auto i = fs::recursive_directory_iterator(p); i != fs::recursive_directory_iterator(); ++i)
    {
        if(fs::exists(i->path()) &&
           fs::is_regular_file(i->path()) &&
           i->path().string().substr(i->path().string().size()-2) == ".t")
        {
            result.insert(i->path().string());
        }
    }
    return result;
}

bool ThorScriptDepStage::parseOptions(po::variables_map& vm)
{
    if(vm.count("input"))
    {
        inputFiles = vm["input"].as<std::vector<std::string>>();
        return true;
    }
    else
    {
        std::set<std::string> inputFileSet = glogAllTsFiles(".");
        inputFiles.assign(inputFileSet.begin(), inputFileSet.end());
        return true;
    }
}

bool isDirectory(const std::string& pathString)
{
    namespace fs = boost::filesystem;
    fs::path p(pathString);
    return fs::exists(p) && fs::is_directory(p);
}

bool isRegularFile(const std::string& pathString)
{
    namespace fs = boost::filesystem;
    fs::path p(pathString);
    return fs::exists(p) && fs::is_regular_file(p);
}

std::string packageNameToPathName(const std::wstring& packageName)
{
    std::string result = ws_to_s(packageName);
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

bool parseFileImportedPackages(const std::string& tsFileName, std::set<std::wstring>& packages)
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

typedef std::map<std::string, std::set<std::string>> FileDependencyType ;

/**
 * @brief Add dependency of a ThorScript source file @p tsFileName to @p deps.
 * @param[in] pathString Package path to add.
 * @param[in,out] deps Data structure to store dependency.
 */
void addFileDependency(const std::string& tsFileName, FileDependencyType& deps, FileGraphType& fileGraph)
{
    // if file already processed
    if(deps.count(tsFileName))
    {
        return;
    }

    deps[tsFileName] = std::set<std::string>();

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
                        return result;
                   });
    // for each dep packages, collect dependent .t files.
    foreach(i, pathNameOfImportedPackages)
    {
        // if is regular .t file, directly add to deps
        if(isRegularFile(*i + ".t"))
        {
            //deps[tsFileName].insert(*i + ".t");
            boost::graph::add_edge(tsFileName, *i + ".t", fileGraph);
        }
        // if is package directory, recusively add all files under the dir.
        else if(isDirectory(*i))
        {
            std::set<std::string> allFilesUnderDir = glogAllTsFiles(*i);
            //deps[tsFileName].insert(allFilesUnderDir.begin(), allFilesUnderDir.end());
            for(auto f = allFilesUnderDir.begin(); f != allFilesUnderDir.end(); ++f)
            {
                boost::graph::add_edge(tsFileName, *f, fileGraph);
            }
        }
    }

    // for each dependent file, recursive apply addFileDependency().
    //const std::set<std::string>& se = deps[tsFileName] ;
    //for(auto i = se.begin(); i != se.end(); ++i)
    //{
    //    addFileDependency(*i, deps, fileGraph);
    //}
    typedef boost::property_map<FileGraphType, boost::vertex_index_t>::type IndexMap;
    IndexMap index = boost::get(boost::vertex_index, fileGraph);
    boost::graph_traits<FileGraphType>::out_edge_iterator ei, ei_end;
    boost::graph_traits<FileGraphType>::vertex_descriptor sourceVertexD = boost::graph::add_vertex(tsFileName, fileGraph);
    for(boost::tie(ei, ei_end) = boost::out_edges(sourceVertexD, fileGraph); ei != ei_end; ++ei)
    {
        const std::string& targetFileName = fileGraph[index[boost::target(*ei, fileGraph)]].name;
        addFileDependency(targetFileName, deps, fileGraph);

    }
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

bool analyzeTangle(const FileDependencyType& deps, ::FileGraphType& g)
{
    // construct graph
    // collect all files
    std::set<std::string> files;
    foreach(i, deps)
    {
        files.insert(i->first);
        for(auto j = i->second.begin(); j != i->second.end(); ++j)
        {
            files.insert(*j);
        }
    }
    // insert edge to graph
    foreach(i, deps)
    {
        for(auto j = i->second.begin(); j != i->second.end(); ++j)
        {
            std::string s = i->first;
            //int u = vertexIdMap.left.at(i->first);
            //int v = vertexIdMap.left.at(*j);
            std::string u = i->first;
            std::string v = *j;
            boost::graph::add_edge(u, v, g);
        }
    }
    // calculate strong connected components
    std::vector<int> component(files.size());
    strong_components(g, &component[0]);

    // init tangle graph
    TangleGraphType tangleG;

    // add tangle edges
    boost::property_map<TangleGraphType, boost::vertex_index_t>::type index = get(boost::vertex_index, g);
    for (auto range = edges(g); range.first != range.second; ++range.first)
    {
        int sourceVertex = index[source(*range.first, g)];
        int targetVertex = index[target(*range.first, g)];
        int sourceTangle = component[sourceVertex];
        int targetTangle = component[targetVertex];
        if(sourceTangle == targetTangle)
        {
            continue;
        }
        add_edge(sourceTangle, targetTangle, tangleG);
        const std::string& sourceFilename = g[sourceVertex].name;
        const std::string& targetFilename = g[targetVertex].name;
        tangleG[sourceTangle].insert(sourceFilename);
        tangleG[targetTangle].insert(targetFilename);
    }

    // serialization
    std::ofstream fout("ts.dep");
    boost::archive::text_oarchive oa(fout);
    oa << tangleG ;
    fout.close();

    // and restore
    std::ifstream fin("ts.dep");
    boost::archive::text_iarchive ia(fin);
    TangleGraphType tangleRestored;
    ia >> tangleRestored;
    fin.close();

    // write graphviz
    fout.open("ts.graphviz");
    boost::write_graphviz(fout, tangleRestored, VertexWriter<TangleGraphType>(tangleRestored));
    fout.close();

    return true;
}

bool ThorScriptDepStage::execute(bool& continue_execution)
{
    // TODO: replace the FileDependencyType with a file graph type
    FileDependencyType deps;
    FileGraphType fileGraph;

    // get file dependency
    foreach(i, inputFiles)
    {
        addFileDependency(*i, deps, fileGraph);
    }

    // analyze source file tangles (strong connected components)
    analyzeTangle(deps, fileGraph);

    return true;
}

} } }

