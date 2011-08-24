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

#ifndef ZILLIANS_LANGUAGE_STAGE_GENERATOR_DETAIL_LLVMHELPER_H_
#define ZILLIANS_LANGUAGE_STAGE_GENERATOR_DETAIL_LLVMHELPER_H_

#include "core/Prerequisite.h"
#include "language/stage/generator/detail/LLVMForeach.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/PassManager.h>
#include <llvm/CallingConv.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>

//using namespace zillians::language::tree;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct LLVMHelper
{
	static bool buildType(llvm::LLVMContext &context, tree::TypeSpecifier& specifier, /*OUT*/ const llvm::Type*& result, /*OUT*/ llvm::Attributes& modifier)
	{
		bool resolved = false;

		switch(specifier.type)
		{
		case tree::TypeSpecifier::ReferredType::CLASS_DECL: // TODO return pointer type
		case tree::TypeSpecifier::ReferredType::INTERFACE_DECL: // TODO return pointer type
		{
			// return generic pointer type, which is an unsigned int32
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt32Ty(context);
			resolved = true; break;
		}
		case tree::TypeSpecifier::ReferredType::FUNCTION_DECL:
		case tree::TypeSpecifier::ReferredType::FUNCTION_TYPE:
		{
			// TODO return pointer to function type
			result = NULL;
			resolved = false; break;
		}
		case tree::TypeSpecifier::ReferredType::ENUM_DECL:
		{
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt32Ty(context);
			resolved = true; break;
		}
		case tree::TypeSpecifier::ReferredType::PRIMITIVE:
		{
			switch(specifier.referred.primitive)
			{
			case tree::TypeSpecifier::PrimitiveType::ANONYMOUS_OBJECT:
			{
				// return generic pointer type, which is an unsigned int32
				modifier |= llvm::Attribute::ZExt;
				result = llvm::IntegerType::getInt32Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::ANONYMOUS_FUNCTION:
			{
				// TODO return pointer to function type
				result = NULL;
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::VOID:
			{
				result = llvm::Type::getVoidTy(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::INT8:
			{
				modifier |= llvm::Attribute::SExt;
				result = llvm::IntegerType::getInt8Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::UINT8:
			{
				modifier |= llvm::Attribute::ZExt;
				result = llvm::IntegerType::getInt8Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::INT16:
			{
				modifier |= llvm::Attribute::SExt;
				result = llvm::IntegerType::getInt16Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::UINT16:
			{
				modifier |= llvm::Attribute::ZExt;
				result = llvm::IntegerType::getInt16Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::INT32:
			{
				modifier |= llvm::Attribute::SExt;
				result = llvm::IntegerType::getInt32Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::UINT32:
			{
				modifier |= llvm::Attribute::ZExt;
				result = llvm::IntegerType::getInt32Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::INT64:
			{
				modifier |= llvm::Attribute::SExt;
				result = llvm::IntegerType::getInt64Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::UINT64:
			{
				modifier |= llvm::Attribute::ZExt;
				result = llvm::IntegerType::getInt64Ty(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::FLOAT32:
			{
				result = llvm::Type::getFloatTy(context);
				resolved = true; break;
			}
			case tree::TypeSpecifier::PrimitiveType::FLOAT64:
			{
				result = llvm::Type::getDoubleTy(context);
				resolved = true; break;
			}
			}
			break;
		}
		case tree::TypeSpecifier::ReferredType::TYPEDEF_DECL:
		case tree::TypeSpecifier::ReferredType::UNSPECIFIED:
			// TODO these are cases that shouldn't happen
			result = NULL;
			resolved = false; break;
		}
		return resolved;
	}

	static bool buildFunctionType(llvm::LLVMContext &context, tree::FunctionDecl& ast_function, /*OUT*/ llvm::FunctionType* llvm_function_type, /*OUT*/ std::vector<llvm::AttributeWithIndex>& llvm_function_parameter_type_attributes, /*OUT*/ llvm::Attributes& llvm_function_return_type_attribute)
	{
		// prepare LLVM function parameter type list
		std::vector<const llvm::Type*> llvm_function_parameter_types;
		{
			int index = 0;
			foreach(i, ast_function.parameters)
			{
				llvm::Attributes attr = llvm::Attribute::None;
				const llvm::Type* t = NULL;

				if(!buildType(context, *i->second, t, attr))
					return false;

				llvm_function_parameter_types.push_back(t);
				llvm_function_parameter_type_attributes.push_back(llvm::AttributeWithIndex::get(index, attr));

				++index;
			}
		}

		// prepare LLVM function return type
		const llvm::Type* llvm_function_return_type = NULL;
		{
			if(!buildType(context, *ast_function.type, llvm_function_return_type, llvm_function_return_type_attribute))
				return false;
		}

		llvm_function_type = llvm::FunctionType::get(llvm_function_return_type, llvm_function_parameter_types, false /*not variadic*/);

		return true;
	}

	static bool buildFunction(llvm::LLVMContext &context, llvm::Module& llvm_module, tree::FunctionDecl& ast_function, /*OUT*/ llvm::Function* llvm_function)
	{
		llvm::FunctionType* llvm_function_type = NULL;
		std::vector<llvm::AttributeWithIndex> llvm_function_parameter_type_attributes;
		llvm::Attributes llvm_function_return_type_attribute;

		// try to resolve function type
		if(!buildFunctionType(context, ast_function, llvm_function_type, llvm_function_parameter_type_attributes, llvm_function_return_type_attribute))
			return false;

		// TODO we should provide some generator name manging helper
		llvm_function = llvm::Function::Create(llvm_function_type, llvm::Function::ExternalLinkage, "test_function", &llvm_module);

		if(!llvm_function)
			return false;

		// set function attributes (modifiers)
		llvm_function->setAttributes(llvm::AttrListPtr::get(llvm_function_parameter_type_attributes.begin(), llvm_function_parameter_type_attributes.end()));

		return true;
	}

private:
	LLVMHelper() { }
};

} } } }

#endif /* LLVMHELPER_H_ */
