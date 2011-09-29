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

#ifndef ZILLIANS_LANGUAGE_TREE_INTERNAL_H_
#define ZILLIANS_LANGUAGE_TREE_INTERNAL_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/TypeSpecifier.h"

namespace zillians { namespace language { namespace tree {

struct Internal : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Internal, (Internal)(ASTNode));

	Internal()
	{
		VoidTy     = new TypeSpecifier(PrimitiveType::VOID);
		BooleanTy  = new TypeSpecifier(PrimitiveType::BOOL);
		Int8Ty     = new TypeSpecifier(PrimitiveType::INT8);
		Int16Ty    = new TypeSpecifier(PrimitiveType::INT16);
		Int32Ty    = new TypeSpecifier(PrimitiveType::INT32);
		Int64Ty    = new TypeSpecifier(PrimitiveType::INT64);
		Float32Ty  = new TypeSpecifier(PrimitiveType::FLOAT32);
		Float64Ty  = new TypeSpecifier(PrimitiveType::FLOAT64);
		ObjectTy   = new TypeSpecifier(PrimitiveType::OBJECT);
		FunctionTy = new TypeSpecifier(PrimitiveType::FUNCTION);
		StringTy   = new TypeSpecifier(PrimitiveType::STRING);
	}

	TypeSpecifier* getPrimitiveTy(PrimitiveType::type t)
	{
		switch(t)
		{
		case PrimitiveType::VOID: return VoidTy;
		case PrimitiveType::BOOL: return BooleanTy;
		case PrimitiveType::INT8: return Int8Ty;
		case PrimitiveType::INT16: return Int16Ty;
		case PrimitiveType::INT32: return Int32Ty;
		case PrimitiveType::INT64: return Int64Ty;
		case PrimitiveType::FLOAT32: return Float32Ty;
		case PrimitiveType::FLOAT64: return Float64Ty;
		case PrimitiveType::OBJECT: return ObjectTy;
		case PrimitiveType::FUNCTION: return FunctionTy;
		case PrimitiveType::STRING: return StringTy;
		default: break;
		}
		BOOST_ASSERT(false && "reaching unreachable code");
		return NULL;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(VoidTy)
		COMPARE_MEMBER(BooleanTy)
		COMPARE_MEMBER(Int8Ty)
		COMPARE_MEMBER(Int16Ty)
		COMPARE_MEMBER(Int32Ty)
		COMPARE_MEMBER(Int64Ty)
		COMPARE_MEMBER(Float32Ty)
		COMPARE_MEMBER(Float64Ty)
		COMPARE_MEMBER(ObjectTy)
		COMPARE_MEMBER(FunctionTy)
		COMPARE_MEMBER(StringTy)
		COMPARE_MEMBER(others)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(VoidTy)
		REPLACE_USE_WITH(BooleanTy)
		REPLACE_USE_WITH(Int8Ty)
		REPLACE_USE_WITH(Int16Ty)
		REPLACE_USE_WITH(Int32Ty)
		REPLACE_USE_WITH(Int64Ty)
		REPLACE_USE_WITH(Float32Ty)
		REPLACE_USE_WITH(Float64Ty)
		REPLACE_USE_WITH(ObjectTy)
		REPLACE_USE_WITH(FunctionTy)
		REPLACE_USE_WITH(StringTy)
		REPLACE_USE_WITH(others)
		END_REPLACE()
    }

	TypeSpecifier* VoidTy;
	TypeSpecifier* BooleanTy;
	TypeSpecifier* Int8Ty;
	TypeSpecifier* Int16Ty;
	TypeSpecifier* Int32Ty;
	TypeSpecifier* Int64Ty;
	TypeSpecifier* Float32Ty;
	TypeSpecifier* Float64Ty;
	TypeSpecifier* ObjectTy;
	TypeSpecifier* FunctionTy;
	TypeSpecifier* StringTy;

	std::vector<ASTNode*> others;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_INTERNAL_H_ */
