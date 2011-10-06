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

	void verify(PrimaryExpr &node)
	{
		if(!!ASTNodeHelper::owner<FunctionDecl>(node) && node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
		{
			ASTNode* unknown = ResolvedSymbol::get(&node);
			if(isa<VariableDecl>(unknown) || isa<FunctionDecl>(unknown))
			{
				std::wstring name;
				bool static_violation = false;
				Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;

				if(isa<VariableDecl>(unknown))
				{
					VariableDecl* var_decl = cast<VariableDecl>(unknown);
					name = var_decl->name->toString();
					visibility = var_decl->visibility;

					if(!ASTNodeHelper::isFuncParam(var_decl))
					{
						// UNINIT_REF
						if(!SemanticVerificationVariableDeclContext_HasBeenInit::get(var_decl))
							LOG_MESSAGE(UNINIT_REF, &node, _var_id = name);

						// INVALID_NONSTATIC_REF
						static_violation = ASTNodeHelper::owner<FunctionDecl>(node)->is_static && !var_decl->is_static;
					}
				}
				else if(isa<FunctionDecl>(unknown))
				{
					FunctionDecl* func_decl = cast<FunctionDecl>(unknown);
					name = func_decl->name->toString();
					visibility = func_decl->visibility;

					// INVALID_NONSTATIC_REF
					static_violation = ASTNodeHelper::owner<FunctionDecl>(node)->is_static && !func_decl->is_static;
				}

				if(static_violation)
					LOG_MESSAGE(INVALID_NONSTATIC_REF, &node, _var_id = name);

				// INVALID_ACCESS_PRIVATE
				// INVALID_ACCESS_PROTECTED
				ClassDecl* ref_point = ASTNodeHelper::owner<ClassDecl>(node);
				ClassDecl* decl_point = ASTNodeHelper::owner<ClassDecl>(*unknown);
				if(ref_point != decl_point)
					switch(visibility)
					{
					case Declaration::VisibilitySpecifier::PRIVATE:
						LOG_MESSAGE(INVALID_ACCESS_PRIVATE, &node, _id = name);
						break;
					case Declaration::VisibilitySpecifier::PROTECTED:
						if(!(!!ref_point && !!decl_point && ASTNodeHelper::extends(*ref_point, *decl_point)))
						{
							LOG_MESSAGE(INVALID_ACCESS_PROTECTED, &node, _id = name);
						}
						break;
					}
			}
		}

		revisit(node);
	}

	void verify(CallExpr &node)
	{
		// INVALID_NONSTATIC_CALL
		bool decl_is_static = false;
		std::wstring name;
		ASTNode* unknown = ResolvedSymbol::get(node.node);
		if(isa<FunctionDecl>(unknown))
		{
			FunctionDecl* func_decl = cast<FunctionDecl>(unknown);
			decl_is_static = !func_decl->is_static;
			name = func_decl->name->toString();
		}
		else if(isa<VariableDecl>(unknown))
		{
			VariableDecl* var_decl = cast<VariableDecl>(unknown);
			decl_is_static = !var_decl->is_static;
			name = var_decl->name->toString();
		}
		if(ASTNodeHelper::owner<FunctionDecl>(node)->is_static && decl_is_static)
			LOG_MESSAGE(INVALID_NONSTATIC_CALL, &node, _func_id = name);

		revisit(node);
	}

	void verify(BinaryExpr &node)
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
						&& !!SemanticVerificationVariableDeclContext_HasBeenInit::get(ResolvedSymbol::get(node.right))))
				{
					SemanticVerificationVariableDeclContext_HasBeenInit::instance(var_decl);
				}
			}
		}

		revisit(node);
	}

	void verify(BranchStmt &node)
	{
		if(node.opcode == BranchStmt::OpCode::RETURN)
		{
			FunctionDecl* func_decl = ASTNodeHelper::owner<FunctionDecl>(node);

			// CONTROL_REACHES_END
			// MISSING_RETURN
			SemanticVerificationFunctionDeclContext_ReturnCount* ReturnCount_context =
					SemanticVerificationFunctionDeclContext_ReturnCount::instance(func_decl);
			ReturnCount_context->count++;

			TypeSpecifier* return_param = func_decl->type;
			TypeSpecifier* return_arg = cast<TypeSpecifier>(ResolvedType::get(&node));

			// UNEXPECTED_RETURN_VALUE
			if(_is_void(return_param) && !_is_void(return_arg))
				LOG_MESSAGE(UNEXPECTED_RETURN_VALUE, &node);

			// MISSING_RETURN_VALUE
			if(!_is_void(return_param) && _is_void(return_arg))
				LOG_MESSAGE(MISSING_RETURN_VALUE, &node, _type = return_param->toString());
		}

		revisit(node);
	}

	void verify(FunctionDecl &node)
	{
		// UNINIT_REF
		foreach(i, node.parameters)
			SemanticVerificationVariableDeclContext_HasBeenInit::instance(*i);

		revisit(node);

		if(!_is_void(node.type))
		{
			SemanticVerificationFunctionDeclContext_ReturnCount* ReturnCount_context =
					SemanticVerificationFunctionDeclContext_ReturnCount::get(&node);

			if(!!ReturnCount_context)
			{
				SemanticVerificationFunctionDeclContext_PathCount* PathCount_context =
						SemanticVerificationFunctionDeclContext_PathCount::instance(&node);

				// CONTROL_REACHES_END
				// NOTE: FIX-ME! -- should use context up-propagation instead -- see impl for s0 "DEAD_CODE"
				if(PathCount_context->count > ReturnCount_context->count)
					LOG_MESSAGE(CONTROL_REACHES_END, &node);
			}
			else
			{
				// MISSING_RETURN
				LOG_MESSAGE(MISSING_RETURN, &node);
			}
		}
	}

	void verify(IfElseStmt &node)
	{
		// CONTROL_REACHES_END
		FunctionDecl* func_decl = ASTNodeHelper::owner<FunctionDecl>(node);
		SemanticVerificationFunctionDeclContext_PathCount* PathCount_context =
				SemanticVerificationFunctionDeclContext_PathCount::instance(func_decl);
		PathCount_context->count += 1+node.elseif_branches.size()+(!!node.else_block ? 1 : 0);

		revisit(node);
	}

	void verify(SwitchStmt &node)
	{
		// CONTROL_REACHES_END
		FunctionDecl* func_decl = ASTNodeHelper::owner<FunctionDecl>(node);
		SemanticVerificationFunctionDeclContext_PathCount* PathCount_context =
				SemanticVerificationFunctionDeclContext_PathCount::instance(func_decl);

		// MISSING_CASE
		typedef std::map<std::wstring, int> enum_key_value_map_t;
		enum_key_value_map_t enum_key_value_map;
		EnumDecl* enum_decl = _resolve_enum(node.node);
		if(!!enum_decl)
			foreach(i, enum_decl->enumeration_list)
			{
				std::wstring key = (*i).first->toString();
				int value = -1;
				if(_resolve_int32((*i).second, &value))
					enum_key_value_map.insert(enum_key_value_map_t::value_type(key, value));
			}
		typedef std::set<int> case_value_set_t;
		case_value_set_t case_value_set;
		foreach(i, node.cases)
		{
			int value = -1;
			if(_resolve_int32((*i).cond, &value))
				case_value_set.insert(value);
			else if(isa<MemberExpr>((*i).cond))
			{
				MemberExpr* member_expr = cast<MemberExpr>((*i).cond);
				EnumDecl* enum_decl = _resolve_enum(member_expr->node);
				if(!!enum_decl)
					if(isa<SimpleIdentifier>(member_expr->member))
					{
						enum_key_value_map_t::iterator p = enum_key_value_map.find(member_expr->member->toString());
						if(p != enum_key_value_map.end())
							case_value_set.insert((*p).second);
					}
			}

			// CONTROL_REACHES_END
			PathCount_context->count++;
		}
		std::vector<std::wstring> result;
		if(!!node.default_block)
		{
			// CONTROL_REACHES_END
			PathCount_context->count++;
		}
		else
			foreach(i, enum_key_value_map)
				if(case_value_set.find((*i).second) == case_value_set.end())
					LOG_MESSAGE(MISSING_CASE, &node, _id = (*i).first);
	}

