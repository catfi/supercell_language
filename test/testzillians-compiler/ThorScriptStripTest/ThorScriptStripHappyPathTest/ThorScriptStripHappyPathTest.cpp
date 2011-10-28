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
#include "language/ThorScriptStrip.h"
#include "language/stage/strip/visitor/ThorScriptStripStageVisitor.h"
#include "../../ThorScriptTreeTest/ASTNodeSamples.h"

#define BOOST_TEST_MODULE ThorScriptStripTest_ThorScriptStripHappyPathTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( ThorScriptStripTest_ThorScriptStripHappyPathTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptStripTest_ThorScriptStripHappyPathTestCase1 )
{
    using namespace zillians::language::tree;
    Program* program = cast<Program>(createSample5());
    zillians::language::stage::visitor::ThorScriptStripStageVisitor strip;

    ClassDecl* c = cast<ClassDecl>(program->root->children[0]->children[0]->objects[0]);
    BOOST_CHECK(c->member_functions[0]->block != NULL);
    BOOST_CHECK(c->member_functions[1]->block != NULL);
    BOOST_CHECK(c->member_functions[2]->block != NULL);

    strip.strip(*program);

    BOOST_CHECK(c->member_functions[0]->block == NULL);
    BOOST_CHECK(c->member_functions[1]->block != NULL);
    BOOST_CHECK(c->member_functions[2]->block == NULL);
}

BOOST_AUTO_TEST_SUITE_END()
