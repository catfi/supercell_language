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

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#include <boost/filesystem.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/isomorphism.hpp>
#include <boost/serialization/set.hpp>
#include "language/ThorScriptDep.h"
#include "language/stage/dep/ThorScriptSourceTangleGraph.h"

#define BOOST_TEST_MODULE ThorScriptDepTest_ThorScriptDepHappyPathTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( ThorScriptDepTest_ThorScriptDepHappyPathTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepHappyPathTestCase1 )
{
    // Construct file system and file content
    boost::filesystem::create_directories(boost::filesystem::path("c"));
    boost::filesystem::create_directories(boost::filesystem::path("d"));
    boost::filesystem::create_directories(boost::filesystem::path("e"));
    boost::filesystem::create_directories(boost::filesystem::path("e/e1"));

    system("echo 'import b; \
                  import c; \
                  import d.d1; \
                  import e.e1;' >> a.t");

    system("echo 'package b;' > b.t");

    system("echo 'import c.c2;' > c/c1.t");
    system("echo 'import c.c1;' > c/c2.t");
    system("echo 'package c.c3;' > c/c3.t");

    system("echo 'import e.e1;' > d/d1.t");

    system("echo 'import e.e1.e12;' > e/e1/e11.t");
    system("echo 'import e.e1.e11;' > e/e1/e12.t");
    system("echo 'package e.e1.e13;' > e/e1/e13.t");

    const char* argv[] = {"testbin", "a.t"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(2, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("a.t"));
    boost::filesystem::remove_all(boost::filesystem::path("b.t"));
    boost::filesystem::remove_all(boost::filesystem::path("c"));
    boost::filesystem::remove_all(boost::filesystem::path("d"));
    boost::filesystem::remove_all(boost::filesystem::path("e"));

    // unserialize graph
    std::ifstream fin("ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK(boost::num_vertices(g) == 7);
    BOOST_CHECK(boost::num_edges(g) == 8);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        if(boost::out_degree(*vp.first, g) == 6)
        {
            BOOST_CHECK(g[*vp.first].size() == 1);
            BOOST_CHECK(*(g[*vp.first].begin()) == "a.t");
        }
        if(boost::out_degree(*vp.first, g) == 2)
        {
            BOOST_CHECK(g[*vp.first].size() == 1);
            BOOST_CHECK(*(g[*vp.first].begin()) == "d/d1.t");
        }
    }

    /// @note can not use boost::isomorphism to simplify the graph comparison, 
    /// because boost::isomorphism support only EdgeListAndVertexListGraph.
    /// And I don't know how to declare a EdgeListAndVertexListGraph.
    //{
    //    zillians::language::stage::TangleGraphType g2;
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor a = boost::add_vertex(g2);
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor b = boost::add_vertex(g2);
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor c1c2 = boost::add_vertex(g2);
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor c3 = boost::add_vertex(g2);
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor d = boost::add_vertex(g2);
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor e1e2 = boost::add_vertex(g2);
    //    boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor e3 = boost::add_vertex(g2);
    //    g2[a].insert("a.t");
    //    g2[b].insert("b.t");
    //    g2[c1c2].insert("c/c1.t");
    //    g2[c1c2].insert("c/c2.t");
    //    g2[c3].insert("c/c3.t");
    //    g2[d].insert("d/d1.t");
    //    g2[e1e2].insert("e/e1.t");
    //    g2[e1e2].insert("e/e2.t");
    //    g2[e3].insert("e/e3.t");
    //    BOOST_CHECK(boost::isomorphism(g, g2));
    //}

}

/**
 * Test onle one tangle, no edge case.
 */
BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepHappyPathTestCase2 )
{
    // Construct file system and file content

    boost::filesystem::create_directories(boost::filesystem::path("c"));
    system("echo 'package a; import b;' > a.t");
    system("echo 'package b; import c;' > b.t");
    system("echo 'package c; import a;' > c.t");

    const char* argv[] = {"testbin", "a.t"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(2, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("a.t"));
    boost::filesystem::remove_all(boost::filesystem::path("b.t"));
    boost::filesystem::remove_all(boost::filesystem::path("c.t"));
    boost::filesystem::remove_all(boost::filesystem::path("c"));

    // unserialize graph
    std::ifstream fin("ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK(boost::num_vertices(g) == 1);
    BOOST_CHECK(boost::num_edges(g) == 0);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        BOOST_CHECK(g[*vp.first].size() == 3);
        std::set<std::string> files = g[*vp.first];
        BOOST_CHECK(files.count("a.t"));
        BOOST_CHECK(files.count("b.t"));
        BOOST_CHECK(files.count("c.t"));
    }
}

/**
 * Test only one file in one tangle case
 */
BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepHappyPathTestCase3 )
{
    // Construct file system and file content

    boost::filesystem::create_directories(boost::filesystem::path("c"));
    system("echo 'package a;' > a.t");

    const char* argv[] = {"testbin", "a.t"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(2, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("a.t"));

    // unserialize graph
    std::ifstream fin("ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK(boost::num_vertices(g) == 1);
    BOOST_CHECK(boost::num_edges(g) == 0);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        BOOST_CHECK(g[*vp.first].size() == 1);
        std::set<std::string> files = g[*vp.first];
        BOOST_CHECK(files.count("a.t"));
    }
}

/**
 * When there are both regular file and a directory match the imported pakcage name.
 * Use the regular file one, and issue a warning of the ambiguity.
 */
BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepAmbiguousTestCase1 )
{
    // Construct file system and file content

    system("echo 'package a; import b;' > a.t");
    system("echo 'package b; import c;' > b.t");
    system("echo 'package c; import a;' > c.t");

    const char* argv[] = {"testbin", "a.t"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(2, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("a.t"));
    boost::filesystem::remove_all(boost::filesystem::path("b.t"));
    boost::filesystem::remove_all(boost::filesystem::path("c.t"));

    // unserialize graph
    std::ifstream fin("ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK(boost::num_vertices(g) == 1);
    BOOST_CHECK(boost::num_edges(g) == 0);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        BOOST_CHECK(g[*vp.first].size() == 3);
        std::set<std::string> files = g[*vp.first];
        BOOST_CHECK(files.count("a.t"));
        BOOST_CHECK(files.count("b.t"));
        BOOST_CHECK(files.count("c.t"));
    }
}

BOOST_AUTO_TEST_SUITE_END()