private:
	static bool _is_void(TypeSpecifier* type_specifier)
	{
		return type_specifier->type == TypeSpecifier::ReferredType::PRIMITIVE
				&& type_specifier->referred.primitive == PrimitiveType::VOID;
	}

	static EnumDecl* _resolve_enum(ASTNode* unknown)
	{
		TypeSpecifier* type_specifier = cast<TypeSpecifier>(ResolvedType::get(unknown));
		if(type_specifier->type != TypeSpecifier::ReferredType::UNSPECIFIED)
			return NULL;
		ASTNode* unknown_unspecified = ResolvedSymbol::get(type_specifier->referred.unspecified);
		if(!isa<EnumDecl>(unknown_unspecified))
			return NULL;
		return cast<EnumDecl>(unknown_unspecified);
	}

	static bool _resolve_int32(Expression* expr, int* value)
	{
		if(!isa<PrimaryExpr>(expr))
			return false;
		PrimaryExpr* prim_expr = cast<PrimaryExpr>(expr);
		if(prim_expr->catagory != PrimaryExpr::Catagory::LITERAL)
			return false;
		if(!isa<NumericLiteral>(prim_expr->value.literal))
			return false;
		*value = cast<NumericLiteral>(prim_expr->value.literal)->value.i32;
		return true;
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR1_H_ */
