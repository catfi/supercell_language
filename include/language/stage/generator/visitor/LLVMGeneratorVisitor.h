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
#include "language/tree/visitor/general/NodeInfoVisitor.h"
#include "language/stage/generator/detail/LLVMForeach.h"
#include "language/resolver/context/ResolverContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"


using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NodeInfoVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct LLVMGeneratorVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(generateInvoker, generate)

	LLVMGeneratorVisitor(llvm::LLVMContext& context, llvm::Module& module) :
		mContext(context), mModule(module), mBuilder(context)
	{
		mFunctionContext.function = NULL;
		mFunctionContext.entry_block = NULL;
		mFunctionContext.return_block = NULL;
		mFunctionContext.alloca_insert_point = NULL;

		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	void generate(ASTNode& node)
	{
		revisit(node);
	}

	void generate(NumericLiteral& node)
	{
		revisit(node);

		llvm::Value* result = NULL;
		switch(node.type)
		{
		case PrimitiveType::UINT64:
			result = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(mContext), node.value.u64, false); break;
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		node.set<llvm::Value>(result);
	}

	void generate(ObjectLiteral& node)
	{
		revisit(node);
	}

	void generate(StringLiteral& node)
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
		if(!hasFunction(node))
		{
			// create function signature (if necessary) and emit prologue of function
			if(!startFunction(node))
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
	}

	void generate(VariableDecl& node)
	{
		revisit(node);

		// here we only generate AllocaInst for AST variables within function
		// those sit in global scope or class member scope will be stored in some other object and access through game object API
		// (all global variables are stored in a single game object, which is assembled by compiler)
		if(isDeclaredInFunction(node))
		{
			createAlloca(node);

			if(node.initializer)
			{
				llvm::Value* llvm_alloca_inst = node.get<llvm::Value>();
				llvm::Value* llvm_init = node.initializer->get<llvm::Value>();

				if(llvm_init)
				{
					llvm::cast<llvm::AllocaInst>(llvm_alloca_inst)->getAllocatedType()->dump();
					llvm::cast<llvm::PointerType>(llvm_alloca_inst->getType())->getElementType()->dump();
					llvm_init->getType()->dump();
					BOOST_ASSERT(llvm::cast<llvm::AllocaInst>(llvm_alloca_inst)->getAllocatedType() == llvm_init->getType());
					BOOST_ASSERT(llvm::cast<llvm::PointerType>(llvm_alloca_inst->getType())->getElementType() == llvm_init->getType());
					mBuilder.CreateStore(llvm_init, llvm_alloca_inst);
				}
			}
		}
	}

	void generate(DeclarativeStmt& node)
	{
		revisit(node);

		propagate(&node, node.declaration);
	}

	void generate(BranchStmt& node)
	{
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
			llvm::Value* return_value = node.result->get<llvm::Value>();
			BOOST_ASSERT(return_value && "invalid LLVM value");

			result = mBuilder.CreateRet(return_value);
			break;
		}
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		node.set<llvm::Value>(result);
	}

	void generate(ExpressionStmt& node)
	{
		revisit(node);

		propagate(&node, node.expr);
	}

	void generate(IfElseStmt& node)
	{
		revisit(node);
		// emit the preamble and prepare blocks
	}

	void generate(ForeachStmt& node)
	{
		revisit(node);
		// emit the preamble and prepare blocks
	}

	void generate(WhileStmt& node)
	{
		revisit(node);
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
		revisit(node);
		// emit the preamble and prepare blocks
	}

	void generate(PrimaryExpr& node)
	{
		revisit(node);

		switch(node.catagory)
		{
		case PrimaryExpr::Catagory::IDENTIFIER:
			propagate(&node, ResolvedSymbol::get(node.value.identifier)); break;
		case PrimaryExpr::Catagory::LITERAL:
			propagate(&node, node.value.literal); break;
		case PrimaryExpr::Catagory::LAMBDA:
			break;
		}
	}

	void generate(UnaryExpr& node)
	{
		revisit(node);

		llvm::Value* result = NULL;

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

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		node.set<llvm::Value>(result);
	}

	void generate(BinaryExpr& node)
	{
		revisit(node);

		llvm::Value* result = NULL;
		llvm::Value* lhs = node.left->get<llvm::Value>();
		llvm::Value* rhs = node.right->get<llvm::Value>();

		// TODO implement load instruction cache here to prevent reduntent load
		// if either LHS or RHS is a AllocaInst, we need to load its value to register
		llvm::Value* lhs_value = llvm::isa<llvm::AllocaInst>(lhs) ? mBuilder.CreateLoad(lhs) : lhs;
		llvm::Value* rhs_value = llvm::isa<llvm::AllocaInst>(rhs) ? mBuilder.CreateLoad(rhs) : rhs;
		llvm::Value* temporary = NULL;

		switch(node.opcode)
		{
		case BinaryExpr::OpCode::BINARY_AND:
			result = mBuilder.CreateAnd(lhs_value, rhs_value); break;
		case BinaryExpr::OpCode::BINARY_OR:
			result = mBuilder.CreateOr(lhs_value, rhs_value); break;
		case BinaryExpr::OpCode::BINARY_XOR:
			result = mBuilder.CreateXor(lhs_value, rhs_value); break;
		case BinaryExpr::OpCode::BINARY_LSHIFT:
			// TODO what's the left shift operation?
			break;
		case BinaryExpr::OpCode::BINARY_RSHIFT:
			// TODO should we use CreateAShr or CreateLShr? depending on the object type?
			result = mBuilder.CreateLShr(lhs_value, rhs_value); break;

		case BinaryExpr::OpCode::ARITHMETIC_ADD:
			result = (isFloatType(node)) ? mBuilder.CreateFAdd(lhs_value, rhs_value) : mBuilder.CreateAdd(lhs_value, rhs_value); break;
		case BinaryExpr::OpCode::ARITHMETIC_SUB:
			result = (isFloatType(node)) ? mBuilder.CreateFSub(lhs_value, rhs_value) : mBuilder.CreateSub(lhs_value, rhs_value); break;
		case BinaryExpr::OpCode::ARITHMETIC_MUL:
			result = (isFloatType(node)) ? mBuilder.CreateFMul(lhs_value, rhs_value) : mBuilder.CreateMul(lhs_value, rhs_value); break;
		case BinaryExpr::OpCode::ARITHMETIC_DIV:
			// TODO should be use CreateUDiv or CreateSDiv? depending on whether LHS or RHS is signed or unsigned
			// TODO current zscript does not handle this correctly
			// TODO see clang CGExprScalar.cpp:1737 hasUnsignedIntegerRepresentation()
			result = (isFloatType(node)) ? mBuilder.CreateFDiv(lhs_value, rhs_value) : mBuilder.CreateUDiv(lhs_value, rhs_value); break;

		case BinaryExpr::OpCode::ASSIGN:
			result = mBuilder.CreateStore(rhs_value, lhs_value);
			break;
		case BinaryExpr::OpCode::ADD_ASSIGN:
			temporary = (isFloatType(node)) ? mBuilder.CreateFAdd(lhs_value, rhs_value) : mBuilder.CreateAdd(lhs_value, rhs_value);
			result = mBuilder.CreateStore(temporary, lhs_value);
			break;
		case BinaryExpr::OpCode::SUB_ASSIGN:
			temporary = (isFloatType(node)) ? mBuilder.CreateFSub(lhs_value, rhs_value) : mBuilder.CreateSub(lhs_value, rhs_value);
			result = mBuilder.CreateStore(temporary, lhs_value);
			break;
		case BinaryExpr::OpCode::MUL_ASSIGN:
			temporary = (isFloatType(node)) ? mBuilder.CreateFMul(lhs_value, rhs_value) : mBuilder.CreateMul(lhs_value, rhs_value);
			result = mBuilder.CreateStore(temporary, lhs_value);
			break;
		case BinaryExpr::OpCode::DIV_ASSIGN:
			temporary = (isFloatType(node)) ? mBuilder.CreateFDiv(lhs_value, rhs_value) : mBuilder.CreateUDiv(lhs_value, rhs_value);
			result = mBuilder.CreateStore(temporary, lhs_value);
			break;
		default:
			result = NULL; break;
		}

		if(!result)
		{
			BOOST_ASSERT(false && "invalid LLVM interpretation");
			terminateRevisit();
		}

		node.set<llvm::Value>(result);
	}

	void generate(TernaryExpr& node)
	{
		revisit(node);
		// TODO use predicate
	}

	void generate(CallExpr& node)
	{
		revisit(node);
		// TODO depending on the LHS, if it's a directly-invokable function, just get the function prototype and invoke
		// TODO if it's not a directly-invokable function, which can be a class member function, pass this pointer to that function and make the call
		// TODO if it's a class member function and it's virtual, we have different calling convention here
	}

	void generate(CastExpr& node)
	{
		revisit(node);
		// TODO only generate code when there's type mismatch
	}

	void generate(MemberExpr& node)
	{
		revisit(node);
		// TODO if the node is resolved to a package, do nothing
		// TODO otherwise, the node is resolved to a local variable, if the RHS is a member variable, use object resolver to resolve correct offset
		// TODO if RHS is a member function, use object resolver to resolve correct function pointer (if it's a non-virtual function)
		// TODO if it's virtual function, resolve to correct table index using VTable
	}

