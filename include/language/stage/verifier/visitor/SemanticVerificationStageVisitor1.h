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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR1_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR1_H_

#include "core/Prerequisite.h"
#include "language/context/TransformerContext.h"
#include "language/tree/visitor/general/GenericVisitor.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/tree/visitor/general/NameManglingVisitor.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/logging/StringTable.h"
#include "language/logging/LoggerWrapper.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/logging/StringTable.h"
#include "language/stage/verifier/context/SemanticVerificationContext.h"
#include "language/resolver/Resolver.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NameManglingVisitor;

// CHECKS IN SEMANTIC VERIFICATION STAGE 1

// ERRORS:
// ====================================
// UNEXPECTED_RETURN_VALUE
// MISSING_RETURN_VALUE
// WRITE_CONST
// INVALID_NONSTATIC_CALL
// INVALID_NONSTATIC_REF
// INVALID_ACCESS_PRIVATE
// INVALID_ACCESS_PROTECTED
// UNEXPECTED_VARIADIC_ARG

// WARNINGS:
// ====================================
// MISSING_RETURN
// UNINIT_REF
// CONTROL_REACHES_END
// MISSING_CASE

namespace zillians { namespace language { namespace stage { namespace visitor {

struct SemanticVerificationStageVisitor1 : GenericDoubleVisitor
{
	CREATE_INVOKER(verifyInvoker, verify)

	SemanticVerificationStageVisitor1()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(verifyInvoker)
	}

	void verify(ASTNode& node)
	{
		revisit(node);
	}

	void verify(MemberExpr& node)
	{
		ASTNode* resolved_symbol = ResolvedSymbol::get(&node);
		if(resolved_symbol) // NOTE: NULL if package
		{
			// INVALID_ACCESS_PRIVATE
			// INVALID_ACCESS_PROTECTED
			if(isa<VariableDecl>(resolved_symbol))
				verifyVisibilityAccessViolation(&node, cast<VariableDecl>(resolved_symbol));
			else if(isa<FunctionDecl>(resolved_symbol))
				verifyVisibilityAccessViolation(&node, cast<FunctionDecl>(resolved_symbol));
		}

		revisit(node);
	}

