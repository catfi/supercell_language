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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/stage/generator/detail/LLVMHelper.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct LLVMGeneratorVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(generateInvoker, generate)

	LLVMGeneratorVisitor(llvm::LLVMContext& context, llvm::Module& current_module) :
		context(context), current_module(current_module), builder(context), helper(context, current_module, builder)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	void generate(ASTNode& node)
	{
		revisit(node);
	}

	void generate(Program& node)
	{
		// create LLVM global variables to store VTT and other global variables in thorscript
		revisit(node);
	}

	void generate(FunctionDecl& node)
	{
		if(!helper.hasFunction(node))
		{
			// create function signature (if necessary) and emit prologue of function
			if(!helper.startFunction(node))
			{
				terminateRevisit();
				return;
			}

			// visit all children
			revisit(node);

			// emit epilogue of function
			if(!helper.finishFunction(node))
			{
				terminateRevisit();
				return;
			}
		}
	}

	void generate(ExpressionStmt& node)
	{

	}

	void generate(IfElseStmt& node)
	{

	}

	void generate(ForeachStmt& node)
	{

	}

	void generate(WhileStmt& node)
	{

	}

	void generate(SwitchStmt& node)
	{

	}

	void generate(UnaryExpr& node)
	{

	}

	void generate(BinaryExpr& node)
	{

	}

	void generate(TernaryExpr& node)
	{

	}

	void generate(CallExpr& node)
	{

	}

	void generate(CastExpr& node)
	{

	}

	void generate(MemberExpr& node)
	{

	}

	llvm::LLVMContext& context;
	llvm::Module& current_module;
	llvm::IRBuilder<> builder;
	LLVMHelper helper;
	llvm::Function* current_function; // current_function would be updated throughout the code generation
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORVISITOR_H_ */
