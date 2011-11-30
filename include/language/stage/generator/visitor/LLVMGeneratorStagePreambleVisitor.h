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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORPREAMBLESTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORPREAMBLESTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/stage/generator/detail/LLVMHelper.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/stage/generator/context/SynthesizedFunctionContext.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * LLVMGeneratorStagePreambleVisitor is used to generate llvm::Function object for all functions prior to actual code generation
 *
 * We have to generate llvm::Function object prior to actual code generation because the LLVMGeneratorVisitor visits the tree in a top-down approach,
 * so it's common case that the callee is visited after the caller, which requires llvm::Function object to create llvm::CallInst
 *
 * @see LLVMGeneratorStageVisitor
 */
struct LLVMGeneratorStagePreambleVisitor : public GenericDoubleVisitor
{
	CREATE_INVOKER(generateInvoker, generate)

	LLVMGeneratorStagePreambleVisitor(llvm::LLVMContext& context, llvm::Module& module) :
		mContext(context), mModule(module), mBuilder(context), mHelper(context)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	void generate(ASTNode& node)
	{
		revisit(node);
	}

	void generate(InterfaceDecl& node)
	{
		UNUSED_ARGUMENT(node);
		// we don't generate code for interface
	}

	void generate(ClassDecl& node)
	{
		visit(*node.name);

		// we don't generate code for non-fully-specialized classes
		if(isa<TemplatedIdentifier>(node.name))
		{
			// if the class itself is a class template, which has non-specialized version in its templated identifier
			// we don't try to resolve types for class template
			if(!cast<TemplatedIdentifier>(node.name)->isFullySpecialized())
				return;
		}

		revisit(node);
	}

	void generate(FunctionDecl& node)
	{
		if(!GET_SYNTHESIZED_LLVM_FUNCTION(&node))
		{
			llvm::Function* llvm_function = NULL;

			llvm::FunctionType* llvm_function_type = NULL;
			std::vector<llvm::AttributeWithIndex> llvm_function_type_attributes;

			// try to resolve function type
			if(!mHelper.getFunctionType(node, llvm_function_type, llvm_function_type_attributes))
			{
				BOOST_ASSERT(false && "failed to generate LLVM function object");
				terminateRevisit();
				return;
			}

			// TODO we should provide some generator name manging helper
			llvm_function = llvm::Function::Create(llvm_function_type, llvm::Function::ExternalLinkage, NameManglingContext::get(&node)->managled_name, &mModule);

			if(!llvm_function)
			{
				BOOST_ASSERT(false && "failed to generate LLVM function object");
				terminateRevisit();
				return;
			}

			// set function attributes (modifiers)
			if(llvm_function_type_attributes.size() > 0)
				llvm_function->setAttributes(llvm::AttrListPtr::get(llvm_function_type_attributes.begin(), llvm_function_type_attributes.end()));

			// set function parameter names
			int index = 0;
			for(llvm::Function::arg_iterator i = llvm_function->arg_begin(); i != llvm_function->arg_end(); ++i, ++index)
			{
				i->setName(NameManglingContext::get(node.parameters[index])->managled_name);
			}

			// associate the LLVM function object with AST FunctionDecl object
			SET_SYNTHESIZED_LLVM_FUNCTION(&node, llvm_function);
		}
	}

private:
	llvm::LLVMContext &mContext;
	llvm::Module& mModule;
	llvm::IRBuilder<> mBuilder;
	LLVMHelper mHelper;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORPREAMBLESTAGEVISITOR_H_ */
