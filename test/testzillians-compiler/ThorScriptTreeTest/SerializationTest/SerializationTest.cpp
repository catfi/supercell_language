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
#include "language/tree/ASTNodeSerialization.h"
#include "language/tree/visitor/PrettyPrintVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#define BOOST_TEST_MODULE ThorScriptTreeTest_SerializationTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::tree;
using namespace zillians::language::tree::visitor;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_SerializationTestSuite )

template <typename TreeCreateFunction>
bool CreateSaveRestoreComare(TreeCreateFunction f)
{
    // create and save program ast to file
    ASTNode* origProgram = f();
    {
        std::ofstream ofs("p1.txt");
        boost::archive::text_oarchive oa(ofs);
        oa << origProgram;
    }

    // restore ast from saved file
    ASTNode* restoredProgram = NULL;
    {
        std::ifstream ifs("p1.txt");
        boost::archive::text_iarchive ia(ifs);
        ia >> restoredProgram;
    }

    // compare
    BOOST_CHECK(origProgram->isEqual(*restoredProgram));
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_SerializationTestCase1 )
{
    CreateSaveRestoreComare(createSample1);
    CreateSaveRestoreComare(createSample2);
    CreateSaveRestoreComare(createSample3);
    CreateSaveRestoreComare(createSample4);
}

BOOST_AUTO_TEST_SUITE_END()
