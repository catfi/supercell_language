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

	void verify_UNINIT_REF(ASTNode &node)
	{
		// UNINIT_REF
		if(ASTNodeHelper::isOwnedByRValue(node))
		{
			ASTNode* decl = ResolvedSymbol::get(&node);
			if(isa<VariableDecl>(decl) && ASTNodeHelper::isOwnedByFunction(node))
			{
				VariableDecl* var_decl = cast<VariableDecl>(decl);
				if(!SemanticVerificationVariableDeclContext_HasBeenInit::get(var_decl))
					LOG_MESSAGE(UNINIT_REF, ASTNodeHelper::getOwnerAnnotationAttachPoint(node), _var_id = var_decl->name->toString());
			}
		}
	}

	void verify(PrimaryExpr &node)
	{
		// UNINIT_REF
		if(node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
			verify_UNINIT_REF(node);
	}

	void verify(MemberExpr &node)
	{
		// UNINIT_REF
		verify_UNINIT_REF(*node.node);

		revisit(node);
	}

	void verify(BinaryExpr &node)
	{
		if(node.isAssignment())
		{
			VariableDecl* var_decl = cast<VariableDecl>(ResolvedSymbol::get(node.left));
			std::wstring name = var_decl->name->toString();

			// WRITE_CONST
			if(var_decl->is_const)
				LOG_MESSAGE(WRITE_CONST, ASTNodeHelper::getOwnerAnnotationAttachPoint(node), _var_id = name);

			// UNINIT_REF
			SemanticVerificationVariableDeclContext_HasBeenInit::set(var_decl, NULL);
		}

		revisit(node);
	}

	void verify(VariableDecl &node)
	{
		// UNINIT_REF
		if(ASTNodeHelper::isOwnedByFunction(node) && ASTNodeHelper::isOwnedByBlock(node) && !!node.initializer)
			SemanticVerificationVariableDeclContext_HasBeenInit::get_instance(&node);

		revisit(node);
	}

	void verify(BranchStmt &node)
	{
		if(node.opcode == BranchStmt::OpCode::RETURN)
		{
			FunctionDecl* func_decl = ASTNodeHelper::getOwnerFunction(node);
			TypeSpecifier* return_param_type = func_decl->type;
			TypeSpecifier* return_arg_type = cast<TypeSpecifier>(ResolvedType::get(node.result));

			if(ASTNodeHelper::compareTypeSpecifier(return_param_type, return_arg_type))
			{
				ASTNode* attach_point = ASTNodeHelper::getOwnerAnnotationAttachPoint(node);

				// UNEXPECTED_RETURN_VALUE
				if(ASTNodeHelper::isVoidType(return_param_type) && !ASTNodeHelper::isVoidType(return_arg_type))
					LOG_MESSAGE(UNEXPECTED_RETURN_VALUE, attach_point);

				// MISSING_RETURN_VALUE
				if(!ASTNodeHelper::isVoidType(return_param_type) && ASTNodeHelper::isVoidType(return_arg_type))
					LOG_MESSAGE(MISSING_RETURN_VALUE, attach_point, _type = return_param_type->toString());
			}
		}
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR1_H_ */
