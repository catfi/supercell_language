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
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/visitor/PrettyPrintVisitor.h"
#include "../ASTNodeSamples.h"

#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"

#include <iostream>
#include <string>
#include <limits>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#define BOOST_TEST_MODULE ThorScriptTreeTest_TreeCloneTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::tree;
using namespace zillians::language::tree::visitor;
using namespace zillians::language::stage;

typedef boost::mpl::vector<SourceInfoContext, NameManglingContext> ContextToCloneT;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_TreeCloneTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_TreeCloneTestCase1 )
{
	SimpleIdentifier* original_id = new SimpleIdentifier(L"hello");

	SourceInfoContext* original_src_ctx = new SourceInfoContext(123, 456);
	original_id->set<SourceInfoContext>(original_src_ctx);

	NameManglingContext* original_name_ctx = new NameManglingContext("hello");
	original_id->set<NameManglingContext>(original_name_ctx);

	SimpleIdentifier* cloned_id = cast<SimpleIdentifier>(original_id->clone());

	BOOST_CHECK(cloned_id != NULL);

	ASTNodeHelper::clone<ContextToCloneT>(cloned_id, original_id);

	SourceInfoContext* cloned_src_ctx = cloned_id->get<SourceInfoContext>();
	NameManglingContext* cloned_name_ctx = cloned_id->get<NameManglingContext>();

	BOOST_CHECK(cloned_src_ctx != NULL);

	if(cloned_src_ctx)
	{
		BOOST_CHECK(cloned_src_ctx->line == original_src_ctx->line);
		BOOST_CHECK(cloned_src_ctx->column == original_src_ctx->column);
	}

	BOOST_CHECK(cloned_name_ctx != NULL);

	if(cloned_name_ctx)
	{
		BOOST_CHECK(cloned_name_ctx->managled_name == original_name_ctx->managled_name);
	}

}

BOOST_AUTO_TEST_SUITE_END()
