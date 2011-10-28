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

#include "core/Prerequisite.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/GenericVisitor.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/tree/visitor/GenericComposableVisitor.h"
#include "language/tree/visitor/ObjectCountVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_GenericVisitorTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::tree;
using namespace zillians::language::tree::visitor;

struct DummyCountVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(countInvoker, count);

	DummyCountVisitor() : total_count(0L)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(countInvoker)
	}

	void count(ASTNode& node)
	{
		++total_count;
		revisit(node); // use parent's revisit so that we will walk through entire AST tree hierarchy
	}

	std::size_t total_count;
};

struct DummyComposedCountVisitor : Visitor<ASTNode, void>
{
	CREATE_INVOKER(countInvoker, count);

	DummyComposedCountVisitor() : total_count(0L)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(countInvoker)
	}

	void count(ASTNode& node)
	{
		++total_count;
		// here we don't have to revisit but just implement action on specific AST node types
		// (because the GenericComposableVisitor will handle revisit for us)
	}

	std::size_t total_count;
};

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_GenericVisitorTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GenericVisitorTestCase1 )
{
	DummyCountVisitor counter;

	ASTNode* program = createSample1();
	counter.visit(*program);

	BOOST_CHECK(counter.total_count == 17);

	std::cout << "total node count = " << counter.total_count << std::endl;
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GenericVisitorTestCase2 )
{
	DummyComposedCountVisitor counter1;
	DummyComposedCountVisitor counter2;

	// create a visitor composed by multiple composable visitor
	// note that in composable visitor we basically don't have to call revisit() or visit() because the GenericComposableVisitor will walk through the entire AST tree
	// (of course we can still do so if we need some custom traversal)
	GenericComposableVisitor<DummyComposedCountVisitor&, DummyComposedCountVisitor&> composed_counter(counter1, counter2);

	ASTNode* program = createSample1();
	composed_counter.visit(*program);

	BOOST_CHECK(counter1.total_count == 17);
	BOOST_CHECK(counter2.total_count == 17);

	std::cout << "total node count (counter1) = " << counter1.total_count << std::endl;
	std::cout << "total node count (counter2) = " << counter2.total_count << std::endl;
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GenericVisitorTestCase3 )
{
	ObjectCountVisitor<Composable> counter1;
	ObjectCountVisitor<Composable> counter2;

	// create a visitor composed by multiple composable visitor
	// note that in composable visitor we basically don't have to call revisit() or visit() because the GenericComposableVisitor will walk through the entire AST tree
	// (of course we can still do so if we need some custom traversal)
	GenericComposableVisitor<ObjectCountVisitor<Composable>&, ObjectCountVisitor<Composable>&> composed_counter(counter1, counter2);

	ASTNode* program = createSample1();
	composed_counter.visit(*program);

	BOOST_CHECK(counter1.get_count() == 17);
	BOOST_CHECK(counter2.get_count() == 17);

	std::cout << "total node count (counter1) = " << counter1.get_count() << std::endl;
	std::cout << "total node count (counter2) = " << counter2.get_count() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
