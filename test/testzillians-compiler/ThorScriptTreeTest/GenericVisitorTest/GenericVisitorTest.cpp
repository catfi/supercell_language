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
#include "compiler/tree/visitor/general/GenericVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_GenericVisitorTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::compiler::tree;
using namespace zillians::compiler::tree::visitor;

struct DummyCountVisitor : GenericVisitor
{
	CREATE_INVOKER(countInvoker, count);

	DummyCountVisitor() : total_count(0L)
	{
		registerInvoker<DummyCountVisitor, countInvoker>(this);
	}

	//////////////////////////////////////////////////////////////////////
	/// Basic

	void count(const ASTNode& node)
	{
		++total_count; apply(node);
	}

	void count(const Annotation& node)
	{
		++total_count; apply(node);
	}

	void count(const Annotations& node)
	{
		++total_count; apply(node);
	}

	void count(const Block& node)
	{
		++total_count; apply(node);
	}

	void count(const SimpleIdentifier& node)
	{
		++total_count; apply(node);
	}

	void count(const NestedIdentifier& node)
	{
		++total_count; apply(node);
	}

	void count(const TemplatedIdentifier& node)
	{
		++total_count; apply(node);
	}

	//////////////////////////////////////////////////////////////////////
	/// Module

	void count(const Program& node)
	{
		++total_count; apply(node);
	}

	void count(const Package& node)
	{
		++total_count; apply(node);
	}

	void count(const Import& node)
	{
		++total_count; apply(node);
	}

	//////////////////////////////////////////////////////////////////////
	/// Declaration

	void count(const ClassDecl& node)
	{
		++total_count; apply(node);
	}

	void count(const EnumDecl& node)
	{
		++total_count; apply(node);
	}

	void count(const FunctionDecl& node)
	{
		++total_count; apply(node);
	}

	void count(const InterfaceDecl& node)
	{
		++total_count; apply(node);
	}

	void count(const TypedefDecl& node)
	{
		++total_count; apply(node);
	}

	void count(const VariableDecl& node)
	{
		++total_count; apply(node);
	}

	//////////////////////////////////////////////////////////////////////
	/// Statement

	void count(const DeclarativeStmt& node)
	{
		++total_count; apply(node);
	}

	void count(const ExpressionStmt& node)
	{
		++total_count; apply(node);
	}

	void count(const ForeachStmt& node)
	{
		++total_count; apply(node);
	}

	void count(const WhileStmt& node)
	{
		++total_count; apply(node);
	}

	void count(const IfElseStmt& node)
	{
		++total_count; apply(node);
	}

	void count(const SwitchStmt& node)
	{
		++total_count; apply(node);
	}

	void count(const BranchStmt& node)
	{
		++total_count; apply(node);
	}

	//////////////////////////////////////////////////////////////////////
	/// Expression

	void count(const UnaryExpr& node)
	{
		++total_count; apply(node);
	}

	void count(const BinaryExpr& node)
	{
		++total_count; apply(node);
	}

	void count(const TernaryExpr& node)
	{
		++total_count; apply(node);
	}

	void count(const MemberExpr& node)
	{
		++total_count; apply(node);
	}

	void count(const CallExpr& node)
	{
		++total_count; apply(node);
	}

	void count(const CastExpr& node)
	{
		++total_count; apply(node);
	}

	std::size_t total_count;
};

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_GenericVisitorTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GenericVisitorTestCase1 )
{
	DummyCountVisitor counter;

	ASTNode* program = createSample1();
	counter.visit(*program);

	std::cout << "total node count = " << counter.total_count << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
