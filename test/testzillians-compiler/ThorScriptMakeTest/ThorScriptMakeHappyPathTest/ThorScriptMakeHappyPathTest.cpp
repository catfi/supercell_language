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
#include "language/ThorScriptMake.h"
#include "language/stage/make/ThorScriptMakeStage.h"
#include "language/stage/dep/ThorScriptSourceTangleGraph.h"

#define BOOST_TEST_MODULE ThorScriptMakeTest_ThorScriptMakeHappyPathTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( ThorScriptMakeTest_ThorScriptMakeHappyPathTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptMakeTest_ThorScriptMakeHappyPathTestCase1 )
{
    // Construct file graph
    {
        zillians::language::stage::TangleGraphType g2;
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor a = boost::add_vertex(g2);
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor b = boost::add_vertex(g2);
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor c1c2 = boost::add_vertex(g2);
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor c3 = boost::add_vertex(g2);
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor d = boost::add_vertex(g2);
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor e1e2 = boost::add_vertex(g2);
        boost::graph_traits<zillians::language::stage::TangleGraphType>::vertex_descriptor e3 = boost::add_vertex(g2);
        g2[a].insert("a.t");
        g2[b].insert("b.t");
        g2[c1c2].insert("c/c1.t");
        g2[c1c2].insert("c/c2.t");
        g2[c3].insert("c/c3.t");
        g2[d].insert("d/d1.t");
        g2[e1e2].insert("e/e1.t");
        g2[e1e2].insert("e/e2.t");
        g2[e3].insert("e/e3.t");
        //BOOST_CHECK(boost::isomorphism(g, g2));
    }

    // parse and output graph, and unserilaize graph
    zillians::language::stage::ThorScriptMakeStage make;

    //BOOST_CHECK_EQUAL(make.cmds.size(), 7);
    //BOOST_CHECK_EQUAL(make.cmds[0], "ts-compile c/c1.t c/c2.t");
    //BOOST_CHECK_EQUAL(make.cmds[1], "ts-compile c/c3.t");
    //BOOST_CHECK_EQUAL(make.cmds[2], "ts-compile e/e1/e11.t e/e1/e12.t");
    //BOOST_CHECK_EQUAL(make.cmds[3], "ts-compile e/e1/e13.t");
    //BOOST_CHECK_EQUAL(make.cmds[4], "ts-compile d1.t");
    //BOOST_CHECK_EQUAL(make.cmds[5], "ts-compile b.t");
    //BOOST_CHECK_EQUAL(make.cmds[6], "ts-compile a.t");
}

BOOST_AUTO_TEST_SUITE_END()
