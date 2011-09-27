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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR0_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR0_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/tree/visitor/general/NameManglingVisitor.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/logging/StringTable.h"
#include "language/logging/LoggerWrapper.h"
#include "language/context/ParserContext.h"
#include "language/context/ConfigurationContext.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/stage/verifier/context/SemanticVerificationContext.h"
#include <set>
#include <string>

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NameManglingVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct SemanticVerificationStageVisitor0 : GenericDoubleVisitor
{
	CREATE_INVOKER(verifyInvoker, verify)

	SemanticVerificationStageVisitor0()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(verifyInvoker)
	}

	void verify(ASTNode& node)
	{
		revisit(node);
	}

#if 0
	void verify(Package& node)
	{
		revisit(node);

		// CHECK: the package name should not conflict with its parent and sibling
		if(node.parent && isa<Package>(node.parent))
		{
			if(node.id->toString() == cast<Package>(node.parent)->id->toString())
			{
				// TODO log semantic check error
			}
		}
	}

	void verify(ClassDecl& node)
	{
		// CHECK: variable name and function name should not conflict with each other
		std::map<std::wstring, ASTNode*> all_members;

		foreach(i, node.member_functions)
		{
			std::wstring name = (*i)->name->toString();
			std::map<std::wstring, ASTNode*>::iterator it = all_members.find(name);
			if(it != all_members.end())
			{
				// CHECK: if all member function with same name has different function signature
				// TODO note that even if we don't have complete type information, we can still check by word
			}
			else
			{
				// insert all function names into all_members
				all_members.insert(std::make_pair(name, (ASTNode*)(*i)));
			}
		}

		foreach(i, node.member_variables)
		{
			std::wstring name = (*i)->name->toString();
			std::map<std::wstring, ASTNode*>::iterator it = all_members.find(name);
			if(it != all_members.end())
			{
				// TODO log semantic check error
			}
			else
			{
				all_members.insert(std::make_pair(name, (ASTNode*)(*i)));
			}
		}

		// CHECK: if all member variable or function is marked is_member = true
		foreach(i, node.member_variables)
		{
			if(!(*i)->is_member)
			{
				// TODO log semantic check error
			}
		}

		foreach(i, node.member_functions)
		{
			if(!(*i)->is_member)
			{
				// TODO log semantic check error
			}
		}

		revisit(node);
	}

	void verify(InterfaceDecl& node)
	{
		// CHECK: all member function in interface must not have private scope specifier
	}
