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
#include "language/tree/visitor/general/ObjectCountVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_GarbageCollectionVisitor
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::tree;
using namespace zillians::language::tree::visitor;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_GarbageCollectionVisitorSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GarbageCollectionVisitorTestCase1 )
{
	Program* node = new Program();

	int count_before_gc = 0;
	{
		ObjectCountVisitor<> counter;
		counter.visit(*node);
		count_before_gc = counter.get_count();
	}

	{
		GarbageCollectionVisitor<> marker;
		marker.visit(*node);
		BOOST_CHECK(marker.get_sweep_count() == 0);
	}

	int count_after_gc = 0;
	{
		ObjectCountVisitor<> counter;
		counter.visit(*node);
		count_after_gc = counter.get_count();
	}

	BOOST_CHECK(count_before_gc == count_after_gc);

	BOOST_CHECK(count_after_gc == ASTNodeGC::instance()->cleanup());
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GarbageCollectionVisitorTestCase2 )
{
	Program* node = new Program(new Package(new SimpleIdentifier(L"test")));

	int count_before_gc = 0;
	{
		ObjectCountVisitor<> counter;
		counter.visit(*node);
		count_before_gc = counter.get_count();
	}

	{
		GarbageCollectionVisitor<> marker;
		marker.visit(*node);
		BOOST_CHECK(marker.get_sweep_count() == 0);
	}

	int count_after_gc = 0;
	{
		ObjectCountVisitor<> counter;
		counter.visit(*node);
		count_after_gc = counter.get_count();
	}

	BOOST_CHECK(count_before_gc == count_after_gc);

	BOOST_CHECK(count_after_gc == ASTNodeGC::instance()->cleanup());
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_GarbageCollectionVisitorTestCase3 )
{
	ASTNode* node = createSample1();
	int count_before_gc = 0;
	{
		ObjectCountVisitor<> counter;
		counter.visit(*node);
		count_before_gc = counter.get_count();
	}

	{
		GarbageCollectionVisitor<> marker;
		marker.visit(*node);
		BOOST_CHECK(marker.get_sweep_count() == 0);
	}

	int count_after_gc = 0;
	{
		ObjectCountVisitor<> counter;
		counter.visit(*node);
		count_after_gc = counter.get_count();
	}

	BOOST_CHECK(count_before_gc == count_after_gc);

	BOOST_CHECK(count_after_gc == ASTNodeGC::instance()->cleanup());
}

BOOST_AUTO_TEST_SUITE_END()
