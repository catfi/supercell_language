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

	void verify(BinaryExpr &node)
	{
		// WRITE_RVALUE
		if(node.isAssignment() && node.left->isRValue() && ASTNodeHelper::isOwnedByStatement(node))
		{
			ASTNode* owner = ASTNodeHelper::getOwnerStatement(*node.left->parent);
			BOOST_ASSERT(!!owner);
			LOG_MESSAGE(WRITE_RVALUE, *owner);
		}
	}

	void verify(BranchStmt &node)
	{
		// MISSING_BREAK_TARGET
		// MISSING_CONTINUE_TARGET
		if(node.isBreakOrContinue() && !ASTNodeHelper::isOwnedByIterativeStmt(node))
			switch(node.opcode)
			{
			case BranchStmt::OpCode::BREAK:    LOG_MESSAGE(MISSING_BREAK_TARGET, node); break;
			case BranchStmt::OpCode::CONTINUE: LOG_MESSAGE(MISSING_CONTINUE_TARGET, node); break;
			default:                           BOOST_ASSERT(false && "reaching unreachable code"); break;
			}
	}

	void verify(Declaration &node)
	{
		// UNDEFINED_REF -- function has no body ??
		// DUPE_NAME
		revisit(node);
	}

	void verify(VariableDecl &node)
	{
		// MISSING_STATIC_INIT
		if(node.is_static && !node.initializer)
			LOG_MESSAGE(MISSING_STATIC_INIT, node);
	}

	void verify(FunctionDecl &node)
	{
		// UNDEFINED_REF -- function has no body ??
		if(!node.block) // NOTE: must have @native annotation ??
		{
		}

		bool DUPE_NAME                 = false;
		bool MISSING_PARAM_INIT        = false;
		bool UNEXPECTED_VARIADIC_PARAM = false;
		bool EXCEED_PARAM_LIMIT        = false;
		std::set<std::wstring> name_set;
		std::wstring dupe_name_string;
		bool visited_optional_param = false;
		int missing_param_init_index = 0;
		size_t param_count = 0;
		foreach(i, node.parameters)
		{
			// DUPE_NAME
			std::wstring name = cast<VariableDecl>(*i)->name->toString();
			if(name_set.find(name) != name_set.end())
			{
				DUPE_NAME = true;
				dupe_name_string = name;
			}
			else
				name_set.insert(name);

			// MISSING_PARAM_INIT
			if(!!cast<VariableDecl>(*i)->initializer)
				visited_optional_param = true;
			else if(visited_optional_param)
			{
				MISSING_PARAM_INIT = true;
				missing_param_init_index = static_cast<int>(param_count)+1;
			}

			// UNEXPECTED_VARIADIC_PARAM
			if(cast<VariableDecl>(*i)->type->type == TypeSpecifier::ReferredType::PRIMITIVE &&
					cast<VariableDecl>(*i)->type->referred.primitive == PrimitiveType::VARIADIC_ELLIPSIS &&
					!is_end_of_foreach(i, node.parameters))
							UNEXPECTED_VARIADIC_PARAM = true;

			// EXCEED_PARAM_LIMIT
			param_count++;
			if(param_count>getConfigurationContext().max_param_count)
				EXCEED_PARAM_LIMIT = true;
		}
		if(DUPE_NAME)                 LOG_MESSAGE(DUPE_NAME,                 node, _ID = dupe_name_string);
		if(MISSING_PARAM_INIT)        LOG_MESSAGE(MISSING_PARAM_INIT,        node, _PARAM_INDEX = missing_param_init_index, _FUNC = node.name->toString());
		if(UNEXPECTED_VARIADIC_PARAM) LOG_MESSAGE(UNEXPECTED_VARIADIC_PARAM, node);
		if(EXCEED_PARAM_LIMIT)        LOG_MESSAGE(EXCEED_PARAM_LIMIT,        node);
		revisit(node);
	}

	void verify(TemplatedIdentifier &node)
	{
		bool DUPE_NAME                          = false;
		bool UNEXPECTED_VARIADIC_TEMPLATE_PARAM = false;
		bool EXCEED_TEMPLATE_PARAM_LIMIT        = false;
		std::set<std::wstring> name_set;
		std::wstring dupe_name_string;
		size_t arg_param_count = 0;
		foreach(i, node.templated_type_list)
		{
			switch(node.type)
			{
			case TemplatedIdentifier::Usage::FORMAL_PARAMETER:
				{
					// DUPE_NAME
					std::wstring name = cast<Identifier>(*i)->toString();
					if(name_set.find(name) != name_set.end())
					{
						DUPE_NAME = true;
						dupe_name_string = name;
					}
					else
						name_set.insert(name);

					// UNEXPECTED_VARIADIC_TEMPLATE_PARAM
					if(name == L"..." && !is_end_of_foreach(i, node.templated_type_list))
						UNEXPECTED_VARIADIC_TEMPLATE_PARAM = true;
				}
				break;
			case TemplatedIdentifier::Usage::ACTUAL_ARGUMENT:
				// NOTE: no need to check DUPE_NAME
				// NOTE: no need to check UNEXPECTED_VARIADIC_TEMPLATE_PARAM
				break;
			}

			// EXCEED_TEMPLATE_PARAM_LIMIT
			arg_param_count++;
			if(arg_param_count>getConfigurationContext().max_template_arg_param_count)
				EXCEED_TEMPLATE_PARAM_LIMIT = true;
		}
		if(DUPE_NAME || UNEXPECTED_VARIADIC_TEMPLATE_PARAM || EXCEED_TEMPLATE_PARAM_LIMIT)
		{
			ASTNode* owner = NULL;
			if(ASTNodeHelper::isOwnedByFunction(node))   owner = ASTNodeHelper::getOwnerFunction(node);
			else if(ASTNodeHelper::isOwnedByClass(node)) owner = ASTNodeHelper::getOwnerClass(node);
			else BOOST_ASSERT(false && "reaching unreachable code");
			if(DUPE_NAME)                          LOG_MESSAGE(DUPE_NAME,                          *owner, _ID = dupe_name_string);
			if(UNEXPECTED_VARIADIC_TEMPLATE_PARAM) LOG_MESSAGE(UNEXPECTED_VARIADIC_TEMPLATE_PARAM, *owner);
			if(EXCEED_TEMPLATE_PARAM_LIMIT)        LOG_MESSAGE(EXCEED_TEMPLATE_PARAM_LIMIT,        *owner);
		}
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR0_H_ */
