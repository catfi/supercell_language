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
#include "language/tree/visitor/general/GarbageCollectionVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_BasicTreeGenerationTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::tree;
using namespace zillians::language::tree::visitor;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_BasicTreeGenerationTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_BasicTreeGenerationTestCase1_IsA )
{
	{
		ASTNode *node = new Program(new Package(new SimpleIdentifier(L"")));
		BOOST_CHECK(isa<Program>(node));
		BOOST_CHECK(!isa<Literal>(node));
	}

	{
		ASTNode *node = new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"abc")), new PrimaryExpr(new NumericLiteral((uint64)123L)));
		BOOST_CHECK(isa<Expression>(node));
		BOOST_CHECK(isa<BinaryExpr>(node));
	}

	{
		NestedIdentifier* node = new NestedIdentifier();
		BOOST_CHECK(isa<Identifier>(node));
	}

	{
		VariableDecl* node = new VariableDecl(new SimpleIdentifier(L"test"), new TypeSpecifier(PrimitiveType::FLOAT32), false, Declaration::VisibilitySpecifier::DEFAULT, Declaration::StorageSpecifier::CONST);
		BOOST_CHECK(isa<VariableDecl>(node));
		BOOST_CHECK(isa<Declaration>(node));
	}
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_BasicTreeGenerationTestCase2 )
{
	{
		Program* program = new Program(new Package(new SimpleIdentifier(L"")));
		BOOST_CHECK(program->root->id->toString() == L"<empty>");
	}

	{
		Program* program = new Program();
		BOOST_CHECK(program->root->id->toString() == L"<empty>"); // the default package name is "<empty>"
	}
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_BasicTreeGenerationTestCase3 )
{
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_TreeEqualityTest )
{
    {
        ASTNode::ASTNodeSet visited;
        ASTNode* p1_1 = createSample1() ;
        ASTNode* p1_2 = createSample1() ;
        BOOST_CHECK(p1_1->isEqual(*p1_2, visited));
    }

    {
        ASTNode::ASTNodeSet visited;
        ASTNode* p2_1 = createSample2() ;
        ASTNode* p2_2 = createSample2() ;
        BOOST_CHECK(p2_1->isEqual(*p2_2, visited));
    }

    {
        ASTNode::ASTNodeSet visited;
        ASTNode* p3_1 = createSample3() ;
        ASTNode* p3_2 = createSample3() ;
        BOOST_CHECK(p3_1->isEqual(*p3_2, visited));
    }

    {
        ASTNode::ASTNodeSet visited;
        ASTNode* p4_1 = createSample4() ;
        ASTNode* p4_2 = createSample4() ;
        BOOST_CHECK(p4_1->isEqual(*p4_2, visited));
    }

    {
        ASTNode::ASTNodeSet visited;
        ASTNode* p5_1 = createSample5() ;
        ASTNode* p5_2 = createSample5() ;
        BOOST_CHECK(p5_1->isEqual(*p5_2, visited));
    }
}

BOOST_AUTO_TEST_SUITE_END()
