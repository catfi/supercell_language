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

namespace boost { namespace serialization {

// boost::tuple
#define GENERATE_ELEMENT_SERIALIZE(z,which,unused) \
    ar & boost::serialization::make_nvp("element",t.get< which >());

#define GENERATE_TUPLE_SERIALIZE(z,nargs,unused)                        \
    template< typename Archive, BOOST_PP_ENUM_PARAMS(nargs,typename T) > \
    void serialize(Archive & ar,                                        \
                   boost::tuple< BOOST_PP_ENUM_PARAMS(nargs,T) > & t,   \
                   const unsigned int version)                          \
    {                                                                   \
      BOOST_PP_REPEAT_FROM_TO(0,nargs,GENERATE_ELEMENT_SERIALIZE,~);    \
    }

    BOOST_PP_REPEAT_FROM_TO(1,6,GENERATE_TUPLE_SERIALIZE,~);


// ASTNode
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::ASTNode& node, const unsigned int version)
{
	base_object<zillians::ContextHub<zillians::ContextOwnership::transfer>>(node);
	ar & node.parent;
}

// Annotation
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Annotation& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.attribute_list;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::Annotation* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::Annotation* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	SimpleIdentifier* name;
	ar >> name;
	::new(p) Annotation(name);
}

// Annotations
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Annotations& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.annotation_list;
}

// Program
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Program& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.root;
	ar & node.imports;
}

// Package
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Package& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.children;
	ar & node.objects;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::Package* p, const unsigned int file_version)
{
	ar << p->id;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::Package* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	SimpleIdentifier* id;
	ar >> id;
	::new(p) Package(id);
}

// Import
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Import& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::Import* p, const unsigned int file_version)
{
	ar << p->ns;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::Import* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Identifier* ns;
	ar >> ns;
	::new(p) Import(ns);
}

// Block
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Block& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.is_pipelined_block;
	ar & node.is_async_block;
	ar & node.objects;
}

// Identifier
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Identifier& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
}

// SimpleIdentifier
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::SimpleIdentifier& node, const unsigned int version)
{
	base_object<zillians::language::tree::Identifier>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::SimpleIdentifier* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::SimpleIdentifier* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	std::wstring name;
	ar >> name;
	::new(p) SimpleIdentifier(name);
}

// NestedIdentifier
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::NestedIdentifier& node, const unsigned int version)
{
	base_object<zillians::language::tree::Identifier>(node);
	ar & node.identifier_list;
}

// TemplatedIdentifier
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::TemplatedIdentifier& node, const unsigned int version)
{
	base_object<zillians::language::tree::Identifier>(node);
	ar & node.templated_type_list;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TemplatedIdentifier* p, const unsigned int file_version)
{
	ar << (int&)p->type;
	ar << p->id;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::TemplatedIdentifier* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int type;
	Identifier* id;
	ar >> type;
	ar >> id;

	::new(p) TemplatedIdentifier(static_cast<TemplatedIdentifier::Usage::type>(type), id);
}

// Literal
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Literal& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
}

// NumericLiteral
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::NumericLiteral& node, const unsigned int version)
{
	base_object<zillians::language::tree::Literal>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::NumericLiteral* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ar << (int&)p->type;
	switch(p->type)
	{
	case PrimitiveType::type::BOOL	 : ar << p->value.b  ; break;
	case PrimitiveType::type::UINT8	: ar << p->value.u8 ; break;
	case PrimitiveType::type::UINT16   : ar << p->value.u16; break;
	case PrimitiveType::type::UINT32   : ar << p->value.u32; break;
	case PrimitiveType::type::UINT64   : ar << p->value.u64; break;
	case PrimitiveType::type::INT8	 : ar << p->value.i8 ; break;
	case PrimitiveType::type::INT16	: ar << p->value.i16; break;
	case PrimitiveType::type::INT32	: ar << p->value.i32; break;
	case PrimitiveType::type::INT64	: ar << p->value.i64; break;
	case PrimitiveType::type::FLOAT32  : ar << p->value.f32; break;
	case PrimitiveType::type::FLOAT64  : ar << p->value.f64; break;
	}
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::NumericLiteral* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int type;
	ar >> type;

	bool			  b   ;
	zillians::int8	i8  ;
	zillians::int16   i16 ;
	zillians::int32   i32 ;
	zillians::int64   i64 ;
	zillians::uint8   u8  ;
	zillians::uint16  u16 ;
	zillians::uint32  u32 ;
	zillians::uint64  u64 ;
	float			 f32 ;
	double			f64 ;

	switch(type)
	{
	case PrimitiveType::type::BOOL	 : ar >> b  ; ::new(p) NumericLiteral(b  ); break;
	case PrimitiveType::type::UINT8	: ar >> u8 ; ::new(p) NumericLiteral(u8 ); break;
	case PrimitiveType::type::UINT16   : ar >> u16; ::new(p) NumericLiteral(u16); break;
	case PrimitiveType::type::UINT32   : ar >> u32; ::new(p) NumericLiteral(u32); break;
	case PrimitiveType::type::UINT64   : ar >> u64; ::new(p) NumericLiteral(u64); break;
	case PrimitiveType::type::INT8	 : ar >> i8 ; ::new(p) NumericLiteral(i8 ); break;
	case PrimitiveType::type::INT16	: ar >> i16; ::new(p) NumericLiteral(i16); break;
	case PrimitiveType::type::INT32	: ar >> i32; ::new(p) NumericLiteral(i32); break;
	case PrimitiveType::type::INT64	: ar >> i64; ::new(p) NumericLiteral(i64); break;
	case PrimitiveType::type::FLOAT32  : ar >> f32; ::new(p) NumericLiteral(f32); break;
	case PrimitiveType::type::FLOAT64  : ar >> f64; ::new(p) NumericLiteral(f64); break;
	}
}

