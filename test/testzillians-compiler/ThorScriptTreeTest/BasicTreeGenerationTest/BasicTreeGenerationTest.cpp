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
/**
 * @date Sep 1, 2009 sdk - Initial version created.
 */

#include "core/Prerequisite.h"
#include "compiler/tree/ASTNode.h"
#include "compiler/tree/ASTNodeFactory.h"
#include "compiler/tree/visitor/AllocationCleanupVisitor.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_BasicTreeGenerationTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::compiler::tree;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_BasicTreeGenerationTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_BasicTreeGenerationTestCase1_IsA )
{
	AllocationCleanupVisitor cleanup;

	{
		ASTNode *node = new Program(NULL);
		BOOST_CHECK(isa<Program>(node));
		BOOST_CHECK(!isa<Literal>(node));
		cleanup.visit(*node);
	}

	{
		ASTNode *node = new BinaryExpr(BinaryExpr::ASSIGN, new SimpleIdentifier(L"abc"), new NumericLiteral(123L));
		BOOST_CHECK(isa<Expression>(node));
		BOOST_CHECK(isa<BinaryExpr>(node));
		cleanup.visit(*node);
	}

	{
		NestedIdentifier* node = new NestedIdentifier();
		BOOST_CHECK(isa<Identifier>(node));
		cleanup.visit(*node);
	}
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_BasicTreeGenerationTestCase2 )
{
	AllocationCleanupVisitor cleanup;

	{
		Package* root = new Package(new SimpleIdentifier(L""));
		Program* program = new Program(root);

		BOOST_CHECK(program->root->id->toString().empty());

		cleanup.visit(*program);
	}

	{
		Program* program = new Program();

		BOOST_CHECK(program->root->id->toString().empty()); // the default package name is empty ""

		cleanup.visit(*program);
	}
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_BasicTreeGenerationTestCase3 )
{
	AllocationCleanupVisitor cleanup;

	Program* program = new Program();
	BOOST_CHECK(program->root->parent == NULL);

	cleanup.visit(*program);
}

BOOST_AUTO_TEST_SUITE_END()
