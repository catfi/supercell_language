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
		UInt8Ty    = new TypeSpecifier(PrimitiveType::UINT8);
		UInt16Ty   = new TypeSpecifier(PrimitiveType::UINT16);
		UInt32Ty   = new TypeSpecifier(PrimitiveType::UINT32);
		UInt64Ty   = new TypeSpecifier(PrimitiveType::UINT64);
		Int8Ty     = new TypeSpecifier(PrimitiveType::INT8);
		Int16Ty    = new TypeSpecifier(PrimitiveType::INT16);
		Int32Ty    = new TypeSpecifier(PrimitiveType::INT32);
		Int64Ty    = new TypeSpecifier(PrimitiveType::INT64);
		Float32Ty  = new TypeSpecifier(PrimitiveType::FLOAT32);
		Float64Ty  = new TypeSpecifier(PrimitiveType::FLOAT64);
		ObjectTy   = new TypeSpecifier(PrimitiveType::ANONYMOUS_OBJECT);
		FunctionTy = new TypeSpecifier(PrimitiveType::ANONYMOUS_FUNCTION);
	}

	TypeSpecifier* getPrimitiveTy(PrimitiveType::type t)
	{
		switch(t)
		{
		case PrimitiveType::VOID: return VoidTy;
		case PrimitiveType::BOOL: return BooleanTy;
		case PrimitiveType::UINT8: return UInt8Ty;
		case PrimitiveType::UINT16: return UInt16Ty;
		case PrimitiveType::UINT32: return UInt32Ty;
		case PrimitiveType::UINT64: return UInt64Ty;
		case PrimitiveType::INT8: return Int8Ty;
		case PrimitiveType::INT16: return Int16Ty;
		case PrimitiveType::INT32: return Int32Ty;
		case PrimitiveType::INT64: return Int64Ty;
		case PrimitiveType::FLOAT32: return Float32Ty;
		case PrimitiveType::FLOAT64: return Float64Ty;
		case PrimitiveType::ANONYMOUS_OBJECT: return ObjectTy;
		case PrimitiveType::ANONYMOUS_FUNCTION: return FunctionTy;
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
		COMPARE_MEMBER(UInt8Ty)
		COMPARE_MEMBER(UInt16Ty)
		COMPARE_MEMBER(UInt32Ty)
		COMPARE_MEMBER(UInt64Ty)
		COMPARE_MEMBER(Int8Ty)
		COMPARE_MEMBER(Int16Ty)
		COMPARE_MEMBER(Int32Ty)
		COMPARE_MEMBER(Int64Ty)
		COMPARE_MEMBER(Float32Ty)
		COMPARE_MEMBER(Float64Ty)
		COMPARE_MEMBER(ObjectTy)
		COMPARE_MEMBER(FunctionTy)
		COMPARE_MEMBER(others)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(VoidTy)
		REPLACE_USE_WITH(BooleanTy)
		REPLACE_USE_WITH(UInt8Ty)
		REPLACE_USE_WITH(UInt16Ty)
		REPLACE_USE_WITH(UInt32Ty)
		REPLACE_USE_WITH(UInt64Ty)
		REPLACE_USE_WITH(Int8Ty)
		REPLACE_USE_WITH(Int16Ty)
		REPLACE_USE_WITH(Int32Ty)
		REPLACE_USE_WITH(Int64Ty)
		REPLACE_USE_WITH(Float32Ty)
		REPLACE_USE_WITH(Float64Ty)
		REPLACE_USE_WITH(ObjectTy)
		REPLACE_USE_WITH(FunctionTy)
		REPLACE_USE_WITH(others)
		END_REPLACE()
    }

	TypeSpecifier* VoidTy;
	TypeSpecifier* BooleanTy;
	TypeSpecifier* UInt8Ty;
	TypeSpecifier* UInt16Ty;
	TypeSpecifier* UInt32Ty;
	TypeSpecifier* UInt64Ty;
	TypeSpecifier* Int8Ty;
	TypeSpecifier* Int16Ty;
	TypeSpecifier* Int32Ty;
	TypeSpecifier* Int64Ty;
	TypeSpecifier* Float32Ty;
	TypeSpecifier* Float64Ty;
	TypeSpecifier* ObjectTy;
	TypeSpecifier* FunctionTy;

	std::vector<ASTNode*> others;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_INTERNAL_H_ */