	void verify(PrimaryExpr& node)
	{
		if(node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
		{
			ASTNode* resolved_symbol = ResolvedSymbol::get(&node);
			if(resolved_symbol) // NOTE: NULL if package
			{
				if(isa<VariableDecl>(resolved_symbol))
				{
					VariableDecl* var_decl = cast<VariableDecl>(resolved_symbol);

					// INVALID_NONSTATIC_REF
					verifyStaticAccessViolation(&node, var_decl);

					// INVALID_ACCESS_PRIVATE
					// INVALID_ACCESS_PROTECTED
					verifyVisibilityAccessViolation(&node, var_decl);

					// UNINIT_REF
					if(!SemanticVerificationVariableDeclContext_HasBeenInit::get(var_decl))
						LOG_MESSAGE(UNINIT_REF, &node, _var_id = var_decl->name->toString());
				}
				else if(isa<FunctionDecl>(resolved_symbol))
				{
					FunctionDecl* func_decl = cast<FunctionDecl>(resolved_symbol);

					// INVALID_NONSTATIC_REF
					verifyStaticAccessViolation(&node, func_decl);

					// INVALID_ACCESS_PRIVATE
					// INVALID_ACCESS_PROTECTED
					verifyVisibilityAccessViolation(&node, func_decl);
				}
			}
		}

		revisit(node);
	}

	void verify(CallExpr& node)
	{
		// INVALID_NONSTATIC_CALL
		ASTNode* resolved_symbol = ResolvedSymbol::get(node.node);
		if(isa<FunctionDecl>(resolved_symbol))
			verifyStaticAccessViolation(&node, cast<FunctionDecl>(resolved_symbol), true);
		else if(isa<VariableDecl>(resolved_symbol))
			verifyStaticAccessViolation(&node, cast<VariableDecl>(resolved_symbol), true);

		// UNEXPECTED_VARIADIC_ARG
		foreach(i, node.parameters)
		{
			ASTNode* resolved_type = ResolvedType::get(*i);
			if(isa<TypeSpecifier>(resolved_type)) // NOTE: only need to handle ellipsis case
			{
				TypeSpecifier* type_specifier = cast<TypeSpecifier>(resolved_type);
				if(isEllipsis(type_specifier) && !is_end_of_foreach(i, node.parameters))
					LOG_MESSAGE(UNEXPECTED_VARIADIC_ARG, &node);
			}
		}

		revisit(node);
	}

	void verify(BinaryExpr& node)
	{
		if(node.isAssignment())
		{
			ASTNode* resolved_symbol = ResolvedSymbol::get(node.left);
			if(isa<VariableDecl>(resolved_symbol))
			{
				VariableDecl* var_decl = cast<VariableDecl>(resolved_symbol);

				// WRITE_CONST
				// if the binary expression is assignment and it's split from variable declaration
				// (by checking the split reference is DeclarativeStmt or not)
				// if it is, then the binary expression is the initializer, so we should skip checking "write to const"
				ASTNode* ref = SplitReferenceContext::get(&node);
				if((!ref || !isa<DeclarativeStmt>(ref)) && var_decl->is_const)
					LOG_MESSAGE(WRITE_CONST, &node, _var_id = var_decl->name->toString());

				// UNINIT_REF
				if(node.right->isRValue() || (node.right->isLValue()
						&& SemanticVerificationVariableDeclContext_HasBeenInit::is_bound(ResolvedSymbol::get(node.right))))
				{
					SemanticVerificationVariableDeclContext_HasBeenInit::bind(var_decl);
				}
			}
		}

		revisit(node);
	}

	void verify(BranchStmt& node)
	{
		if(node.opcode == BranchStmt::OpCode::RETURN)
		{
			FunctionDecl* func_decl = ASTNodeHelper::getOwner<FunctionDecl>(&node);

			// MISSING_RETURN
			SemanticVerificationFunctionDeclContext_ReturnCount::bind(func_decl)->count++;

			// MISSING_RETURN_VALUE
			// UNEXPECTED_RETURN_VALUE
			TypeSpecifier* param_type = func_decl->type;
			ASTNode* resolved_type = ResolvedType::get(&node);
			bool arg_is_void = isa<TypeSpecifier>(resolved_type) ?
					isVoid(cast<TypeSpecifier>(resolved_type)) : false; // NOTE: only need to handle void case
			if(!isVoid(param_type) && arg_is_void)
				LOG_MESSAGE(MISSING_RETURN_VALUE, &node, _type = param_type->toString());
			if(isVoid(param_type) && !arg_is_void)
				LOG_MESSAGE(UNEXPECTED_RETURN_VALUE, &node);
		}

		revisit(node);
	}

	void verify(FunctionDecl& node)
	{
		// UNINIT_REF
		foreach(i, node.parameters)
			SemanticVerificationVariableDeclContext_HasBeenInit::bind(*i);

		revisit(node);

		if(!isVoid(node.type))
		{
			// MISSING_RETURN
			if(SemanticVerificationFunctionDeclContext_ReturnCount::bind(&node)->count == 0)
				LOG_MESSAGE(MISSING_RETURN, &node);
			else
			{
				// CONTROL_REACHES_END
				if(SemanticVerificationBlockContext_BranchCount::bind(node.block)->count
						> SemanticVerificationFunctionDeclContext_ReturnCount::bind(&node)->count)
				{
					LOG_MESSAGE(CONTROL_REACHES_END, &node);
				}
			}
		}
	}

	void verify(Block& node)
	{
		revisit(node);

		// CONTROL_REACHES_END
		if(SemanticVerificationBlockContext_BranchCount::bind(&node)->count == 0)
			SemanticVerificationBlockContext_BranchCount::bind(&node)->count = 1;
	}

	void verify(IfElseStmt& node)
	{
		revisit(node);

		// CONTROL_REACHES_END
		size_t count = 0;
		count += SemanticVerificationBlockContext_BranchCount::bind(node.if_branch.block)->count;
		foreach(i, node.elseif_branches)
			count += SemanticVerificationBlockContext_BranchCount::bind((*i).block)->count;
		count += SemanticVerificationBlockContext_BranchCount::bind(node.else_block)->count;
		if(isa<Block>(node.parent))
			SemanticVerificationBlockContext_BranchCount::bind(node.parent)->count += count;
	}

	void verify(SwitchStmt& node)
	{
		// MISSING_CASE
		ASTNode* resolved_type = ResolvedType::get(node.node);
		if(isa<EnumDecl>(resolved_type)) // NOTE: only need to handle enum case
		{
			EnumDecl* enum_decl = cast<EnumDecl>(resolved_type);
			foreach(i, enum_decl->values)
				SemanticVerificationEnumKeyContext_HasVisited::bind(*i);
			foreach(i, node.cases)
			{
				ASTNode* resolved_symbol = ResolvedSymbol::get((*i).cond);
				if(isa<Identifier>(resolved_symbol))
					SemanticVerificationEnumKeyContext_HasVisited::unbind(resolved_symbol);
			}
			foreach(i, enum_decl->values)
			{
				if(SemanticVerificationEnumKeyContext_HasVisited::is_bound(*i))
				{
					if(!node.default_block)
						LOG_MESSAGE(MISSING_CASE, &node, _id = (*i)->name->toString());
					SemanticVerificationEnumKeyContext_HasVisited::unbind(*i);
				}
			}
		}

		revisit(node);

		// CONTROL_REACHES_END
		size_t count = 0;
		count += SemanticVerificationBlockContext_BranchCount::bind(node.node)->count;
		foreach(i, node.cases)
			count += SemanticVerificationBlockContext_BranchCount::bind((*i).block)->count;
		count += SemanticVerificationBlockContext_BranchCount::bind(node.default_block)->count;
		if(isa<Block>(node.parent))
			SemanticVerificationBlockContext_BranchCount::bind(node.parent)->count += count;
	}

private:
	static bool isVoid(TypeSpecifier* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return node->type == TypeSpecifier::ReferredType::PRIMITIVE
				&& node->referred.primitive == PrimitiveType::VOID;
	}

	static bool isEllipsis(TypeSpecifier* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return node->type == TypeSpecifier::ReferredType::PRIMITIVE
				&& node->referred.primitive == PrimitiveType::VARIADIC_ELLIPSIS;
	}

	template<class T>
	static void verifyVisibilityAccessViolation(ASTNode* node_ref, T* node_decl)
	{
		BOOST_ASSERT(node_ref && node_decl && "null pointer exception");
		BOOST_ASSERT(!node_decl->name->toString().empty() && "empty name");

		// INVALID_ACCESS_PRIVATE
		// INVALID_ACCESS_PROTECTED
		if(node_decl->is_member)
		{
			ClassDecl* ref_point = ASTNodeHelper::getOwner<ClassDecl>(node_ref);
			ClassDecl* decl_point = ASTNodeHelper::getOwner<ClassDecl>(node_decl);
			if(decl_point && ref_point != decl_point)
			{
				switch(node_decl->visibility)
				{
				case Declaration::VisibilitySpecifier::PRIVATE:
					LOG_MESSAGE(INVALID_ACCESS_PRIVATE, node_ref, _id = node_decl->name->toString());
					break;
				case Declaration::VisibilitySpecifier::PROTECTED:
					if(!ref_point || !ASTNodeHelper::isInheritedFrom(ref_point, decl_point))
						LOG_MESSAGE(INVALID_ACCESS_PROTECTED, node_ref, _id = node_decl->name->toString());
					break;
				}
			}
		}
	}

	template<class T>
	static void verifyStaticAccessViolation(ASTNode* node_ref, T* node_decl, bool is_call = false)
	{
		// INVALID_NONSTATIC_REF
		if(ASTNodeHelper::getOwner<FunctionDecl>(node_ref)->is_static && node_decl->is_member && !node_decl->is_static)
		{
			if(is_call)
				LOG_MESSAGE(INVALID_NONSTATIC_CALL, node_ref, _func_id = node_decl->name->toString());
			else
				LOG_MESSAGE(INVALID_NONSTATIC_REF, node_ref, _var_id = node_decl->name->toString());
		}
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR1_H_ */
