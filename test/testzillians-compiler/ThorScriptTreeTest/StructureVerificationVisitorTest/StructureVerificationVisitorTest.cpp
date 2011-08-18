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
#include "compiler/tree/ASTNode.h"
#include "compiler/tree/ASTNodeFactory.h"
#include "compiler/tree/visitor/stage1/StructureVerificationVisitor.h"
#include "compiler/tree/visitor/general/PrettyPrintVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_StructureVerificationVisitorTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::compiler::tree;
using namespace zillians::compiler::tree::visitor;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_GenericVisitorTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GenericVisitorTestCase1 )
{
	stage1::StructureVerificationVisitor<Composed::FALSE> v;

	ASTNode* program = createSample3();
	v.visit(*program);

	BOOST_CHECK(v.passed == true && "since there's no unspecified type specifier in sample3, structure verification should pass");
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GenericVisitorTestCase2 )
{
	stage1::StructureVerificationVisitor<Composed::FALSE> v;

	ASTNode* program = createSample4();
	v.visit(*program);

	BOOST_CHECK(v.passed == false && "since there's unspecified type specifier in sample3, structure verification should fail");
	if(!v.passed)
	{
		BOOST_CHECK(v.unspecified_nodes.size() == 4 && "that's the total number of unspecified type specifier in sample4");
		PrettyPrintVisitor printer;
		foreach(i, v.unspecified_nodes)
			printer.visit(**i);
	}
}

BOOST_AUTO_TEST_SUITE_END()