// StringLiteral
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::StringLiteral& node, const unsigned int version)
{
	base_object<zillians::language::tree::Literal>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::StringLiteral* p, const unsigned int file_version)
{
	ar << p->value;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::StringLiteral* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;
	std::wstring value;
	ar >> value;
	new(p) StringLiteral(value);
}

// ObjectLiteral
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::ObjectLiteral& node, const unsigned int version)
{
	base_object<zillians::language::tree::Literal>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ObjectLiteral* p, const unsigned int file_version)
{
	ar << (int&)p->type;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ObjectLiteral* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;
	int type;
	ar >> type;
	new(p) ObjectLiteral(static_cast<ObjectLiteral::LiteralType::type>(type));
}

// TypeSpecifier
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::TypeSpecifier& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TypeSpecifier* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ar << p->type;
	switch(p->type)
	{
	case TypeSpecifier::ReferredType::FUNCTION_TYPE	 : ar << p->referred.function_type  ; break ;
	case TypeSpecifier::ReferredType::PRIMITIVE		 : ar << p->referred.primitive	    ; break ;
	case TypeSpecifier::ReferredType::UNSPECIFIED	 : ar << p->referred.unspecified	; break ;
	}
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::TypeSpecifier* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ClassDecl* class_decl;
	InterfaceDecl* interface_decl;
	FunctionDecl* function_decl;
	EnumDecl* enum_decl;
	TypedefDecl* typedef_decl;
	FunctionType* function_type;
	PrimitiveType::type primitive;
	Identifier* unspecified;

	int type;
	ar >> type;

	switch(static_cast<TypeSpecifier::ReferredType::type>(type))
	{
	case TypeSpecifier::ReferredType::FUNCTION_TYPE	 : ar >> function_type  ; ::new(p) TypeSpecifier(function_type ); break ;
	case TypeSpecifier::ReferredType::PRIMITIVE		 : ar >> primitive	    ; ::new(p) TypeSpecifier(primitive	 );   break ;
	case TypeSpecifier::ReferredType::UNSPECIFIED	 : ar >> unspecified	; ::new(p) TypeSpecifier(unspecified   ); break ;
	}
}

// FunctionType
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::FunctionType& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.templated_parameters;
	ar & node.argument_types;
	ar & node.return_type;
}

// Declaration
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Declaration& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.annotations;
}

// ClassDecl
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::ClassDecl& node, const unsigned int version)
{
	base_object<zillians::language::tree::Declaration>(node);
	ar & node.base;
	ar & node.implements;
	ar & node.member_functions;
	ar & node.member_variables;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ClassDecl* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ClassDecl* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Identifier* name;
	ar >> name;
	::new(p) ClassDecl(name);
}

// EnumDecl
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::EnumDecl& node, const unsigned int version)
{
	base_object<zillians::language::tree::Declaration>(node);
	ar & node.enumeration_list;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::EnumDecl* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::EnumDecl* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Identifier* name;
	ar >> name;
	::new(p) EnumDecl(name);
}

// InterfaceDecl
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::InterfaceDecl& node, const unsigned int version)
{
	base_object<zillians::language::tree::Declaration>(node);
	ar & node.member_functions;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::InterfaceDecl* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::InterfaceDecl* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Identifier* name;
	ar >> name;
	::new(p) InterfaceDecl(name);
}