#endif

	// CHECKS IN SEMANTIC VERIFICATION STAGE 0

	// ERRORS:
	// ====================================
	// INCOMPLETE_FUNC
	// DUPE_NAME
	// WRITE_RVALUE
	// MISSING_STATIC_INIT
	// MISSING_BREAK_TARGET
	// MISSING_CONTINUE_TARGET
	// MISSING_PARAM_INIT
	// UNEXPECTED_VARIADIC_PARAM
	// UNEXPECTED_VARIADIC_TEMPLATE_PARAM
	// EXCEED_PARAM_LIMIT
	// EXCEED_TEMPLATE_PARAM_LIMIT

	// WARNINGS:
	// ====================================
	// DEAD_CODE

	void verify_DUPE_NAME(Declaration &node)
	{
		// DUPE_NAME
		std::wstring name = node.name->toString();
		SemanticVerificationScopeContext_NameSet* owner_context =
				SemanticVerificationScopeContext_NameSet::get_instance(ASTNodeHelper::getOwnerScope(node));
		if(owner_context->names.find(name) == owner_context->names.end())
			owner_context->names.insert(name);
		else
		{
			ASTNode* owner = node.parent;
			if(isa<FunctionDecl>(owner) || isa<Statement>(owner))
				LOG_MESSAGE(DUPE_NAME, owner, _id = name);
			else
				LOG_MESSAGE(DUPE_NAME, &node, _id = name);
		}
	}

	void verify_DEAD_CODE(Statement &node)
	{
		// DEAD_CODE
		ASTNode* owner = node.parent;
		if(isa<Block>(owner) && !!SemanticVerificationBlockContext_HasVisitedReturn::get(owner))
			LOG_MESSAGE(DEAD_CODE, &node);
	}

	void verify(BinaryExpr &node)
	{
		// WRITE_RVALUE
		if(node.isAssignment() && node.left->isRValue())
			LOG_MESSAGE(WRITE_RVALUE, ASTNodeHelper::getOwnerStatement(*node.left->parent));
	}

	void verify(BranchStmt &node)
	{
		// MISSING_BREAK_TARGET
		// MISSING_CONTINUE_TARGET
		if(node.isBreakOrContinue() && !ASTNodeHelper::isOwnedByIterativeStmt(node))
			switch(node.opcode)
			{
			case BranchStmt::OpCode::BREAK:    LOG_MESSAGE(MISSING_BREAK_TARGET, &node); break;
			case BranchStmt::OpCode::CONTINUE: LOG_MESSAGE(MISSING_CONTINUE_TARGET, &node); break;
			}

		// DEAD_CODE
		verify_DEAD_CODE(node);
		if(node.opcode == BranchStmt::OpCode::RETURN)
			SemanticVerificationBlockContext_HasVisitedReturn::get_instance(ASTNodeHelper::getOwnerBlock(node));
	}

	void verify(Statement &node)
	{
		// DEAD_CODE
		verify_DEAD_CODE(node);

		revisit(node);
	}

	void verify(Block &node)
	{
		revisit(node);

		// DEAD_CODE
		ASTNode* owner = node.parent;
		if(!isa<SelectionStmt>(owner) && !!SemanticVerificationBlockContext_HasVisitedReturn::get(&node))
			SemanticVerificationBlockContext_HasVisitedReturn::get_instance(owner);
	}

	void verify(Declaration &node)
	{
		revisit(node);
	}

	void verify(VariableDecl &node)
	{
		verify_DUPE_NAME(*cast<Declaration>(&node));

		// MISSING_STATIC_INIT
		if(node.is_static && !node.initializer)
			LOG_MESSAGE(MISSING_STATIC_INIT, &node);
	}

	void verify(FunctionDecl &node)
	{
		verify_DUPE_NAME(*cast<Declaration>(&node));

		// INCOMPLETE_FUNC
		if(!node.block && !ASTNodeHelper::hasAnnotationTag(node, L"native"))
			LOG_MESSAGE(INCOMPLETE_FUNC, &node, _id = node.name->toString());

		std::set<std::wstring> name_set;
		bool visited_optional_param = false;
		size_t n = 0;
		foreach(i, node.parameters)
		{
#if 0 // NOTE: already handled by "::verify_DUPE_NAME"
			std::wstring name = cast<VariableDecl>(*i)->name->toString();

			// DUPE_NAME
			if(name_set.find(name) == name_set.end())
				name_set.insert(name);
			else
				LOG_MESSAGE(DUPE_NAME, &node, _id = name);
#endif

			// MISSING_PARAM_INIT
			if(!!cast<VariableDecl>(*i)->initializer)
				visited_optional_param = true;
			else if(visited_optional_param)
				LOG_MESSAGE(MISSING_PARAM_INIT, &node, _param_index = (int)n+1, _func_id = node.name->toString());

			// UNEXPECTED_VARIADIC_PARAM
			if(cast<VariableDecl>(*i)->type->type == TypeSpecifier::ReferredType::PRIMITIVE &&
					cast<VariableDecl>(*i)->type->referred.primitive == PrimitiveType::VARIADIC_ELLIPSIS &&
					!is_end_of_foreach(i, node.parameters))
						LOG_MESSAGE(UNEXPECTED_VARIADIC_PARAM, &node);

			n++;
		}

		// EXCEED_PARAM_LIMIT
		if(n>getConfigurationContext().max_param_count)
			LOG_MESSAGE(EXCEED_PARAM_LIMIT, &node);

		revisit(node);
	}

	void verify(ClassDecl &node)
	{
		verify_DUPE_NAME(*cast<Declaration>(&node));

		revisit(node);
	}

	void verify(TemplatedIdentifier &node)
	{
		ASTNode* owner = node.parent;
		if(isa<FunctionDecl>(owner) || isa<ClassDecl>(owner))
		{
			std::set<std::wstring> name_set;
			size_t n = 0;
			foreach(i, node.templated_type_list)
			{
				switch(node.type)
				{
				case TemplatedIdentifier::Usage::FORMAL_PARAMETER:
					{
						std::wstring name = cast<Identifier>(*i)->toString();

						// DUPE_NAME
						if(name_set.find(name) == name_set.end())
							name_set.insert(name);
						else
							LOG_MESSAGE(DUPE_NAME, owner, _id = name);

						// UNEXPECTED_VARIADIC_TEMPLATE_PARAM
						if(name == L"..." && !is_end_of_foreach(i, node.templated_type_list))
							LOG_MESSAGE(UNEXPECTED_VARIADIC_TEMPLATE_PARAM, owner);
					}
					break;
				case TemplatedIdentifier::Usage::ACTUAL_ARGUMENT:
					// NOTE: no need to check DUPE_NAME
					// NOTE: no need to check UNEXPECTED_VARIADIC_TEMPLATE_PARAM (not possible until s1)
					break;
				}

				n++;
			}

			// EXCEED_TEMPLATE_PARAM_LIMIT
			if(n>getConfigurationContext().max_template_arg_param_count)
				LOG_MESSAGE(EXCEED_TEMPLATE_PARAM_LIMIT, owner);
		}
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR0_H_ */
