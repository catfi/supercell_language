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
#include "language/tree/visitor/general/PrettyPrintVisitor.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(ASTNode               , "ASTNode")
BOOST_CLASS_EXPORT_GUID(Annotation            , "Annotation")
BOOST_CLASS_EXPORT_GUID(Annotations           , "Annotations")
BOOST_CLASS_EXPORT_GUID(Program               , "Program")
BOOST_CLASS_EXPORT_GUID(Package               , "Package")
BOOST_CLASS_EXPORT_GUID(Import                , "Import")
BOOST_CLASS_EXPORT_GUID(Block                 , "Block")
BOOST_CLASS_EXPORT_GUID(Identifier            , "Identifier")
BOOST_CLASS_EXPORT_GUID(SimpleIdentifier      , "SimpleIdentifi")
BOOST_CLASS_EXPORT_GUID(NestedIdentifier      , "NestedIdentifi")
BOOST_CLASS_EXPORT_GUID(TemplatedIdentifier   , "TemplatedIdent")
BOOST_CLASS_EXPORT_GUID(Literal               , "Literal")
BOOST_CLASS_EXPORT_GUID(NumericLiteral        , "NumericLiteral")
BOOST_CLASS_EXPORT_GUID(StringLiteral         , "StringLiteral")
BOOST_CLASS_EXPORT_GUID(ObjectLiteral         , "ObjectLiteral")
BOOST_CLASS_EXPORT_GUID(TypeSpecifier         , "TypeSpecifier")
BOOST_CLASS_EXPORT_GUID(FunctionType          , "FunctionType")
BOOST_CLASS_EXPORT_GUID(Declaration           , "Declaration")
BOOST_CLASS_EXPORT_GUID(ClassDecl             , "ClassDecl")
BOOST_CLASS_EXPORT_GUID(EnumDecl              , "EnumDecl")
BOOST_CLASS_EXPORT_GUID(InterfaceDecl         , "InterfaceDecl")
BOOST_CLASS_EXPORT_GUID(TypedefDecl           , "TypedefDecl")
BOOST_CLASS_EXPORT_GUID(FunctionDecl          , "FunctionDecl")
BOOST_CLASS_EXPORT_GUID(VariableDecl          , "VariableDecl")
BOOST_CLASS_EXPORT_GUID(Statement             , "Statement")
BOOST_CLASS_EXPORT_GUID(DeclarativeStmt       , "DeclarativeStm")
BOOST_CLASS_EXPORT_GUID(ExpressionStmt        , "ExpressionStmt")
BOOST_CLASS_EXPORT_GUID(IterativeStmt         , "IterativeStmt")
BOOST_CLASS_EXPORT_GUID(ForeachStmt           , "ForeachStmt")
BOOST_CLASS_EXPORT_GUID(WhileStmt             , "WhileStmt")
BOOST_CLASS_EXPORT_GUID(SelectionStmt         , "SelectionStmt")
BOOST_CLASS_EXPORT_GUID(IfElseStmt            , "IfElseStmt")
BOOST_CLASS_EXPORT_GUID(SwitchStmt            , "SwitchStmt")
BOOST_CLASS_EXPORT_GUID(BranchStmt            , "BranchStmt")
BOOST_CLASS_EXPORT_GUID(Expression            , "Expression")
BOOST_CLASS_EXPORT_GUID(PrimaryExpr           , "PrimaryExpr")
BOOST_CLASS_EXPORT_GUID(UnaryExpr             , "UnaryExpr")
BOOST_CLASS_EXPORT_GUID(BinaryExpr            , "BinaryExpr")
BOOST_CLASS_EXPORT_GUID(TernaryExpr           , "TernaryExpr")
BOOST_CLASS_EXPORT_GUID(MemberExpr            , "MemberExpr")
BOOST_CLASS_EXPORT_GUID(CallExpr              , "CallExpr")
BOOST_CLASS_EXPORT_GUID(CastExpr              , "CastExpr")

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
    ASTNode::ASTNodeSet visited ;
    BOOST_CHECK(origProgram->isEqual(*restoredProgram, visited));
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_SerializationTestCase1 )
{
    CreateSaveRestoreComare(createSample1);
    CreateSaveRestoreComare(createSample2);
    CreateSaveRestoreComare(createSample3);
    CreateSaveRestoreComare(createSample4);
    CreateSaveRestoreComare(createSample5);
}

BOOST_AUTO_TEST_SUITE_END()