private:

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
			modifier |= llvm::Attribute::SExt;
			result = llvm::IntegerType::getInt64Ty(mContext);
			resolved = true; break;
		}
		case PrimitiveType::UINT64:
		{
			modifier |= llvm::Attribute::ZExt;
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
		case TypeSpecifier::ReferredType::CLASS_DECL: // TODO return pointer type
		case TypeSpecifier::ReferredType::INTERFACE_DECL: // TODO return pointer type
		{
			// return generic pointer type, which is an unsigned int32
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case TypeSpecifier::ReferredType::FUNCTION_DECL:
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
		{
			// TODO return pointer to function type
			result = NULL;
			resolved = false; break;
		}
		case TypeSpecifier::ReferredType::ENUM_DECL:
		{
			modifier |= llvm::Attribute::ZExt;
			result = llvm::IntegerType::getInt32Ty(mContext);
			resolved = true; break;
		}
		case TypeSpecifier::ReferredType::PRIMITIVE:
		{
			return getType(specifier.referred.primitive, result, modifier);
		}
		case TypeSpecifier::ReferredType::TYPEDEF_DECL:
		case TypeSpecifier::ReferredType::UNSPECIFIED:
			// TODO these are cases that shouldn't happen
			result = NULL;
			resolved = false; break;
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

				if(!getType(*i->second, t, attr))
					return false;

				llvm_function_parameter_types.push_back(t);
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

	bool hasFunction(FunctionDecl& ast_function)
	{
		if(ast_function.get<llvm::Function>())
			return true;
		else
			return false;
	}

	bool getFunction(FunctionDecl& ast_function, /*OUT*/ llvm::Function*& llvm_function)
	{
		if(!!(llvm_function = ast_function.get<llvm::Function>()))
			return true;

		llvm::BasicBlock* bb;

		llvm::FunctionType* llvm_function_type = NULL;
		std::vector<llvm::AttributeWithIndex> llvm_function_parameter_type_attributes;
		llvm::Attributes llvm_function_return_type_attribute;

		// try to resolve function type
		if(!getFunctionType(ast_function, llvm_function_type, llvm_function_parameter_type_attributes, llvm_function_return_type_attribute))
			return false;

		// TODO we should provide some generator name manging helper
		llvm_function = llvm::Function::Create(llvm_function_type, llvm::Function::ExternalLinkage, NameManglingContext::get(&ast_function)->managled_name, &mModule);

		if(!llvm_function)
			return false;

		// set function attributes (modifiers)
		llvm_function->setAttributes(llvm::AttrListPtr::get(llvm_function_parameter_type_attributes.begin(), llvm_function_parameter_type_attributes.end()));

		// associate the LLVM function object with AST FunctionDecl object
		ast_function.set<llvm::Function>(llvm_function);

		return true;
	}

	llvm::BasicBlock* createBasicBlock(llvm::StringRef name = "", llvm::Function* parent = NULL, llvm::BasicBlock* before = NULL)
	{
		return llvm::BasicBlock::Create(mContext, name, parent, before);
	}

	bool createAlloca(VariableDecl& ast_variable)
	{
		if(!!ast_variable.get<llvm::Value>())
			return true;

		const llvm::Type* llvm_variable_type = NULL;
		llvm::Attributes llvm_variable_modifier = llvm::Attribute::None;
		if(!getType(*ast_variable.type, llvm_variable_type, llvm_variable_modifier))
			return false;

		llvm_variable_type->dump();

		llvm::AllocaInst* llvm_alloca_inst = NULL;
		if(mBuilder.isNamePreserving())
			llvm_alloca_inst = new llvm::AllocaInst(llvm_variable_type, 0, NameManglingContext::get(&ast_variable)->managled_name, mFunctionContext.alloca_insert_point);
		else
			llvm_alloca_inst = new llvm::AllocaInst(llvm_variable_type, 0, "", mFunctionContext.alloca_insert_point);

		ast_variable.set<llvm::Value>(llvm_alloca_inst);

		return true;
	}

	bool startFunction(FunctionDecl& ast_function)
	{
		BOOST_ASSERT(!mFunctionContext.function);
		BOOST_ASSERT(!mFunctionContext.entry_block);
		BOOST_ASSERT(!mFunctionContext.return_block);
		BOOST_ASSERT(!mFunctionContext.alloca_insert_point);

		llvm::Function* llvm_function = NULL;
		if(!getFunction(ast_function, llvm_function))
			return false;

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
			if(getType(*ast_function.type, type, modifier) && type && !type->isVoidTy())
			{
				mFunctionContext.return_value = new llvm::AllocaInst(type, 0, "retval", mFunctionContext.alloca_insert_point);
			}
			else
			{
				mFunctionContext.return_value = NULL;
			}
		}

//		pushBlock(mFunctionContext.return_block);

		return true;
	}

//	llvm::BasicBlock* currentBlock()
//	{
//		return mFunctionContext.block_stack.top();
//	}

//	void pushBlock(llvm::BasicBlock* block)
//	{
//		mFunctionContext.block_stack.push(block);
//	}
//
//	void popBlock()
//	{
//		if()
//	}

	bool finishFunction(FunctionDecl& ast_function)
	{
		if(mBuilder.GetInsertBlock() != mFunctionContext.return_block)
		{
			mBuilder.CreateBr(mFunctionContext.return_block);
		}

		if(!mFunctionContext.return_value || mFunctionContext.return_value->getType()->isVoidTy())
		{
			mBuilder.SetInsertPoint(mFunctionContext.return_block);
			mBuilder.CreateRetVoid();
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
	bool isFloatType(ASTNode& node)
	{
		// TODO check the resolve type
		return false;
	}

	bool isDeclaredInFunction(ASTNode& node)
	{
		ASTNode* current = &node;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return true;
			current = current->parent;
		}

		return false;
	}

private:
	bool propagate(ASTNode* to, ASTNode* from)
	{
		if(!from || !to) return false;

		llvm::Value* v = from->get<llvm::Value>();
		if(v)
			to->set<llvm::Value>(v);
		else
		{
			NodeInfoVisitor node_info_visitor;
			node_info_visitor.visit(*from);
			std::wstring from_info = node_info_visitor.stream.str();
			node_info_visitor.visit(*to);
			std::wstring to_info = node_info_visitor.stream.str();

			LOG4CXX_ERROR(Logger::GeneratorStage, L"failed to propagate NULL value from \"" << from_info << "\" to \"" << to_info << L"\"");
		}

		return true;
	}

	llvm::LLVMContext &mContext;
	llvm::Module& mModule;
	llvm::IRBuilder<> mBuilder;

	struct {
		llvm::Function* function;
		llvm::BasicBlock* entry_block;
		llvm::BasicBlock* return_block;
		llvm::Instruction* alloca_insert_point;
		llvm::Value* return_value;
		//std::stack<llvm::BasicBlock*> block_stack;
	} mFunctionContext;
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGENERATORVISITOR_H_ */
