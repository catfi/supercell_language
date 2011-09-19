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
			FUNCTION_TYPE,
			PRIMITIVE,
			UNSPECIFIED,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case FUNCTION_TYPE:		return L"function_type";
			case PRIMITIVE:			return L"primitive";
			case UNSPECIFIED: 		return L"unspecified";
			}
		}
	};

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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(type)
        switch(p->type)
        {
        case TypeSpecifier::ReferredType::FUNCTION_TYPE  : COMPARE_MEMBER(referred.function_type )             ; break;
        case TypeSpecifier::ReferredType::PRIMITIVE      : if(referred.primitive != p->referred.primitive) return false; break;
        case TypeSpecifier::ReferredType::UNSPECIFIED    : COMPARE_MEMBER(referred.unspecified   )             ; break;
        }
        END_COMPARE()
    }

	ReferredType::type type;

	union ReferredUnion
	{
		FunctionType* function_type;
		PrimitiveType::type primitive;
		Identifier* unspecified;
	} referred;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_TYPESPECIFIER_H_ */
