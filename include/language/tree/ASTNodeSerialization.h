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
 * @date Sep 5, 2011 yoco - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_
#define ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_

#include "ASTNodeFactory.h"
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(zillians::language::tree::ASTNode               , "ASTNode")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Annotation            , "Annotation")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Annotations           , "Annotations")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Program               , "Program")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Package               , "Package")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Import                , "Import")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Block                 , "Block")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::Identifier            , "Identifier")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::SimpleIdentifier      , "SimpleIdentifi")
BOOST_CLASS_EXPORT_GUID(zillians::language::tree::NestedIdentifier      , "NestedIdentifi")
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

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_ */
