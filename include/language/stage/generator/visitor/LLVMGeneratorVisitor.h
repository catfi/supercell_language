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

	void generate(VariableDecl& node)
	{
		// here we only generate AllocaInst for AST variables within function
		// those sit in global scope or class member scope will be stored in some other object and access through game object API
		// (all global variables are stored in a single game object, which is assembled by compiler)
		if(isDeclaredInFunction(&node))
		{
			BOOST_ASSERT(!node.is_member);
			llvm::AllocaInst* llvm_alloca_inst = NULL;
			helper.getAlloca(node, llvm_alloca_inst);
		}
	}

	void generate(ExpressionStmt& node)
	{
		revisit(node);
	}

	void generate(IfElseStmt& node)
	{
		// emit the preamble and prepare blocks
	}

	void generate(ForeachStmt& node)
	{
		// emit the preamble and prepare blocks
	}

	void generate(WhileStmt& node)
	{
		// emit the preamble and prepare blocks
		if(node.style == WhileStmt::Style::WHILE)
		{

		}
		else if(node.style == WhileStmt::Style::DO_WHILE)
		{

		}
	}

	void generate(SwitchStmt& node)
	{
		// emit the preamble and prepare blocks
		foreach(i, node.cases)
		{

		}
	}

	void generate(UnaryExpr& node)
	{
		switch(node.opcode)
		{
		case UnaryExpr::OpCode::POSTFIX_INCREMENT:
		case UnaryExpr::OpCode::POSTFIX_DECREMENT:
		case UnaryExpr::OpCode::PREFIX_INCREMENT:
		case UnaryExpr::OpCode::PREFIX_DECREMENT:
		case UnaryExpr::OpCode::BINARY_NOT:
		case UnaryExpr::OpCode::LOGICAL_NOT:
		case UnaryExpr::OpCode::ARITHMETIC_NEGATE:
		case UnaryExpr::OpCode::NEW:
			break;
		}
	}

	void generate(BinaryExpr& node)
	{
		if(node.isArithmetic())
		{

		}
		else if(node.isAssignment())
		{

		}
		else if(node.isBinary())
		{

		}
		else if(node.isLogical())
		{

		}
		else if(node.isComparison())
		{

		}
		else
		{
			if(node.opcode == BinaryExpr::OpCode::INSTANCEOF)
			{

			}
			else if(node.opcode == BinaryExpr::OpCode::ARRAY_SUBSCRIPT)
			{

			}
			else
			{
				BOOST_ASSERT(false && "unrecognized binary operator encountered at LLVM code generation");
			}
		}
	}

	void generate(TernaryExpr& node)
	{
		// TODO use predicate
	}

	void generate(CallExpr& node)
	{
		// TODO depending on the LHS, if it's a directly-invokable function, just get the function prototype and invoke
		// TODO if it's not a directly-invokable function, which can be a class member function, pass this pointer to that function and make the call
		// TODO if it's a class member function and it's virtual, we have different calling convention here
	}

	void generate(CastExpr& node)
	{
		// TODO only generate code when there's type mismatch
	}

	void generate(MemberExpr& node)
	{
		// TODO if the node is resolved to a package, do nothing
		// TODO otherwise, the node is resolved to a local variable, if the RHS is a member variable, use object resolver to resolve correct offset
		// TODO if RHS is a member function, use object resolver to resolve correct function pointer (if it's a non-virtual function)
		// TODO if it's virtual function, resolve to correct table index using VTable
	}

private:
	bool isDeclaredInFunction(ASTNode* node)
	{
		ASTNode* current = node;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return true;
			current = current->parent;
		}

		return false;
	}

	llvm::LLVMContext& context;
	llvm::Module& current_module;
	llvm::IRBuilder<> builder;
	LLVMHelper helper;
	llvm::Function* current_function; // current_function would be updated throughout the code generation
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORVISITOR_H_ */
