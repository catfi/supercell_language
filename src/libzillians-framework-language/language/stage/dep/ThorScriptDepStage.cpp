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
#include <boost/bimap.hpp>
#include <boost/filesystem.hpp>
//#include <boost/graph/adjacency_list.hpp>
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

std::set<std::string> recursiveAllFiles(const std::string& dirPath)
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
        std::set<std::string> inputFileSet = recursiveAllFiles(".");
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
    fs::path p(pathString + ".t");
    return fs::exists(p) && fs::is_regular_file(p);
}

std::string packageNameToPathName(const std::wstring& packageName)
{
    std::string result = ws_to_s(packageName);
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

// TODO implement
bool getFileDependentPackages(const std::string& tPathName, std::set<std::wstring>& packages)
{
    std::string filename = tPathName + ".t" ;
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

typedef std::map<std::string, std::set<std::string>> DepType ;

void addDeps(const std::string& pathString, DepType& deps)
{
    namespace fs = boost::filesystem;
    // if file processed
    if(deps.count(pathString + ".t"))
    {
        return;
    }

    deps[pathString + ".t"] = std::set<std::string>();

    // if is dir name
    fs::path p(pathString);
    if(isDirectory(pathString))
    {
        for (auto i = fs::directory_iterator(p); i != fs::directory_iterator(); ++i)
        {
            addDeps(i->path().string(), deps);
        }
    }

    // if is .t src file
    if(isRegularFile(pathString))
    {
        std::set<std::wstring> fileDependentPackages;
        bool parseResult = getFileDependentPackages(p.string(), fileDependentPackages);
        std::vector<std::string> fileDependentPathNames(fileDependentPackages.size());
        std::transform(fileDependentPackages.begin(), fileDependentPackages.end(), fileDependentPathNames.begin(), packageNameToPathName);
        // for each dep packages
        foreach(i, fileDependentPathNames)
        {
            // if is regular .t file, directly add to deps
            if(isRegularFile(*i))
            {
                deps[pathString + ".t"].insert(*i + ".t");
            }
            // if is package directory, recusively add all files under the dir.
            else if(isDirectory(*i))
            {
                std::set<std::string> allFilesUnderDir = recursiveAllFiles(*i);
                deps[pathString + ".t"].insert(allFilesUnderDir.begin(), allFilesUnderDir.end());
            }
        }
    }
    
    const std::set<std::string>& se = deps[pathString + ".t"] ;
    for(auto i = se.begin(); i != se.end(); ++i)
    {
        //std::cout << (pathString + ".t -> ") << *i << std::endl ;
        std::string recursiveVisitingPathString = i->substr(0, i->size()-2);
        addDeps(recursiveVisitingPathString, deps);
    }
}

template<typename GraphType>
class VertexWriter
{
public:
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

bool analyzeTangle(const DepType& deps)
{
    // construct graph
    using namespace boost;
    typedef adjacency_list<vecS, vecS> GraphType;
    boost::bimap<std::string, size_t> vertexIdMap;
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
    // construct file <-> id bimap
    size_t id = 0;
    for(auto i = files.begin(); i != files.end(); ++i)
    {
        vertexIdMap.insert(boost::bimap<std::string, size_t>::value_type(*i, id));
        ++id;
    }
    // insert edge to graph
    GraphType g;
    foreach(i, deps)
    {
        for(auto j = i->second.begin(); j != i->second.end(); ++j)
        {
            std::string s = i->first;
            int u = vertexIdMap.left.at(i->first);
            int v = vertexIdMap.left.at(*j);
            add_edge(u, v, g);
        }
    }
    // calculate strong connected components
    std::vector<int> component(files.size());
    strong_components(g, &component[0]);
    // output result
    id = 0;
    int maxComponentId = 0;
    for(auto i = files.begin(); i != files.end(); ++i)
    {
        //std::cout << *i << "  " << component[id] << std::endl ;
        ++id;
        maxComponentId = std::max(component[id], maxComponentId);
    }

    // init tangle graph
    TangleGraphType tangleG(maxComponentId + 1);

    // add tangle edges
    property_map<TangleGraphType, vertex_index_t>::type index = get(vertex_index, g);
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
        const std::string& sourceFilename = vertexIdMap.right.at(sourceVertex);
        const std::string& targetFilename = vertexIdMap.right.at(targetVertex);
        tangleG[sourceTangle].insert(sourceFilename);
        tangleG[targetTangle].insert(targetFilename);
    }

    std::ofstream fout("ts.dep");
    boost::archive::text_oarchive oa(fout);
    oa << tangleG ;
    fout.close();

    std::ifstream fin("ts.dep");
    boost::archive::text_iarchive ia(fin);
    TangleGraphType tangleRestored;
    ia >> tangleRestored;
    fin.close();

    fout.open("ts.graphviz");
    boost::write_graphviz(fout, tangleRestored, VertexWriter<TangleGraphType>(tangleRestored));
    fout.close();

    return true;
}

bool ThorScriptDepStage::execute(bool& continue_execution)
{
    DepType deps;

    // get file dependency
    foreach(i, inputFiles)
    {
        std::string s = *i ;
        //std::cout << "Parsing file: " << s << std::endl;
        s.resize(s.size() - 2) ;
        addDeps(s, deps);
    }

    // analyze source file tangles (strong connected components)
    //std::cout << std::endl ;
    //std::cout << std::endl ;
    analyzeTangle(deps);


    // analyze parallel groups
    // TODO:

    return true;
}

} } }