// TypedefDecl
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::TypedefDecl& node, const unsigned int version)
{
	base_object<zillians::language::tree::Declaration>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TypedefDecl* p, const unsigned int file_version)
{
	ar << p->from;
	ar << p->to;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::TypedefDecl* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	TypeSpecifier* from;
	SimpleIdentifier* to;
	ar >> from;
	ar >> to;
	::new(p) TypedefDecl(from, to);
}

// FunctionDecl
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::FunctionDecl& node, const unsigned int version)
{
	base_object<zillians::language::tree::Declaration>(node);
	ar & node.parameters;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::FunctionDecl* p, const unsigned int file_version)
{
	ar << p->name;
	ar << p->type;
	ar << p->is_member;
	ar << p->is_static;
	ar << p->visibility;
	ar << p->block;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::FunctionDecl* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Identifier* name;
	TypeSpecifier* type;
	bool is_member;
	bool is_static;
	Declaration::VisibilitySpecifier::type visibility;
	Block* block;

	ar >> name;
	ar >> type;
	ar >> is_member;
	ar >> is_static;
	ar >> visibility;
	ar >> block;

	::new(p) FunctionDecl(name, type, is_member, is_static, visibility, block);
}

// VariableDecl
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::VariableDecl& node, const unsigned int version)
{
	base_object<zillians::language::tree::Declaration>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::VariableDecl* p, const unsigned int file_version)
{
	ar << p->name;
	ar << p->type;
	ar << p->is_member;
	ar << p->is_static;
	ar << p->is_const;
	ar << p->visibility;
	ar << p->initializer;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::VariableDecl* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Identifier* name;
	TypeSpecifier* type;
	bool is_member;
	bool is_static;
	bool is_const;
	Declaration::VisibilitySpecifier::type visibility;
	ASTNode* initializer;

	ar >> name;
	ar >> type;
	ar >> is_member;
	ar >> is_static;
	ar >> is_const;
	ar >> visibility;
	ar >> initializer;

	::new(p) VariableDecl(name, type, is_member, is_static, is_const, visibility, initializer);
}

// Statement
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Statement& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.annotations;
}

// DeclarativeStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::DeclarativeStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::Statement>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::DeclarativeStmt* p, const unsigned int file_version)
{
	ar << p->declaration;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::DeclarativeStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Declaration* declaration;
	ar >> declaration;
	::new(p) DeclarativeStmt(declaration);
}

// ExpressionStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::ExpressionStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::Statement>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ExpressionStmt* p, const unsigned int file_version)
{
	ar << p->expr;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ExpressionStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Expression* expr;
	ar >> expr;
	::new(p) ExpressionStmt(expr);
}

// IterativeStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::IterativeStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::Statement>(node);
}

// ForStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::ForStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::IterativeStmt>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ForStmt* p, const unsigned int file_version)
{
	ar << p->init;
	ar << p->cond;
	ar << p->step;
	ar << p->block;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ForStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ASTNode* init; // TODO semantic-check: it must be L-value expression or declarative statement
	ASTNode* cond;
	ASTNode* step;
	ASTNode* block;

	ar >> init;
	ar >> cond;
	ar >> step;
	ar >> block;

	::new(p) ForStmt(init, cond, block);
}

// ForeachStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::ForeachStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::IterativeStmt>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ForeachStmt* p, const unsigned int file_version)
{
	ar << p->iterator;
	ar << p->range;
	ar << p->block;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ForeachStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ASTNode* iterator; // TODO semantic-check: it must be L-value expression or declarative statement
	Expression* range;
	ASTNode* block;

	ar >> iterator;
	ar >> range;
	ar >> block;

	::new(p) ForeachStmt(iterator, range, block);
}

// WhileStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::WhileStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::IterativeStmt>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::WhileStmt* p, const unsigned int file_version)
{
	ar << (int&)p->style;
	ar << p->cond;
	ar << p->block;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::WhileStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int style;
	Expression* cond;
	ASTNode* block;

	ar >> style;
	ar >> cond;
	ar >> block;

	::new(p) WhileStmt(static_cast<WhileStmt::Style::type>(style), cond, block);
}

// Selection struct
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Selection& selection, const unsigned int version)
{
	ar & selection.cond;
	ar & selection.block;
}

// SelectionStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::SelectionStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::Statement>(node);
}

// IfElseStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::IfElseStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::SelectionStmt>(node);
	ar & node.elseif_branches;
	ar & node.else_block;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::IfElseStmt* p, const unsigned int file_version)
{
	ar << p->if_branch;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::IfElseStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Expression* selectionExpression;
	ASTNode*	selectionASTNode;
	ar >> selectionExpression;
	ar >> selectionASTNode;
	Selection if_branch(selectionExpression, selectionASTNode);
	::new(p) IfElseStmt(if_branch);
}

// SwitchStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::SwitchStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::SelectionStmt>(node);
	ar & node.cases;
	ar & node.default_block;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::SwitchStmt* p, const unsigned int file_version)
{
	ar << p->node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::SwitchStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Expression* node;
	ar >> node;
	::new(p) SwitchStmt(node);
}

// BranchStmt
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::BranchStmt& node, const unsigned int version)
{
	base_object<zillians::language::tree::Statement>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::BranchStmt* p, const unsigned int file_version)
{
	ar << p->opcode;
	ar << p->result;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::BranchStmt* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int opcode;
	ASTNode* result;
	ar >> opcode;
	ar >> result;
	::new(p) BranchStmt(static_cast<BranchStmt::OpCode::type>(opcode), result);
}

// Expression
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::Expression& node, const unsigned int version)
{
	base_object<zillians::language::tree::ASTNode>(node);
	ar & node.annotations;
}

// PrimaryExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::PrimaryExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::PrimaryExpr* p, const unsigned int file_version)
{
	ar << (int&)p->catagory ;
	switch(p->catagory)
	{
	case zillians::language::tree::PrimaryExpr::Catagory::IDENTIFIER : ar << p->value.identifier; break;
	case zillians::language::tree::PrimaryExpr::Catagory::LITERAL	: ar << p->value.literal; break;
	case zillians::language::tree::PrimaryExpr::Catagory::LAMBDA	 : ar << p->value.lambda; break;
	}
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::PrimaryExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int type;
	ar >> type;
	
	switch(static_cast<PrimaryExpr::Catagory::type>(type))
	{
	case PrimaryExpr::Catagory::IDENTIFIER :
		Identifier* identifier;
		ar >> identifier;
		::new(p) PrimaryExpr(identifier);
		break;
	case PrimaryExpr::Catagory::LITERAL	:
		Literal* literal;
		ar >> literal;
		::new(p) PrimaryExpr(literal);
		break;
	case PrimaryExpr::Catagory::LAMBDA	 :
		FunctionDecl* functionDecl;
		ar >> functionDecl;
		::new(p) PrimaryExpr(functionDecl);
		break;
	}
}

// UnaryExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::UnaryExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::UnaryExpr* p, const unsigned int file_version)
{
	ar << (int&)p->opcode;
	ar << p->node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::UnaryExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int opcode;
	ASTNode* node;

	ar >> opcode;
	ar >> node;

	::new(p) UnaryExpr(static_cast<UnaryExpr::OpCode::type>(opcode), node);
}

// BinaryExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::BinaryExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::BinaryExpr* p, const unsigned int file_version)
{
	ar << (int&)p->opcode;
	ar << p->left;
	ar << p->right;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::BinaryExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	int opcode;
	Expression* left;
	Expression* right;

	ar >> opcode;
	ar >> left;
	ar >> right;

	::new(p) BinaryExpr(static_cast<BinaryExpr::OpCode::type>(opcode), left, right);
}

// TernaryExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::TernaryExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TernaryExpr* p, const unsigned int file_version)
{
	ar << p->cond;
	ar << p->true_node;
	ar << p->false_node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::TernaryExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Expression* cond;
	Expression* true_node;
	Expression* false_node;

	ar >> cond;
	ar >> true_node;
	ar >> false_node;

	::new(p) TernaryExpr(cond, true_node, false_node);
}

// MemberExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::MemberExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::MemberExpr* p, const unsigned int file_version)
{
	ar << p->node;
	ar << p->member;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::MemberExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ASTNode* node;
	Identifier* member;
	ar >> node;
	ar >> member;
	::new(p) MemberExpr(node, member);
}

// CallExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::CallExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
	ar & node.parameters;
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::CallExpr* p, const unsigned int file_version)
{
	ar << p->node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::CallExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	ASTNode* node;
	ar >> node;
	::new(p) CallExpr(node);
}

// CastExpr
template<typename Archive>
void serialize(Archive& ar, zillians::language::tree::CastExpr& node, const unsigned int version)
{
	base_object<zillians::language::tree::Expression>(node);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::CastExpr* p, const unsigned int file_version)
{
	ar << p->node;
	ar << p->type;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::CastExpr* p, const unsigned int file_version)
{
	using namespace zillians::language::tree;

	Expression* node;
	TypeSpecifier* type;
	ar >> node;
	ar >> type;
	::new(p) CastExpr(node, type);
}


} }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_ */
