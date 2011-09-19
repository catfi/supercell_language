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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_DETAIL_LLVMHELPER_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_DETAIL_LLVMHELPER_H_

#include "core/Prerequisite.h"
#include "language/stage/generator/detail/LLVMHeaders.h"
#include "language/context/ResolverContext.h"

using namespace zillians::language::tree;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct LLVMHelper
{
	LLVMHelper(llvm::LLVMContext& context) : mContext(context)
	{ }

	bool getType(PrimitiveType::type type, /*OUT*/ const llvm::Type*& result, /*OUT*/ llvm::Attributes& modifier)
	{
		bool resolved = false;

		switch(type)
		{
		case PrimitiveType::ANONYMOUS_OBJECT:
		{
			// return generic pointer type, which is an unsigned int32
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::ANONYMOUS_FUNCTION:
		{
			// TODO return pointer to function type
			result = NULL;
			resolved = true; break;
		}
		case PrimitiveType::VOID:
		{
			result = llvm::Type::getVoidTy(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT8:
		{
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt8Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::UINT8:
		{
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt8Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT16:
		{
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt16Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::UINT16:
		{
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt16Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT32:
		{
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::UINT32:
		{
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT64:
		{
			//modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt64Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::UINT64:
		{
			//modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt64Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::FLOAT32:
		{
			result = llvm::Type::getFloatTy(mContext);
			resolved = true; break;
		}
		case PrimitiveType::FLOAT64:
		{
			result = llvm::Type::getDoubleTy(mContext);
			resolved = true; break;
		}
		}

		return resolved;
	}

	bool getType(TypeSpecifier& specifier, /*OUT*/ const llvm::Type*& result, /*OUT*/ llvm::Attributes& modifier)
	{
		bool resolved = false;

		switch(specifier.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
		{
			// TODO return pointer to function type
			result = NULL;
			resolved = false; break;
		}
		case TypeSpecifier::ReferredType::PRIMITIVE:
		{
			return getType(specifier.referred.primitive, result, modifier);
		}
		case TypeSpecifier::ReferredType::UNSPECIFIED:
		{
			// TODO these are cases that shouldn't happen
			ASTNode* resolved_type = ResolvedType::get(&specifier);
			if(resolved_type)
			{
				if(isa<ClassDecl>(resolved_type) || isa<InterfaceDecl>(resolved_type))
				{
					modifier |= llvm::Attribute::ZExt;
					result = llvm::IntegerType::getInt32Ty(mContext);
					resolved = true; break;
				}
				else if(isa<EnumDecl>(resolved_type))
				{
					modifier |= llvm::Attribute::ZExt;
					result = llvm::IntegerType::getInt32Ty(mContext);
					resolved = true; break;
				}
				else
				{
					BOOST_ASSERT(false && "resolved to unknown type");
					result = NULL;
					resolved = false;
				}
			}
			else
			{
				result = NULL;
				resolved = false;
			}
			break;
		}
		}
		return resolved;
	}

	bool getFunctionType(FunctionDecl& ast_function, /*OUT*/ llvm::FunctionType*& llvm_function_type, /*OUT*/ std::vector<llvm::AttributeWithIndex>& llvm_function_parameter_type_attributes, /*OUT*/ llvm::Attributes& llvm_function_return_type_attribute)
	{
		// prepare LLVM function parameter type list
		std::vector<const llvm::Type*> llvm_function_parameter_types;
		{
			int index = 0;
			foreach(i, ast_function.parameters)
			{
				llvm::Attributes attr = llvm::Attribute::None;
				const llvm::Type* t = NULL;

				if(!getType(*i->get<1>(), t, attr))
					return false;

				llvm_function_parameter_types.push_back(t);
				if(attr != llvm::Attribute::None)
					llvm_function_parameter_type_attributes.push_back(llvm::AttributeWithIndex::get(index, attr));

				++index;
			}
		}

		// prepare LLVM function return type
		const llvm::Type* llvm_function_return_type = NULL;
		{
			if(!getType(*ast_function.type, llvm_function_return_type, llvm_function_return_type_attribute))
				return false;
		}

		llvm_function_type = llvm::FunctionType::get(llvm_function_return_type, llvm_function_parameter_types, false /*not variadic*/);

		return true;
	}

	static llvm::BasicBlock* getPredecessorBlock(llvm::BasicBlock* block, int index)
	{
		int i=0;
		for (llvm::pred_iterator it = llvm::pred_begin(block), it_end = llvm::pred_end(block); it != it_end; ++it, ++i)
		{
			if(i == index)
				return *it;
		}
		return NULL;
	}

private:
	llvm::LLVMContext& mContext;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_DETAIL_LLVMHELPER_H_ */
