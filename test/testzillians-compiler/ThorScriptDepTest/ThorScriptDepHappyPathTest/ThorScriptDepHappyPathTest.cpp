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

static void createManifest()
{
    std::ofstream fout("manifest.xml");
    fout << "<project name=\"" << "test-dep-project" << "\" author=\"author\" version=\"0.0.0.1\">\n"
         << "    <dependency>\n"
         << "        <!-- bundle         path=\"./in_bundle/a.bundle\" -->\n"
         << "        <!-- native_object  path=\"./native/b.o\"         -->\n"
         << "        <!-- native_library path=\"./native/c.a\"         -->\n"
         << "    </dependency>\n"
         << "</project>\n" ;
    fout.close();
}

BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepHappyPathTestCase1 )
{
    // Construct file system and file content
    createManifest();

    boost::filesystem::create_directories(boost::filesystem::path("src"));
    boost::filesystem::create_directories(boost::filesystem::path("src/c"));
    boost::filesystem::create_directories(boost::filesystem::path("src/d"));
    boost::filesystem::create_directories(boost::filesystem::path("src/e"));
    boost::filesystem::create_directories(boost::filesystem::path("src/e/e1"));

    system("echo 'import b; \
                  import c; \
                  import d.d1; \
                  import e.e1;' >> src/a.t");

    system("echo ' ' > src/b.t");

    system("echo 'import c.c2;' > src/c/c1.t");
    system("echo 'import c.c1;' > src/c/c2.t");
    system("echo ' ' > src/c/c3.t");

    system("echo 'import e.e1;' > src/d/d1.t");

    system("echo 'import e.e1.e12;' > src/e/e1/e11.t");
    system("echo 'import e.e1.e11;' > src/e/e1/e12.t");
    system("echo ' ' > src/e/e1/e13.t");

    //const char* argv[] = {"testbin"};
    const char* argv[] = {};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(1, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("src"));

    // unserialize graph
    std::ifstream fin("build/ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK_EQUAL(boost::num_vertices(g), 7);
    BOOST_CHECK_EQUAL(boost::num_edges(g), 8);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        if(boost::out_degree(*vp.first, g) == 6)
        {
            BOOST_CHECK_EQUAL(g[*vp.first].size(), 1);
            BOOST_CHECK_EQUAL(*(g[*vp.first].begin()), "src/a.t");
        }
        if(boost::out_degree(*vp.first, g) == 2)
        {
            BOOST_CHECK_EQUAL(g[*vp.first].size(), 1);
            BOOST_CHECK_EQUAL(*(g[*vp.first].begin()), "src/d/d1.t");
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
    createManifest();

    boost::filesystem::create_directories(boost::filesystem::path("src"));
    system("echo 'import b;' > src/a.t");
    system("echo 'import c;' > src/b.t");
    system("echo 'import a;' > src/c.t");

    const char* argv[] = {"testbin"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(1, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("src"));
    // unserialize graph
    std::ifstream fin("build/ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK_EQUAL(boost::num_vertices(g), 1);
    BOOST_CHECK_EQUAL(boost::num_edges(g), 0);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        BOOST_CHECK_EQUAL(g[*vp.first].size(), 3);
        std::set<std::string> files = g[*vp.first];
        BOOST_CHECK(files.count("src/a.t"));
        BOOST_CHECK(files.count("src/b.t"));
        BOOST_CHECK(files.count("src/c.t"));
    }
}

/**
 * Test only one file in one tangle case
 */
BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepHappyPathTestCase3 )
{
    // Construct file system and file content
    createManifest();

    boost::filesystem::create_directories(boost::filesystem::path("src"));
    system("echo ' ' > src/a.t");

    const char* argv[] = {"testbin"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(1, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("src"));

    // unserialize graph
    std::ifstream fin("build/ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK_EQUAL(boost::num_vertices(g), 1);
    BOOST_CHECK_EQUAL(boost::num_edges(g), 0);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        BOOST_CHECK_EQUAL(g[*vp.first].size(), 1);
        std::set<std::string> files = g[*vp.first];
        BOOST_CHECK(files.count("src/a.t"));
    }
}

/**
 * When there are both regular file and a directory match the imported pakcage name.
 * Use the regular file one, and issue a warning of the ambiguity.
 */
BOOST_AUTO_TEST_CASE( ThorScriptDepTest_ThorScriptDepAmbiguousTestCase1 )
{
    // Construct file system and file content
    createManifest();

    boost::filesystem::create_directories(boost::filesystem::path("src"));
    system("echo 'import b;' > src/a.t");
    system("echo 'import c;' > src/b.t");
    system("echo 'import a;' > src/c.t");

    //const char* argv[] = {"testbin", "src/a.t"};
    const char* argv[] = {"testbin"};

    // parse and output graph, and unserilaize graph
    zillians::language::ThorScriptDep dep;
    dep.main(1, argv);

    // remove tmp files
    boost::filesystem::remove_all(boost::filesystem::path("src"));

    // unserialize graph
    std::ifstream fin("build/ts.dep");
    boost::archive::text_iarchive ia(fin);
    zillians::language::stage::TangleGraphType g;
    ia >> g;
    fin.close();

    // check
    BOOST_CHECK_EQUAL(boost::num_vertices(g), 1);
    BOOST_CHECK_EQUAL(boost::num_edges(g), 0);
    for(auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        BOOST_CHECK_EQUAL(g[*vp.first].size(), 3);
        std::set<std::string> files = g[*vp.first];
        BOOST_CHECK(files.count("src/a.t"));
        BOOST_CHECK(files.count("src/b.t"));
        BOOST_CHECK(files.count("src/c.t"));
    }
}

BOOST_AUTO_TEST_SUITE_END()
