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

#ifndef ZILLIANS_LANGUAGE_STAGE_DEP_THORSCRIPTDEPSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_DEP_THORSCRIPTDEPSTAGE_H_

#include <utility>
#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/topological_sort.hpp>
#include <log4cxx/logger.h>
#include "language/stage/Stage.h"

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

namespace zillians { namespace language { namespace stage {

/**
 * The ThorScriptDepStage analyze the dependency of *.t files, out put 
 *
 * For all type-related semantic checks, we need to put it into SemanticVerificationStage1
 */
class ThorScriptDepStage : public Stage
{
public:
	ThorScriptDepStage();
	virtual ~ThorScriptDepStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

private:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, zillians::language::stage::detail::vertex_info> FileGraphType;

private:
    bool parseFileImportedPackages(const std::string& tsFileName, std::set<std::wstring>& packages);
    bool addFileDependency(std::string tsFileName, const std::map<std::wstring, std::string>& packagesInAsts, FileGraphType& fileGraph, log4cxx::LoggerPtr logger);
    bool scanBundlePackage(const boost::filesystem::path& astPath, std::multimap<std::string, std::wstring>& allBundlePackage);
    bool analyzeTangle(const boost::filesystem::path& buildPath, FileGraphType& g);
    bool scanAllBundlePackage(const boost::filesystem::path& buildPath, std::multimap<std::string, std::wstring>& allBundlePackage);

public:
    std::vector<std::string> inputFiles;
    boost::filesystem::path rootDir;
    boost::filesystem::path buildPath;
    log4cxx::LoggerPtr logger;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_DEP_THORSCRIPTDEPSTAGE_H_ */
