/**
 * Zillians MMO
 * Copyright (C) 2007-2011 Zillians.com, Inc.
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

#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/stage/serialization/visitor/ASTDeserializationStageVisitor.h"
#include "language/stage/serialization/visitor/ASTSerializationStageVisitor.h"

BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ASTNode               , "ASTNode")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Annotation            , "Annotation")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Annotations           , "Annotations")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Internal              , "Internal")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Program               , "Program")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Package               , "Package")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Import                , "Import")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Block                 , "Block")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Identifier            , "Identifier")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::SimpleIdentifier      , "SimpleIdentifier")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::NestedIdentifier      , "NestedIdentifier")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::TemplatedIdentifier   , "TemplatedIdent")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Literal               , "Literal")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::NumericLiteral        , "NumericLiteral")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::StringLiteral         , "StringLiteral")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ObjectLiteral         , "ObjectLiteral")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::TypeSpecifier         , "TypeSpecifier")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::FunctionType          , "FunctionType")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Declaration           , "Declaration")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ClassDecl             , "ClassDecl")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::EnumDecl              , "EnumDecl")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::InterfaceDecl         , "InterfaceDecl")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::TypedefDecl           , "TypedefDecl")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::FunctionDecl          , "FunctionDecl")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::VariableDecl          , "VariableDecl")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Statement             , "Statement")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::DeclarativeStmt       , "DeclarativeStm")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ExpressionStmt        , "ExpressionStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::IterativeStmt         , "IterativeStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ForStmt               , "ForStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ForeachStmt           , "ForeachStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::WhileStmt             , "WhileStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::SelectionStmt         , "SelectionStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::IfElseStmt            , "IfElseStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::SwitchStmt            , "SwitchStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::BranchStmt            , "BranchStmt")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Expression            , "Expression")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::PrimaryExpr           , "PrimaryExpr")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::UnaryExpr             , "UnaryExpr")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::BinaryExpr            , "BinaryExpr")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::TernaryExpr           , "TernaryExpr")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::MemberExpr            , "MemberExpr")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::CallExpr              , "CallExpr")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::CastExpr              , "CastExpr")

namespace zillians { namespace language { namespace stage {

bool ASTSerializationHelper::serialize(const std::string& filename, tree::ASTNode* node)
{
    std::ofstream ofs(filename);
    if(!ofs.good()) return false;

    // serialize through boost archive
    boost::archive::text_oarchive oa(ofs);
    tree::ASTNode* to_serialize = node;
    oa << to_serialize;

	// de-serialize all objects attached to ContextHub
	// see ASTSerializationStageVisitor::FullDeserializer, which defines the context object types needed to be serialized
    visitor::ASTSerializationStageVisitor<boost::archive::text_oarchive> serialzer(oa);
    serialzer.visit(*to_serialize);
}

tree::ASTNode* ASTSerializationHelper::deserialize(const std::string& filename)
{
	std::ifstream ifs(filename);
	if(!ifs.good()) return NULL;

	// de-serialize through boost archive
	boost::archive::text_iarchive ia(ifs);
	tree::ASTNode* from_serialize = NULL;
	ia >> from_serialize;

	// de-serialize all objects attached to ContextHub
	// see ASTDeserializationStageVisitor::FullDeserializer, which defines the context object types needed to be de-serialized
	visitor::ASTDeserializationStageVisitor<boost::archive::text_iarchive> deserialzer(ia);
	deserialzer.visit(*from_serialize);

	return from_serialize;
}

} } }
