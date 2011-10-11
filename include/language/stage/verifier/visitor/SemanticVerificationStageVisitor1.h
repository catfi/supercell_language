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
		ASTNode* unknown = ResolvedSymbol::get(&node);
		if(unknown) // NOTE: NULL if package
			if(isa<VariableDecl>(unknown))
			{
				VariableDecl* var_decl = cast<VariableDecl>(unknown);
				verifyVisibilityAccessViolation(&node, unknown, var_decl->name->toString(), var_decl->visibility);
			}
			else if(isa<FunctionDecl>(unknown))
			{
				FunctionDecl* func_decl = cast<FunctionDecl>(unknown);
				verifyVisibilityAccessViolation(&node, unknown, func_decl->name->toString(), func_decl->visibility);
			}

		revisit(node);
	}

	void verify(PrimaryExpr& node)
	{
		if(ASTNodeHelper::hasOwner<FunctionDecl>(node) && node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
		{
			ASTNode* unknown = ResolvedSymbol::get(&node);
			if(unknown) // NOTE: NULL if package
				if(isa<VariableDecl>(unknown))
				{
					VariableDecl* var_decl = cast<VariableDecl>(unknown);

					if(!ASTNodeHelper::isFuncParam(var_decl))
					{
						// UNINIT_REF
						if(!SemanticVerificationVariableDeclContext_HasBeenInit::get(var_decl))
							LOG_MESSAGE(UNINIT_REF, &node, _var_id = var_decl->name->toString());

						// INVALID_NONSTATIC_REF
						if(ASTNodeHelper::getOwner<FunctionDecl>(node)->is_static && !var_decl->is_static)
							LOG_MESSAGE(INVALID_NONSTATIC_REF, &node, _var_id = var_decl->name->toString());
					}

					verifyVisibilityAccessViolation(&node, unknown, var_decl->name->toString(), var_decl->visibility);
				}
				else if(isa<FunctionDecl>(unknown))
				{
					FunctionDecl* func_decl = cast<FunctionDecl>(unknown);

					// INVALID_NONSTATIC_REF
					if(ASTNodeHelper::getOwner<FunctionDecl>(node)->is_static && !func_decl->is_static)
						LOG_MESSAGE(INVALID_NONSTATIC_REF, &node, _var_id = func_decl->name->toString());

					verifyVisibilityAccessViolation(&node, unknown, func_decl->name->toString(), func_decl->visibility);
				}
		}

		revisit(node);
	}

	void verify(CallExpr& node)
	{
		// INVALID_NONSTATIC_CALL
		bool is_static = false;
		std::wstring name;
		ASTNode* unknown = ResolvedSymbol::get(node.node);
		if(isa<FunctionDecl>(unknown))
		{
			FunctionDecl* func_decl = cast<FunctionDecl>(unknown);
			is_static = !func_decl->is_static;
			name = func_decl->name->toString();
		}
		else if(isa<VariableDecl>(unknown))
		{
			VariableDecl* var_decl = cast<VariableDecl>(unknown);
			is_static = !var_decl->is_static;
			name = var_decl->name->toString();
		}
		if(ASTNodeHelper::getOwner<FunctionDecl>(node)->is_static && is_static)
			LOG_MESSAGE(INVALID_NONSTATIC_CALL, &node, _func_id = name);

		revisit(node);
	}

	void verify(BinaryExpr& node)
	{
		if(node.isAssignment())
		{
			ASTNode* unknown = ResolvedSymbol::get(node.left);
			if(isa<VariableDecl>(unknown))
			{
				VariableDecl* var_decl = cast<VariableDecl>(unknown);

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
			FunctionDecl* func_decl = ASTNodeHelper::getOwner<FunctionDecl>(node);

			// MISSING_RETURN
			SemanticVerificationFunctionDeclContext_ReturnCount::bind(func_decl)->count++;

			// MISSING_RETURN_VALUE
			// UNEXPECTED_RETURN_VALUE
			TypeSpecifier* return_param = func_decl->type;
			TypeSpecifier* return_arg = cast<TypeSpecifier>(ResolvedType::get(&node));
			if(!isVoid(return_param) && isVoid(return_arg))
				LOG_MESSAGE(MISSING_RETURN_VALUE, &node, _type = return_param->toString());
			if(isVoid(return_param) && !isVoid(return_arg))
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
		foreach(i, node.cases)
		{
			ASTNode* unknown = ResolvedSymbol::get((*i).cond);
			if(isa<Identifier>(unknown))
				SemanticVerificationEnumKeyContext_HasVisited::bind(unknown);
		}
		EnumDecl* enum_decl = resolveEnum(node.node);
		if(enum_decl)
			foreach(i, enum_decl->enumeration_list)
				SemanticVerificationEnumKeyContext_HasVisited::unbind((*i).first);
		foreach(i, enum_decl->enumeration_list)
			if(SemanticVerificationEnumKeyContext_HasVisited::is_bound((*i).first))
			{
				if(!node.default_block)
					LOG_MESSAGE(MISSING_CASE, &node, _id = (*i).first->toString());
				SemanticVerificationEnumKeyContext_HasVisited::unbind((*i).first);
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
	static bool isVoid(TypeSpecifier* type_specifier)
	{
		return type_specifier->type == TypeSpecifier::ReferredType::PRIMITIVE
				&& type_specifier->referred.primitive == PrimitiveType::VOID;
	}

	static EnumDecl* resolveEnum(ASTNode* unknown)
	{
		TypeSpecifier* type_specifier = cast<TypeSpecifier>(ResolvedType::get(unknown));
		if(type_specifier->type != TypeSpecifier::ReferredType::UNSPECIFIED)
			return NULL;
		ASTNode* unknown_unspecified = ResolvedSymbol::get(type_specifier->referred.unspecified);
		if(!isa<EnumDecl>(unknown_unspecified))
			return NULL;
		return cast<EnumDecl>(unknown_unspecified);
	}

	static void verifyVisibilityAccessViolation(
			ASTNode* unknown_ref, ASTNode* unknown_decl, std::wstring name_decl, Declaration::VisibilitySpecifier::type visibility_decl)
	{
		// INVALID_ACCESS_PRIVATE
		// INVALID_ACCESS_PROTECTED
		ClassDecl* ref_point = ASTNodeHelper::getOwner<ClassDecl>(*unknown_ref);
		ClassDecl* decl_point = ASTNodeHelper::getOwner<ClassDecl>(*unknown_decl);
		if(ref_point != decl_point)
			switch(visibility_decl)
			{
			case Declaration::VisibilitySpecifier::PRIVATE:
				LOG_MESSAGE(INVALID_ACCESS_PRIVATE, unknown_ref, _id = name_decl);
				break;
			case Declaration::VisibilitySpecifier::PROTECTED:
				if(!(ref_point && decl_point && ASTNodeHelper::isInheritedFrom(*ref_point, *decl_point)))
					LOG_MESSAGE(INVALID_ACCESS_PROTECTED, unknown_ref, _id = name_decl);
				break;
			}
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR1_H_ */
