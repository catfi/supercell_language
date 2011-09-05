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

#ifndef ZILLIANS_LANGUAGE_TREE_TYPESPECIFIER_H_
#define ZILLIANS_LANGUAGE_TREE_TYPESPECIFIER_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Primitive.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/FunctionType.h"

namespace zillians { namespace language { namespace tree {

class ClassDecl;
class InterfaceDecl;
class EnumDecl;
class FunctionDecl;
class TypedefDecl;

struct TypeSpecifier : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(TypeSpecifier, (TypeSpecifier)(ASTNode));

	struct ReferredType
	{
		enum type {
			CLASS_DECL,
			INTERFACE_DECL,
			FUNCTION_DECL,
			ENUM_DECL,
			TYPEDEF_DECL,
			FUNCTION_TYPE,
			PRIMITIVE,
			UNSPECIFIED,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case CLASS_DECL:		return L"class_declaration";
			case INTERFACE_DECL:	return L"interface_declaration";
			case FUNCTION_DECL:		return L"function_declaration";
			case ENUM_DECL:			return L"enum_declaration";
			case TYPEDEF_DECL:		return L"typedef_declaration";
			case FUNCTION_TYPE:		return L"function_type";
			case PRIMITIVE:			return L"primitive";
			case UNSPECIFIED: 		return L"unspecified";
			}
		}
	};

	explicit TypeSpecifier(ClassDecl* class_decl)
	{
		update(class_decl);
	}

	explicit TypeSpecifier(InterfaceDecl* interface_decl)
	{
		update(interface_decl);
	}

	explicit TypeSpecifier(FunctionDecl* function_decl)
	{
		update(function_decl);
	}

	explicit TypeSpecifier(EnumDecl* enum_decl)
	{
		update(enum_decl);
	}

	explicit TypeSpecifier(TypedefDecl* typedef_decl)
	{
		update(typedef_decl);
	}

	explicit TypeSpecifier(FunctionType* function_proto)
	{
		update(function_proto);
	}

	explicit TypeSpecifier(PrimitiveType::type primitive)
	{
		update(primitive);
	}

	explicit TypeSpecifier(Identifier* unspecified)
	{
		update(unspecified);
	}

	void update(ClassDecl* class_decl)
	{
		type = ReferredType::CLASS_DECL;
		referred.class_decl = class_decl;
	}

	void update(InterfaceDecl* interface_decl)
	{
		type = ReferredType::INTERFACE_DECL;
		referred.interface_decl = interface_decl;
	}

	void update(FunctionDecl* function_decl)
	{
		type = ReferredType::FUNCTION_DECL;
		referred.function_decl = function_decl;
	}

	void update(EnumDecl* enum_decl)
	{
		type = ReferredType::ENUM_DECL;
		referred.enum_decl = enum_decl;
	}

	void update(TypedefDecl* typedef_decl)
	{
		type = ReferredType::TYPEDEF_DECL;
		referred.typedef_decl = typedef_decl;
	}

	void update(FunctionType* function_type)
	{
		type = ReferredType::FUNCTION_TYPE;
		referred.function_type = function_type;
	}

	void update(PrimitiveType::type primitive)
	{
		type = ReferredType::PRIMITIVE;
		referred.primitive = primitive;
	}

	void update(Identifier* unspecified)
	{
		type = ReferredType::UNSPECIFIED;
		referred.unspecified = unspecified;
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<ASTNode>(*this);
    }

	ReferredType::type type;

	union
	{
		ClassDecl* class_decl;
		InterfaceDecl* interface_decl;
		FunctionDecl* function_decl;
		EnumDecl* enum_decl;
		TypedefDecl* typedef_decl;
		FunctionType* function_type;
		PrimitiveType::type primitive;
		Identifier* unspecified;
	} referred;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TypeSpecifier* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

    ar << p->type;
    switch(p->type)
    {
    case TypeSpecifier::ReferredType::CLASS_DECL        : ar << p->referred.class_decl     ; break ;
    case TypeSpecifier::ReferredType::INTERFACE_DECL    : ar << p->referred.interface_decl ; break ;
    case TypeSpecifier::ReferredType::FUNCTION_DECL     : ar << p->referred.function_decl  ; break ;
    case TypeSpecifier::ReferredType::ENUM_DECL         : ar << p->referred.enum_decl      ; break ;
    case TypeSpecifier::ReferredType::TYPEDEF_DECL      : ar << p->referred.typedef_decl   ; break ;
    case TypeSpecifier::ReferredType::FUNCTION_TYPE     : ar << p->referred.function_type  ; break ;
    case TypeSpecifier::ReferredType::PRIMITIVE         : ar << p->referred.primitive      ; break ;
    case TypeSpecifier::ReferredType::UNSPECIFIED       : ar << p->referred.unspecified    ; break ;
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
    case TypeSpecifier::ReferredType::CLASS_DECL        : ar >> class_decl     ; ::new(p) TypeSpecifier(class_decl    ); break ;
    case TypeSpecifier::ReferredType::INTERFACE_DECL    : ar >> interface_decl ; ::new(p) TypeSpecifier(interface_decl); break ;
    case TypeSpecifier::ReferredType::FUNCTION_DECL     : ar >> function_decl  ; ::new(p) TypeSpecifier(function_decl ); break ;
    case TypeSpecifier::ReferredType::ENUM_DECL         : ar >> enum_decl      ; ::new(p) TypeSpecifier(enum_decl     ); break ;
    case TypeSpecifier::ReferredType::TYPEDEF_DECL      : ar >> typedef_decl   ; ::new(p) TypeSpecifier(typedef_decl  ); break ;
    case TypeSpecifier::ReferredType::FUNCTION_TYPE     : ar >> function_type  ; ::new(p) TypeSpecifier(function_type ); break ;
    case TypeSpecifier::ReferredType::PRIMITIVE         : ar >> primitive      ; ::new(p) TypeSpecifier(primitive     ); break ;
    case TypeSpecifier::ReferredType::UNSPECIFIED       : ar >> unspecified    ; ::new(p) TypeSpecifier(unspecified   ); break ;
    }
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_TYPESPECIFIER_H_ */
