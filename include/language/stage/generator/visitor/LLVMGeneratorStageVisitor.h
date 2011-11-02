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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/tree/visitor/NodeInfoVisitor.h"
#include "language/stage/generator/detail/LLVMForeach.h"
#include "language/stage/generator/detail/LLVMHelper.h"
#include "language/context/ResolverContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/stage/generator/context/SynthesizedValueContext.h"
#include "language/stage/generator/context/SynthesizedFunctionContext.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NodeInfoVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * LLVMGeneratorVisitor walks the AST tree and generates LLVM instructions
 *
 * As LLVMGeneratorVisitor traverse the AST tree only once, required information must be prepared prior to the visitor,
 * including any necessary transformation.
 *
 * @see LLVMGeneratorPreambleVisitor
 */
struct LLVMGeneratorStageVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(generateInvoker, generate)

	LLVMGeneratorStageVisitor(llvm::LLVMContext& context, llvm::Module& module) :
		mContext(context), mModule(module), mBuilder(context), mHelper(context)
	{
		mFunctionContext.function = NULL;
		mFunctionContext.entry_block = NULL;
		mFunctionContext.return_block = NULL;
		mFunctionContext.alloca_insert_point = NULL;
		mFunctionContext.mask_insertion = false;

		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	void generate(ASTNode& node)
	{
		revisit(node);
	}

	void generate(Block& node)
	{
		revisit(node);
	}

	void generate(NumericLiteral& node)
	{
		if(hasValue(node)) return;

		revisit(node);

		llvm::Value* result = NULL;
		switch(node.type)
		{
		case PrimitiveType::BOOL:
			result = llvm::ConstantInt::get(llvm::IntegerType::getInt1Ty(mContext), node.value.b, false); break;
		case PrimitiveType::INT8:
			result = llvm::ConstantInt::get(llvm::IntegerType::getInt8Ty(mContext), node.value.i8, false); break;
		case PrimitiveType::INT16:
			result = llvm::ConstantInt::get(llvm::IntegerType::getInt16Ty(mContext), node.value.i16, false); break;
		case PrimitiveType::INT32:
			result = llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(mContext), node.value.i32, false); break;
		case PrimitiveType::INT64:
			result = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(mContext), node.value.i64, false); break;
		case PrimitiveType::FLOAT32:
			result = llvm::ConstantFP::get(llvm::Type::getFloatTy(mContext), node.value.f32); break;
		case PrimitiveType::FLOAT64:
			result = llvm::ConstantFP::get(llvm::Type::getDoubleTy(mContext), node.value.f64); break;
		default:
			break;
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		SET_SYNTHESIZED_LLVM_VALUE(&node, result);
	}

	void generate(ObjectLiteral& node)
	{
		if(hasValue(node)) return;

		revisit(node);
	}

	void generate(StringLiteral& node)
	{
		if(hasValue(node)) return;

		revisit(node);
	}

	void generate(Program& node)
	{
		// create LLVM global variables to store VTT and other global variables in thorscript
		revisit(node);
	}

	void generate(FunctionDecl& node)
	{
		if(isFunctionVisited(node))
			return;

		// create function signature (if necessary) and emit prologue of function
		if(!startFunction(node))
		{
			terminateRevisit();
			return;
		}

		// create alloca for all parameters
		if(!allocateParameters(node))
		{
			terminateRevisit();
			return;
		}

		// visit all children
		revisit(node);

		// emit epilogue of function
		if(!finishFunction(node))
		{
			terminateRevisit();
			return;
		}
	}

	void generate(VariableDecl& node)
	{
		//if(ASTNodeHelper::hasDirectOwnerPackage(node))
		if(node.parent && isa<Package>(node.parent))
			return;

		revisit(node);

		// here we only generate AllocaInst for AST variables within function
		// those sit in global scope or class member scope will be stored in some other object and access through game object API
		// (all global variables are stored in a single game object, which is assembled by compiler)
		if(ASTNodeHelper::hasOwner<FunctionDecl>(&node))
		{
			if(hasValue(node)) return;
			if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

			createAlloca(node);

			if(node.initializer)
			{
				llvm::Value* llvm_alloca_inst = GET_SYNTHESIZED_LLVM_VALUE(&node);
				llvm::Value* llvm_init = GET_SYNTHESIZED_LLVM_VALUE(node.initializer);

				if(llvm_init)
				{
//					llvm::cast<llvm::AllocaInst>(llvm_alloca_inst)->getAllocatedType()->dump();
//					llvm::cast<llvm::PointerType>(llvm_alloca_inst->getType())->getElementType()->dump();
//					llvm_init->getType()->dump();
					BOOST_ASSERT(llvm::cast<llvm::AllocaInst>(llvm_alloca_inst)->getAllocatedType() == llvm_init->getType());
					BOOST_ASSERT(llvm::cast<llvm::PointerType>(llvm_alloca_inst->getType())->getElementType() == llvm_init->getType());
					llvm::StoreInst* store_inst = mBuilder.CreateStore(llvm_init, llvm_alloca_inst);
					SET_SYNTHESIZED_LLVM_VALUE(&node, store_inst);
				}
			}
		}
	}

	void generate(DeclarativeStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		propagate(&node, node.declaration);
	}

	void generate(BranchStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		llvm::Value* result = NULL;

		switch(node.opcode)
		{
		case BranchStmt::OpCode::BREAK:
			break;
		case BranchStmt::OpCode::CONTINUE:
			break;
		case BranchStmt::OpCode::RETURN:
		{
			if(node.result)
			{
				llvm::Value* return_value = GET_SYNTHESIZED_LLVM_VALUE(node.result);
				BOOST_ASSERT(return_value && "invalid LLVM value for return instruction");
				mBuilder.CreateStore(return_value, mFunctionContext.return_value);
				result = mBuilder.CreateBr(mFunctionContext.return_block);
			}
			else
			{
				result = mBuilder.CreateBr(mFunctionContext.return_block);
			}
			setBlockInsertionMask();
			break;
		}
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		SET_SYNTHESIZED_LLVM_VALUE(&node, result);
	}

	void generate(ExpressionStmt& node)
	{
		if(hasValue(node)) return;

		revisit(node);

		propagate(&node, node.expr);
	}

	void generate(IfElseStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		// TODO simplify the blocks by removing or merging unnecessary blocks
		// generate code into blocks
		std::vector<llvm::BasicBlock*> llvm_blocks;
		{
			// for if branch
			{
				llvm::BasicBlock* cond = createBasicBlock("if.eval", mFunctionContext.function);
				// jump from current block to the condition evaluation block
				enterBasicBlock(cond, true);
				visit(*node.if_branch.cond);

				llvm::BasicBlock* block = createBasicBlock("if.then", mFunctionContext.function);
				// because we create linkage among blocks on our own in this if/else structure, so we don't emit branch instruction automatically
				enterBasicBlock(block, false);
				visit(*node.if_branch.block);

				llvm_blocks.push_back(cond);
				llvm_blocks.push_back(block);
			}

			// for elif branch
			{
				foreach(i, node.elseif_branches)
				{
					llvm::BasicBlock* cond = createBasicBlock("elif.eval", mFunctionContext.function);
					enterBasicBlock(cond, false);
					visit(*i->cond);

					llvm::BasicBlock* block = createBasicBlock("elif.then", mFunctionContext.function);
					enterBasicBlock(block, false);
					visit(*i->block);

					llvm_blocks.push_back(cond);
					llvm_blocks.push_back(block);
				}
			}

			// for else branch
			if(node.else_block)
			{
				llvm::BasicBlock* block = createBasicBlock("else.then", mFunctionContext.function);
				enterBasicBlock(block, false);
				visit(*node.else_block);
				llvm_blocks.push_back(block);
			}

			// for the last block
			{
				llvm::BasicBlock* block = createBasicBlock("if.finalized", mFunctionContext.function);
				llvm_blocks.push_back(block);
			}
		}

		// create linkage among blocks
		{
			// for if branch
			{
				llvm::Value* llvm_if_cond = GET_SYNTHESIZED_LLVM_VALUE(node.if_branch.cond);

				if(!isBlockTerminated(llvm_blocks[0]))
				{
					mBuilder.SetInsertPoint(llvm_blocks[0]); // [0] is the 'condition evaluation block for if branch'
					mBuilder.CreateCondBr(llvm_if_cond, llvm_blocks[1], llvm_blocks[2]); // [1] is the 'then block for if branch', [2] could be the 'condition evaluation block for first elif branch', or the 'else block', or the 'end block'
				}

				if(!isBlockTerminated(llvm_blocks[1]))
				{
					mBuilder.SetInsertPoint(llvm_blocks[1]); // [1] is the 'then block for if branch'
					mBuilder.CreateBr(llvm_blocks.back()); // [back()] is the 'end block'
				}
			}

			// for all elif branch
			int index = 2;
			{
				foreach(i, node.elseif_branches)
				{
					llvm::Value* llvm_elif_cond = GET_SYNTHESIZED_LLVM_VALUE(i->cond);

					if(!isBlockTerminated(llvm_blocks[index]))
					{
						mBuilder.SetInsertPoint(llvm_blocks[index]);
						mBuilder.CreateCondBr(llvm_elif_cond, llvm_blocks[index+1], llvm_blocks[index+2]);
					}

					if(!isBlockTerminated(llvm_blocks[index+1]))
					{
						mBuilder.SetInsertPoint(llvm_blocks[index+1]);
						mBuilder.CreateBr(llvm_blocks.back());
					}

					index += 2;
				}
			}

			// for else branch
			if(node.else_block)
			{
				if(!isBlockTerminated(llvm_blocks[index]))
				{
					mBuilder.SetInsertPoint(llvm_blocks[index]);
					mBuilder.CreateBr(llvm_blocks.back());
				}
				index += 1;
			}

			// for the last block
			{
				enterBasicBlock(llvm_blocks.back(), false);
				index += 1;
			}

			BOOST_ASSERT(index == (int)llvm_blocks.size());
		}
	}

	void generate(ForStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		llvm::BasicBlock* init_block = createBasicBlock("for.init", mFunctionContext.function);
		llvm::BasicBlock* cond_block = createBasicBlock("for.cond", mFunctionContext.function);
		llvm::BasicBlock* step_block = createBasicBlock("for.step", mFunctionContext.function);
		llvm::BasicBlock* action_block = createBasicBlock("for.action", mFunctionContext.function);
		llvm::BasicBlock* finalized_block = createBasicBlock("for.finalized", mFunctionContext.function);

		// jump from current block to the condition evaluation block
		enterBasicBlock(init_block);
		if(node.init) visit(*node.init);

		enterBasicBlock(cond_block);
		visit(*node.cond);

		// conditional branch to either action block or finalized block
		llvm::Value* llvm_cond = GET_SYNTHESIZED_LLVM_VALUE(node.cond);
		mBuilder.CreateCondBr(llvm_cond, action_block, finalized_block);

		enterBasicBlock(action_block);
		if(node.block) visit(*node.block);

		enterBasicBlock(step_block);
		if(node.step) visit(*node.step);
		mBuilder.CreateBr(cond_block);

		// enter finalized block
		enterBasicBlock(finalized_block);
	}

	void generate(ForeachStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

//		if(node.if_branch.cond) user_visitor->
//		if(node.if_branch.block) user_visitor->visit(*node.if_branch.block);
//		foreach(i, node.elseif_branches)
//		{
//			if(i->cond) user_visitor->visit(*i->cond);
//			if(i->block) user_visitor->visit(*i->block);
//		}
//		if(node.else_block) user_visitor->visit(*node.else_block);
//
//		if(node.annotations) user_visitor->visit(*node.annotations);

		// emit the preamble and prepare blocks
	}

	void generate(WhileStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		// emit the preamble and prepare blocks
		if(node.style == WhileStmt::Style::WHILE)
		{
			llvm::BasicBlock* cond_block = createBasicBlock("while.eval", mFunctionContext.function);
			llvm::BasicBlock* action_block = createBasicBlock("while.action", mFunctionContext.function);
			llvm::BasicBlock* finalized_block = createBasicBlock("while.finalized", mFunctionContext.function);

			// jump from current block to the condition evaluation block
			enterBasicBlock(cond_block);
			visit(*node.cond);

			// conditional branch to either action block or finalized block
			llvm::Value* llvm_cond = GET_SYNTHESIZED_LLVM_VALUE(node.cond);
			mBuilder.CreateCondBr(llvm_cond, action_block, finalized_block);

			enterBasicBlock(action_block);
			if(node.block) visit(*node.block);
			mBuilder.CreateBr(cond_block);

			// enter finalized block
			enterBasicBlock(finalized_block);
		}
		else if(node.style == WhileStmt::Style::DO_WHILE)
		{
			llvm::BasicBlock* action_block = createBasicBlock("while.action", mFunctionContext.function);
			llvm::BasicBlock* cond_block = createBasicBlock("while.eval", mFunctionContext.function);
			llvm::BasicBlock* finalized_block = createBasicBlock("while.finalized", mFunctionContext.function);

			// jump from current block to the condition evaluation block
			enterBasicBlock(action_block);
			if(node.block) visit(*node.block);

			// conditional branch to either action block or finalized block
			enterBasicBlock(cond_block);
			visit(*node.cond);

			llvm::Value* llvm_cond = GET_SYNTHESIZED_LLVM_VALUE(node.cond);
			mBuilder.CreateCondBr(llvm_cond, action_block, finalized_block);

			// enter finalized block
			enterBasicBlock(finalized_block);
		}
	}

	void generate(SwitchStmt& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);
		// emit the preamble and prepare blocks
	}

	void generate(PrimaryExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		switch(node.catagory)
		{
		case PrimaryExpr::Catagory::IDENTIFIER:
		{
			propagate(&node, ResolvedSymbol::get(&node)); break;
		}
		case PrimaryExpr::Catagory::LITERAL:
			propagate(&node, node.value.literal); break;
		case PrimaryExpr::Catagory::LAMBDA:
			break;
		}
	}

	void generate(UnaryExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		llvm::Value* result = NULL;

		ASTNode* operand_resolved;
		llvm::Value* operand_value_for_read;
		llvm::Value* operand_value_for_write;
		if(!getValue(*node.node, operand_resolved, operand_value_for_read, operand_value_for_write, true, (node.opcode >= UnaryExpr::OpCode::POSTFIX_INCREMENT && node.opcode <= UnaryExpr::OpCode::PREFIX_DECREMENT)))
		{
			BOOST_ASSERT(false && "failed to resolve LLVM value for operand");
			terminateRevisit();
		}

		// TODO handle the differences between postfix and prefix increment/decrement
		switch(node.opcode)
		{
		case UnaryExpr::OpCode::POSTFIX_INCREMENT:
			result = mBuilder.CreateAdd(operand_value_for_read, llvm::ConstantInt::get(operand_value_for_read->getType(), 1, false));
			mBuilder.CreateStore(result, operand_value_for_write);
			break;
		case UnaryExpr::OpCode::POSTFIX_DECREMENT:
			result = mBuilder.CreateSub(operand_value_for_read, llvm::ConstantInt::get(operand_value_for_read->getType(), 1, false));
			mBuilder.CreateStore(result, operand_value_for_write);
			break;
		case UnaryExpr::OpCode::PREFIX_INCREMENT:
			result = mBuilder.CreateAdd(operand_value_for_read, llvm::ConstantInt::get(operand_value_for_read->getType(), 1, false));
			mBuilder.CreateStore(result, operand_value_for_write);
			break;
		case UnaryExpr::OpCode::PREFIX_DECREMENT:
			result = mBuilder.CreateSub(operand_value_for_read, llvm::ConstantInt::get(operand_value_for_read->getType(), 1, false));
			mBuilder.CreateStore(result, operand_value_for_write);
			break;
		case UnaryExpr::OpCode::BINARY_NOT:
			result = mBuilder.CreateNot(operand_value_for_read); break;
		case UnaryExpr::OpCode::LOGICAL_NOT:
			BOOST_ASSERT(false && "not yet implemented");
			break;
		case UnaryExpr::OpCode::ARITHMETIC_NEGATE:
			result = (isFloatType(node)) ? mBuilder.CreateFNeg(operand_value_for_read) : mBuilder.CreateNeg(operand_value_for_read); break;
		case UnaryExpr::OpCode::NEW:
			break;
		case UnaryExpr::OpCode::NOOP:
			result = operand_value_for_read;
			break;
		default:
			result = NULL;
			break;
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		SET_SYNTHESIZED_LLVM_VALUE(&node, result);
	}

	void generate(BinaryExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		ASTNode* lhs_resolved;
		llvm::Value* lhs_value_for_read;
		llvm::Value* lhs_value_for_write;
		bool is_assignment = node.isAssignment();
		if(!getValue(*node.left, lhs_resolved, lhs_value_for_read, lhs_value_for_write, !is_assignment, is_assignment))
		{
			BOOST_ASSERT(false && "failed to resolve LLVM value for LHS");
			terminateRevisit();
		}

		ASTNode* rhs_resolved;
		llvm::Value* rhs_value_for_read;
		llvm::Value* rhs_value_for_write;
		if(!getValue(*node.right, rhs_resolved, rhs_value_for_read, rhs_value_for_write, true, false))
		{
			BOOST_ASSERT(false && "failed to resolve LLVM value for RHS");
			terminateRevisit();
		}

		llvm::Value* result = NULL;
//		llvm::Value* temporary = NULL;

		switch(node.opcode)
		{
			// Assignments Operations
		case BinaryExpr::OpCode::ASSIGN:
			result = mBuilder.CreateStore(rhs_value_for_read, lhs_value_for_write);
			break;
//		case BinaryExpr::OpCode::ADD_ASSIGN:
//			temporary = (isFloatType(*node.left)) ? mBuilder.CreateFAdd(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateAdd(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::SUB_ASSIGN:
//			temporary = (isFloatType(*node.left)) ? mBuilder.CreateFSub(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateSub(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::MUL_ASSIGN:
//			temporary = (isFloatType(*node.left)) ? mBuilder.CreateFMul(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateMul(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::DIV_ASSIGN:
//			temporary = (isFloatType(*node.left)) ? mBuilder.CreateFDiv(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateUDiv(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::MOD_ASSIGN:
//			temporary = (isFloatType(*node.left)) ? mBuilder.CreateFRem(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateURem(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::AND_ASSIGN:
//			temporary = mBuilder.CreateAnd(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::OR_ASSIGN:
//			temporary = mBuilder.CreateOr(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;
//		case BinaryExpr::OpCode::XOR_ASSIGN:
//			temporary = mBuilder.CreateXor(lhs_value_for_read, rhs_value_for_read);
//			result = mBuilder.CreateStore(temporary, lhs_value_for_write);
//			break;

			// Arithmetic Operations
		case BinaryExpr::OpCode::ARITHMETIC_ADD:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFAdd(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateAdd(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::ARITHMETIC_SUB:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFSub(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateSub(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::ARITHMETIC_MUL:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFMul(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateMul(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::ARITHMETIC_DIV:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFDiv(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateSDiv(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::ARITHMETIC_MOD:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFRem(lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateSRem(lhs_value_for_read, rhs_value_for_read); break;

			// Arithmetic Bitwise Operations
		case BinaryExpr::OpCode::BINARY_AND:
			result = mBuilder.CreateAnd(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::BINARY_OR:
			result = mBuilder.CreateOr(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::BINARY_XOR:
			result = mBuilder.CreateXor(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::BINARY_LSHIFT:
			BOOST_ASSERT(false && "not yet implemented");
			// TODO what's the left shift operation?
			break;
		case BinaryExpr::OpCode::BINARY_RSHIFT:
			// TODO should we use CreateAShr or CreateLShr? depending on the object type?
			result = mBuilder.CreateAShr(lhs_value_for_read, rhs_value_for_read); break;

			// Logic Operations
		case BinaryExpr::OpCode::LOGICAL_AND:
			result = mBuilder.CreateAnd(lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::LOGICAL_OR:
			result = mBuilder.CreateOr(lhs_value_for_read, rhs_value_for_read); break;

			// Instance Of (Type testing)
		case BinaryExpr::OpCode::INSTANCEOF:
			BOOST_ASSERT(false && "not yet implemented");
			break;

			// Logical Comparison
		case BinaryExpr::OpCode::COMPARE_EQ:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_UEQ, lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::COMPARE_NE:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_UNE, lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::COMPARE_GT:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_UGT, lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_UGT, lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::COMPARE_LT:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_ULT, lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULT, lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::COMPARE_GE:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_UGE, lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_UGE, lhs_value_for_read, rhs_value_for_read); break;
		case BinaryExpr::OpCode::COMPARE_LE:
			result = (isFloatType(*node.left)) ? mBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_ULE, lhs_value_for_read, rhs_value_for_read) : mBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULE, lhs_value_for_read, rhs_value_for_read); break;

			// Range Operator
		case BinaryExpr::OpCode::RANGE_ELLIPSIS:
			BOOST_ASSERT(false && "not yet implemented");
			break;

			// Array Subscript
		case BinaryExpr::OpCode::ARRAY_SUBSCRIPT:
			BOOST_ASSERT(false && "not yet implemented");
			break;

		default:
			BOOST_ASSERT(false && "not yet implemented");
			result = NULL; break;
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		SET_SYNTHESIZED_LLVM_VALUE(&node, result);
	}

	void generate(TernaryExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		llvm::BasicBlock* cond_block = createBasicBlock("ternary.eval", mFunctionContext.function);
		llvm::BasicBlock* true_block = createBasicBlock("ternary.true", mFunctionContext.function);
		llvm::BasicBlock* false_block = createBasicBlock("ternary.false", mFunctionContext.function);
		llvm::BasicBlock* final_block = createBasicBlock("ternary.finalized", mFunctionContext.function);

		// jump from current block to the condition evaluation block
		enterBasicBlock(cond_block);
		{
			visit(*node.cond);

			// conditional branch to either action block or finalized block
			llvm::Value* llvm_cond = GET_SYNTHESIZED_LLVM_VALUE(node.cond);
			mBuilder.CreateCondBr(llvm_cond, true_block, false_block);
		}

		// try entering true block and get the value
		enterBasicBlock(true_block);
		if(node.true_node)
		{
			visit(*node.true_node);
			mBuilder.CreateBr(final_block);
		}

		// try entering true block and get the value
		enterBasicBlock(false_block);
		if(node.false_node)
		{
			visit(*node.false_node);
			mBuilder.CreateBr(final_block);
		}

		// try enter finalized block and create branch from true block
		enterBasicBlock(final_block);
		{
			llvm::Value* true_value = GET_SYNTHESIZED_LLVM_VALUE(node.true_node);
			llvm::Value* false_value = GET_SYNTHESIZED_LLVM_VALUE(node.false_node);
			if(node.isRValue())
			{
				if(true_value->getType()->isPointerTy())
					true_value = mBuilder.CreateLoad(true_value);

				if(false_value->getType()->isPointerTy())
					false_value = mBuilder.CreateLoad(false_value);
			}

			llvm::PHINode* phi = mBuilder.CreatePHI(true_value->getType());
			phi->addIncoming(true_value, true_block);
			phi->addIncoming(false_value, false_block);

			SET_SYNTHESIZED_LLVM_VALUE(&node, phi);
		}
	}

	void generate(CallExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		std::vector<llvm::Value*> arguments;
		foreach(i, node.parameters)
		{
			ASTNode* parameter_resolved;
			llvm::Value* parameter_value_for_read;
			llvm::Value* parameter_value_for_write;
			if(!getValue(**i, parameter_resolved, parameter_value_for_read, parameter_value_for_write, true, false))
			{
				BOOST_ASSERT(false && "invalid LLVM parameter value for function call");
				terminateRevisit();
			}

			arguments.push_back(parameter_value_for_read);
		}

		ASTNode* resolved = ResolvedSymbol::get(node.node);
		if(isa<FunctionDecl>(resolved))
		{
			llvm::Function* llvm_function = GET_SYNTHESIZED_LLVM_FUNCTION(resolved);
			if(llvm_function)
			{
				llvm::Value* result = mBuilder.CreateCall(llvm_function, arguments.begin(), arguments.end());
				SET_SYNTHESIZED_LLVM_VALUE(&node, result);
			}
			else
			{
				BOOST_ASSERT(false && "invalid LLVM function object");
			}
		}
		else
		{
			BOOST_ASSERT(false && "calling non-invokable value");
		}
		// TODO depending on the LHS, if it's a directly-invokable function, just get the function prototype and invoke
		// TODO if it's not a directly-invokable function, which can be a class member function, pass this pointer to that function and make the call
		// TODO if it's a class member function and it's virtual, we have different calling convention here
	}

	void generate(CastExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);

		ASTNode* node_resolved = NULL;
		llvm::Value* llvm_result = NULL;
		llvm::Value* llvm_value_for_read = NULL;
		llvm::Value* llvm_value_for_write = NULL;
		if(!getValue(*node.node, node_resolved, llvm_value_for_read, llvm_value_for_write, true, false))
		{
			BOOST_ASSERT(false && "invalid LLVM parameter value for type conversion");
			terminateRevisit();
		}

		if(node.type->type == TypeSpecifier::ReferredType::PRIMITIVE)
		{
			TypeSpecifier* node_specifier = cast<TypeSpecifier>(ResolvedType::get(node.node));
			if(node_specifier && node_specifier->type == TypeSpecifier::ReferredType::PRIMITIVE)
			{
				bool need_ext = (PrimitiveType::bitSize(node.type->referred.primitive) > PrimitiveType::bitSize(node_specifier->referred.primitive));
				bool need_truc = (PrimitiveType::bitSize(node.type->referred.primitive) < PrimitiveType::bitSize(node_specifier->referred.primitive));
				bool bitsize_mismatched = (PrimitiveType::bitSize(node.type->referred.primitive) != PrimitiveType::bitSize(node_specifier->referred.primitive));

				UNUSED_ARGUMENT(bitsize_mismatched);

				const llvm::Type* llvm_cast_type = NULL;

				llvm::Attributes llvm_dummy_modifier = llvm::Attribute::None; // dummy
				mHelper.getType(*node.type, llvm_cast_type, llvm_dummy_modifier);

				if(PrimitiveType::isIntegerType(node.type->referred.primitive) && PrimitiveType::isIntegerType(node_specifier->referred.primitive))
				{
					// casting from integer to integer
					if(need_truc)
					{
						llvm_result = mBuilder.CreateTrunc(llvm_value_for_read, llvm_cast_type);
					}
					else if(need_ext)
					{
						llvm_result = mBuilder.CreateSExt(llvm_value_for_read, llvm_cast_type);
					}
					else
					{
						// the source the destination are all integer with same size, no-op
					}
				}
				else if(PrimitiveType::isIntegerType(node.type->referred.primitive) && PrimitiveType::isFloatType(node_specifier->referred.primitive))
				{
					// casting from float to integer
					llvm_result = mBuilder.CreateFPToSI(llvm_value_for_read, llvm_cast_type);
				}
				else if(PrimitiveType::isFloatType(node.type->referred.primitive) && PrimitiveType::isIntegerType(node_specifier->referred.primitive))
				{
					// casting from integer to float
					llvm_result = mBuilder.CreateSIToFP(llvm_value_for_read, llvm_cast_type);
				}
				else if(PrimitiveType::isFloatType(node.type->referred.primitive) && PrimitiveType::isFloatType(node_specifier->referred.primitive))
				{
					// casting from float to float
					if(need_truc)
					{
						llvm_result = mBuilder.CreateFPTrunc(llvm_value_for_read, llvm_cast_type);
					}
					else if(need_ext)
					{
						llvm_result = mBuilder.CreateFPExt(llvm_value_for_read, llvm_cast_type);
					}
					else
					{
						// the source the destination are all float with same size, no-op
					}
				}
				else
				{
					// the rest of types can be both object types or function types or void type, no casting required
					if(	(node.type->referred.primitive == PrimitiveType::VOID && node_specifier->referred.primitive == PrimitiveType::VOID) ||
						(node.type->referred.primitive == PrimitiveType::OBJECT && node_specifier->referred.primitive == PrimitiveType::OBJECT) ||
						(node.type->referred.primitive == PrimitiveType::FUNCTION && node_specifier->referred.primitive == PrimitiveType::FUNCTION) ||
						(node.type->referred.primitive == PrimitiveType::STRING && node_specifier->referred.primitive == PrimitiveType::STRING) )
					{
						llvm_result = llvm_value_for_read;
					}
					else
					{
						BOOST_ASSERT(false && "invalid primitive type conversion for LLVM generator stage");
					}
				}
			}
		}
		else
		{
			// for function type or unspecified type, they should be all compatible anyway, so no conversion is necessary
			// TODO confirm this
			llvm_result = llvm_value_for_read;
		}

		if(llvm_result)
		{
			SET_SYNTHESIZED_LLVM_VALUE(&node, llvm_result);
		}
		else
		{
			BOOST_ASSERT(false && "invalid AST for LLVM generator stage");
		}
	}

	void generate(MemberExpr& node)
	{
		if(hasValue(node)) return;
		if(isBlockInsertionMasked() || isBlockTerminated(currentBlock()))	return;

		revisit(node);
		// TODO if the node is resolved to a package, do nothing
		// TODO otherwise, the node is resolved to a local variable, if the RHS is a member variable, use object resolver to resolve correct offset
		// TODO if RHS is a member function, use object resolver to resolve correct function pointer (if it's a non-virtual function)
		// TODO if it's virtual function, resolve to correct table index using VTable
	}

private:
	llvm::BasicBlock* createBasicBlock(llvm::StringRef name = "", llvm::Function* parent = NULL, llvm::BasicBlock* before = NULL)
	{
		return llvm::BasicBlock::Create(mContext, name, parent, before);
	}

	void enterBasicBlock(llvm::BasicBlock* block, bool emit_branch_if_necessary = true)
	{
		if(emit_branch_if_necessary && !mBuilder.GetInsertBlock()->getTerminator())
		{
			mBuilder.CreateBr(block);
		}

		mBuilder.SetInsertPoint(block);

		resetBlockInsertionMask();
	}

	llvm::AllocaInst* createAlloca(const llvm::Type* type, const llvm::Twine& name = "")
	{
		llvm::AllocaInst* llvm_alloca_inst = NULL;
		if(mBuilder.isNamePreserving())
			llvm_alloca_inst = new llvm::AllocaInst(type, 0, name, mFunctionContext.alloca_insert_point);
		else
			llvm_alloca_inst = new llvm::AllocaInst(type, 0, "", mFunctionContext.alloca_insert_point);

		return llvm_alloca_inst;
	}

	bool createAlloca(VariableDecl& ast_variable)
	{
		if(GET_SYNTHESIZED_LLVM_VALUE(&ast_variable))
			return true;

		const llvm::Type* llvm_variable_type = NULL;
		llvm::Attributes llvm_variable_modifier = llvm::Attribute::None;
		if(!mHelper.getType(*ast_variable.type, llvm_variable_type, llvm_variable_modifier))
			return false;

//		llvm_variable_type->dump();

		llvm::AllocaInst* llvm_alloca_inst = NULL;
		if(mBuilder.isNamePreserving())
			llvm_alloca_inst = new llvm::AllocaInst(llvm_variable_type, 0, NameManglingContext::get(&ast_variable)->managled_name, mFunctionContext.alloca_insert_point);
		else
			llvm_alloca_inst = new llvm::AllocaInst(llvm_variable_type, 0, "", mFunctionContext.alloca_insert_point);

		SET_SYNTHESIZED_LLVM_VALUE(&ast_variable, llvm_alloca_inst);

		return true;
	}

	bool startFunction(FunctionDecl& ast_function)
	{
		// TODO simplify the blocks by removing or merging unnecessary blocks
		BOOST_ASSERT(!mFunctionContext.function);
		BOOST_ASSERT(!mFunctionContext.entry_block);
		BOOST_ASSERT(!mFunctionContext.return_block);
		BOOST_ASSERT(!mFunctionContext.alloca_insert_point);

		llvm::Function* llvm_function = GET_SYNTHESIZED_LLVM_FUNCTION(&ast_function);
		if(!llvm_function)
		{
			BOOST_ASSERT(false && "invalid LLVM function object");
			terminateRevisit();
			return false;
		}

		// create basic entry blocks
		mFunctionContext.entry_block = createBasicBlock("entry", llvm_function);
		mBuilder.SetInsertPoint(mFunctionContext.entry_block);

		// create dummy alloca insertion point (from clang & foster)
		{
			llvm::Value *undef = llvm::UndefValue::get(llvm::Type::getInt32Ty(mContext));
			mFunctionContext.alloca_insert_point = new llvm::BitCastInst(undef, llvm::Type::getInt32Ty(mContext), "", mFunctionContext.entry_block);
			if(mBuilder.isNamePreserving())
				mFunctionContext.alloca_insert_point->setName("allocapt");
		}

		// create return block
		mFunctionContext.return_block = createBasicBlock("return", llvm_function);

		// allocate return value if necessary
		{
			const llvm::Type* type;
			llvm::Attributes modifier;
			if(mHelper.getType(*ast_function.type, type, modifier) && type && !type->isVoidTy())
			{
				mFunctionContext.return_value = new llvm::AllocaInst(type, 0, "retval", mFunctionContext.alloca_insert_point);
			}
			else
			{
				mFunctionContext.return_value = NULL;
			}
		}

		// set current function
		mFunctionContext.function = llvm_function;

		return true;
	}

	bool allocateParameters(FunctionDecl& ast_function)
	{
		llvm::Function* llvm_function = GET_SYNTHESIZED_LLVM_FUNCTION(&ast_function);
		if(!llvm_function)
		{
			BOOST_ASSERT(false && "invalid LLVM function object");
			terminateRevisit();
			return false;
		}

		int index = 0;
		for(llvm::Function::arg_iterator it_arg = llvm_function->arg_begin(); it_arg != llvm_function->arg_end(); ++it_arg, ++index)
		{
			//Identifier* parameter_identifier = ast_function.parameters[index]->name;

			const llvm::Type* t;
			llvm::Attributes modifier;
			if(mHelper.getType(*ast_function.parameters[index]->type, t, modifier) && t && !t->isVoidTy())
			{
				llvm::AllocaInst* parameter_alloc = createAlloca(t, NameManglingContext::get(ast_function.parameters[index])->managled_name + "_parameter_alloca");
				mBuilder.CreateStore(&(*it_arg), parameter_alloc);

				// store the alloca in the parameter identifier
				SET_SYNTHESIZED_LLVM_VALUE(ast_function.parameters[index], parameter_alloc);
			}
			else
			{
				BOOST_ASSERT(false && "writing to read-only LLVM type");
				return false;
			}
		}

		return true;
	}

	bool finishFunction(FunctionDecl& ast_function)
	{
		UNUSED_ARGUMENT(ast_function);

		enterBasicBlock(mFunctionContext.return_block);

		if(!mFunctionContext.return_value)
		{
			mBuilder.CreateRetVoid();
		}
		else
		{
			BOOST_ASSERT(!mFunctionContext.return_value->getType()->isVoidTy());
			mBuilder.CreateRet(mBuilder.CreateLoad(mFunctionContext.return_value));
		}

		mFunctionContext.alloca_insert_point->eraseFromParent();
		mFunctionContext.alloca_insert_point = NULL;

		// TODO simplify the block if there're few instructions by merging return and entry block

		// TODO add return instruction for void return function

		// TODO does this cause memory leak? don't know yet
		mFunctionContext.function = NULL;
		mFunctionContext.entry_block = NULL;
		mFunctionContext.return_block = NULL;

		return true;
	}

private:
	bool getValue(ASTNode& node, /*OUT*/ ASTNode*& resolved_symbol, /*OUT*/ llvm::Value*& llvm_value_for_read, /*OUT*/ llvm::Value*& llvm_value_for_write, bool require_read_access, bool require_write_access)
	{
		llvm_value_for_read = GET_SYNTHESIZED_LLVM_VALUE(&node);
		if(llvm_value_for_read && !llvm::isa<llvm::StoreInst>(llvm_value_for_read))
		{
			if(llvm_value_for_read->getType()->isPointerTy())
			{
				llvm_value_for_write = llvm_value_for_read;
				if(require_read_access)
				{
					llvm_value_for_read = mBuilder.CreateLoad(llvm_value_for_read);
				}
				else
				{
					llvm_value_for_read = NULL;
				}
			}
			else
			{
				if(require_write_access)
				{
					BOOST_ASSERT(false && "writing to read-only LLVM value");
					return false;
				}
			}
			resolved_symbol = &node;
			return true;
		}
		else
		{
			resolved_symbol = ResolvedSymbol::get(&node);
			if(!resolved_symbol)
				return false;

			llvm_value_for_read = GET_SYNTHESIZED_LLVM_VALUE(resolved_symbol);

			if(llvm_value_for_read)
			{
				if(llvm_value_for_read->getType()->isPointerTy())
				{
					llvm_value_for_write = llvm_value_for_read;
					if(require_read_access)
					{
						llvm_value_for_read = mBuilder.CreateLoad(llvm_value_for_read);
					}
					else
					{
						llvm_value_for_read = NULL;
					}
				}
				else
				{
					if(require_write_access)
					{
						BOOST_ASSERT(false && "writing to read-only LLVM value");
						return false;
					}
				}
			}
			else
			{
				// nested value resolve
				return getValue(*resolved_symbol, resolved_symbol, llvm_value_for_read, llvm_value_for_write, require_read_access, require_write_access);
			}
		}

		return true;
	}

private:
	bool isFloatType(ASTNode& node)
	{
		ASTNode* node_resolved = ResolvedType::get(&node);
		if(!node_resolved)
			return false;

		TypeSpecifier* node_specifier = cast<TypeSpecifier>(node_resolved);
		if(!node_resolved)
			return false;

		if(node_specifier->type != TypeSpecifier::ReferredType::PRIMITIVE)
			return false;

		if(!PrimitiveType::isFloatType(node_specifier->referred.primitive))
			return false;

		return true;
	}

	bool isFunctionParameter(ASTNode& node)
	{
		if(!isa<SimpleIdentifier>(&node))
			return false;

		if(!node.parent)
			return false;

		if(!isa<FunctionDecl>(node.parent))
			return false;

		return true;
	}

	int getFunctionParameterIndex(ASTNode& node)
	{
		if(!isa<SimpleIdentifier>(&node))
			return -1;

		if(!node.parent)
			return -1;

		if(!isa<FunctionDecl>(node.parent))
			return -1;

		SimpleIdentifier* p = cast<SimpleIdentifier>(&node);
		FunctionDecl* f = cast<FunctionDecl>(node.parent);

		int index = 0;
		foreach(i, f->parameters)
		{
			if((*i)->name == p) break;
			++index;
		}

		if(index >= (int)f->parameters.size())
			return -1;

		return index;
	}

	FunctionDecl* getContainingFunction(ASTNode& node)
	{
		if(!isa<SimpleIdentifier>(&node))
			return NULL;

		if(!node.parent)
			return NULL;

		return cast<FunctionDecl>(node.parent);
	}

	bool hasValue(ASTNode& ast_node)
	{
		if(GET_SYNTHESIZED_LLVM_VALUE(&ast_node))
			return true;
		else
			return false;
	}

	bool isResolved(ASTNode& ast_node)
	{
		if(ResolvedSymbol::get(&ast_node))
			return true;
		else
			return false;
	}

	bool isFunctionVisited(FunctionDecl& ast_function)
	{
		llvm::Function* llvm_function = GET_SYNTHESIZED_LLVM_FUNCTION(&ast_function);
		if(!llvm_function)
			return false;

		if(llvm_function->getBasicBlockList().size() == 0 )
			return false;

		return true;
	}

	bool isBlockInsertionMasked()
	{
		return mFunctionContext.mask_insertion;
	}

	void setBlockInsertionMask()
	{
		mFunctionContext.mask_insertion = true;
	}

	void resetBlockInsertionMask()
	{
		mFunctionContext.mask_insertion = false;
	}

	llvm::BasicBlock* currentBlock()
	{
		return mBuilder.GetInsertBlock();
	}

	bool isBlockTerminated(llvm::BasicBlock* block)
	{
		return (block->getTerminator()) ? true : false;
	}

private:
	bool propagate(ASTNode* to, ASTNode* from)
	{
		if(!from || !to) return false;

		llvm::Value* existing_value = GET_SYNTHESIZED_LLVM_VALUE(to);
		if(!existing_value)
		{
			llvm::Value* value = GET_SYNTHESIZED_LLVM_VALUE(from);
			if(value)
			{
				SET_SYNTHESIZED_LLVM_VALUE(to, value);
			}
			else
			{
				if(isa<VariableDecl>(from))
				{
					NodeInfoVisitor node_info_visitor;
					node_info_visitor.visit(*from);
					std::wstring from_info = node_info_visitor.stream.str();
					node_info_visitor.visit(*to);
					std::wstring to_info = node_info_visitor.stream.str();

					LOG4CXX_ERROR(LoggerWrapper::GeneratorStage, L"failed to propagate NULL value from \"" << from_info << "\" to \"" << to_info << L"\"");
				}
			}
		}

		return true;
	}

	llvm::LLVMContext &mContext;
	llvm::Module& mModule;
	llvm::IRBuilder<> mBuilder;
	LLVMHelper mHelper;

	struct {
		llvm::Function* function;
		llvm::BasicBlock* entry_block;
		llvm::BasicBlock* return_block;
		llvm::BasicBlock* continue_block;
		llvm::BasicBlock* break_block;
		llvm::Instruction* alloca_insert_point;
		llvm::Value* return_value;
		bool mask_insertion;
		//std::stack<llvm::BasicBlock*> block_stack;
	} mFunctionContext;
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORSTAGEVISITOR_H_ */
