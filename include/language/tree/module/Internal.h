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
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Internal, (Internal)(ASTNode));

	Internal()
	{
		VoidTy     = new TypeSpecifier(PrimitiveType::VOID_TYPE);
		BooleanTy  = new TypeSpecifier(PrimitiveType::BOOL_TYPE);
		Int8Ty     = new TypeSpecifier(PrimitiveType::INT8_TYPE);
		Int16Ty    = new TypeSpecifier(PrimitiveType::INT16_TYPE);
		Int32Ty    = new TypeSpecifier(PrimitiveType::INT32_TYPE);
		Int64Ty    = new TypeSpecifier(PrimitiveType::INT64_TYPE);
		Float32Ty  = new TypeSpecifier(PrimitiveType::FLOAT32_TYPE);
		Float64Ty  = new TypeSpecifier(PrimitiveType::FLOAT64_TYPE);
		ObjectTy   = new TypeSpecifier(PrimitiveType::OBJECT_TYPE);
		FunctionTy = new TypeSpecifier(PrimitiveType::FUNCTION_TYPE);
		StringTy   = new TypeSpecifier(PrimitiveType::STRING_TYPE);

		VoidTy->parent     = this;
		BooleanTy->parent  = this;
		Int8Ty->parent     = this;
		Int16Ty->parent    = this;
		Int32Ty->parent    = this;
		Int64Ty->parent    = this;
		Float32Ty->parent  = this;
		Float64Ty->parent  = this;
		ObjectTy->parent   = this;
		FunctionTy->parent = this;
		StringTy->parent   = this;
	}

	TypeSpecifier* getPrimitiveTy(PrimitiveType::type t)
	{
		switch(t)
		{
		case PrimitiveType::VOID_TYPE: return VoidTy;
		case PrimitiveType::BOOL_TYPE: return BooleanTy;
		case PrimitiveType::INT8_TYPE: return Int8Ty;
		case PrimitiveType::INT16_TYPE: return Int16Ty;
		case PrimitiveType::INT32_TYPE: return Int32Ty;
		case PrimitiveType::INT64_TYPE: return Int64Ty;
		case PrimitiveType::FLOAT32_TYPE: return Float32Ty;
		case PrimitiveType::FLOAT64_TYPE: return Float64Ty;
		case PrimitiveType::OBJECT_TYPE: return ObjectTy;
		case PrimitiveType::FUNCTION_TYPE: return FunctionTy;
		case PrimitiveType::STRING_TYPE: return StringTy;
		default: UNREACHABLE_CODE(); return NULL;
		}
	}

	void addDanglingObject(ASTNode* object)
	{
		object->parent = this;
		others.push_back(object);
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

    virtual ASTNode* clone() const
    {
    	Internal* internal = new Internal();

    	foreach(i, others)
    		internal->addDanglingObject((*i)->clone());

    	return internal;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & VoidTy;
    	ar & BooleanTy;
    	ar & Int8Ty;
    	ar & Int16Ty;
    	ar & Int32Ty;
    	ar & Int64Ty;
    	ar & Float32Ty;
    	ar & Float64Ty;
    	ar & ObjectTy;
    	ar & FunctionTy;
    	ar & StringTy;
    	ar & others;
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
