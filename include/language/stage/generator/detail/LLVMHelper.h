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

	bool getType(PrimitiveType::type type, /*OUT*/ llvm::Type*& result, /*OUT*/ llvm::Attributes& modifier)
	{
		bool resolved = false;

		switch(type)
		{
		case PrimitiveType::OBJECT_TYPE:
		{
			// return generic object type, which is an signed int32
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::FUNCTION_TYPE:
		{
			// TODO return pointer to function type
			result = NULL;
			resolved = true; break;
		}
		case PrimitiveType::STRING_TYPE:
		{
			// TODO return pointer to function type
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::VOID_TYPE:
		{
			result = llvm::Type::getVoidTy(mContext);
			resolved = true; break;
		}
		case PrimitiveType::BOOL_TYPE:
		{
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt8Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT8_TYPE:
		{
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt8Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT16_TYPE:
		{
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt16Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT32_TYPE:
		{
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::INT64_TYPE:
		{
			//modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt64Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::FLOAT32_TYPE:
		{
			result = llvm::Type::getFloatTy(mContext);
			resolved = true; break;
		}
		case PrimitiveType::FLOAT64_TYPE:
		{
			result = llvm::Type::getDoubleTy(mContext);
			resolved = true; break;
		}
		default:
		{
			result = NULL;
			resolved = false;
			break;
		}
		}

		return resolved;
	}

	bool getType(TypeSpecifier& specifier, /*OUT*/ llvm::Type*& result, /*OUT*/ llvm::Attributes& modifier)
	{
		bool resolved = false;

		switch(specifier.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
		{
			// TODO return pointer to function type
			resolved = getFunctionType(*specifier.referred.function_type, result);
			break;
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
					resolved = true;
				}
				else if(isa<EnumDecl>(resolved_type))
				{
					modifier |= llvm::Attribute::ZExt;
					result = llvm::IntegerType::getInt32Ty(mContext);
					resolved = true;
				}
				else if(isa<TypeSpecifier>(resolved_type))
				{
					resolved = getType(*cast<TypeSpecifier>(resolved_type), result, modifier);
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

	bool getFunctionType(FunctionType& ast_function_type, /*OUT*/ llvm::Type*& llvm_function_type)
	{
		// prepare LLVM function return type
		llvm::Type* llvm_function_return_type = NULL;
		{
			llvm::Attributes attr = llvm::Attribute::None;
			if(!getType(*ast_function_type.return_type, llvm_function_return_type, attr))
				return false;
		}

		// prepare LLVM function parameter type list
		std::vector<llvm::Type*> function_parameter_types;
		{
			foreach(i, ast_function_type.parameter_types)
			{
				llvm::Attributes attr = llvm::Attribute::None;
				llvm::Type* t = NULL;

				if(!getType(**i, t, attr))
					return false;

				function_parameter_types.push_back(t);
			}
		}

		llvm::ArrayRef<llvm::Type*> llvm_function_parameter_types(function_parameter_types);
		llvm_function_type = llvm::FunctionType::get(llvm_function_return_type, llvm_function_parameter_types, false /*not variadic*/)->getPointerTo();

		return true;
	}

	bool getFunctionType(FunctionDecl& ast_function, /*OUT*/ llvm::FunctionType*& llvm_function_type, /*OUT*/ std::vector<llvm::AttributeWithIndex>& llvm_function_type_attributes)
	{
		// prepare LLVM function return type
		llvm::Type* llvm_function_return_type = NULL;
		{
			llvm::Attributes attr = llvm::Attribute::None;
			if(!getType(*ast_function.type, llvm_function_return_type, attr))
				return false;

			if(attr != llvm::Attribute::None)
				llvm_function_type_attributes.push_back(llvm::AttributeWithIndex::get(0, attr));
		}

		// prepare LLVM function parameter type list
		std::vector<llvm::Type*> function_parameter_types;
		{
			int index = 1;
			foreach(i, ast_function.parameters)
			{
				llvm::Attributes attr = llvm::Attribute::None;
				llvm::Type* t = NULL;

				if(!getType(*((*i)->type), t, attr))
					return false;

				function_parameter_types.push_back(t);
				if(attr != llvm::Attribute::None)
					llvm_function_type_attributes.push_back(llvm::AttributeWithIndex::get(index, attr));

				++index;
			}
		}

		llvm::ArrayRef<llvm::Type*> llvm_function_parameter_types(function_parameter_types);
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
