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
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/tree/visitor/NameManglingVisitor.h"
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
// UNEXPECTED_VARIADIC_TEMPLATE_ARG
// EXCEED_PARAM_LIMIT
// EXCEED_TEMPLATE_PARAM_LIMIT

// WARNINGS:
// ====================================
// DEAD_CODE

namespace zillians { namespace language { namespace stage { namespace visitor {

struct SemanticVerificationStageVisitor0 : public GenericDoubleVisitor
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

	void verify(Package& node)
	{
		verifyDupeName(&node);
		cleanup.push_back([&](){
			SemanticVerificationScopeContext_NameSet::unbind(&node);
		});

		// PACKAGE_NAME_COLLIDE_PARENT
		if(node.parent && isa<Package>(node.parent)
				&& node.id->toString() == cast<Package>(node.parent)->id->toString())
		{
			LOG_MESSAGE(PACKAGE_NAME_COLLIDE_PARENT, &node, _package_id = node.id->toString());
		}

		revisit(node);
	}

#if 0
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

	void verify(TemplatedIdentifier& node)
	{
		std::set<std::wstring> name_set;
		size_t n = 0;
		foreach(i, node.templated_type_list)
		{
			switch(node.type)
			{
			case TemplatedIdentifier::Usage::FORMAL_PARAMETER:
				{
					std::wstring name = cast<TypenameDecl>(*i)->name->toString();

					// DUPE_NAME
					if(name_set.find(name) == name_set.end())
						name_set.insert(name);
					else
						LOG_MESSAGE(DUPE_NAME, &node, _id = name);

//					// UNEXPECTED_VARIADIC_TEMPLATE_PARAM
//					if(name == L"..." && !is_end_of_foreach(i, node.templated_type_list))
//						LOG_MESSAGE(UNEXPECTED_VARIADIC_TEMPLATE_PARAM, &node);
				}
				break;
			case TemplatedIdentifier::Usage::ACTUAL_ARGUMENT:
//
//				// UNEXPECTED_VARIADIC_TEMPLATE_ARG
//				if(isEllipsis(cast<TypeSpecifier>(*i)) && !is_end_of_foreach(i, node.templated_type_list))
//					LOG_MESSAGE(UNEXPECTED_VARIADIC_TEMPLATE_ARG, &node);
//
				break;
			}

			n++;
		}

		// EXCEED_TEMPLATE_PARAM_LIMIT
		if(n>getConfigurationContext().max_template_arg_param_count)
			LOG_MESSAGE(EXCEED_TEMPLATE_PARAM_LIMIT, &node);

		revisit(node);
	}

	void verify(BinaryExpr& node)
	{
		// WRITE_RVALUE
		if(node.isAssignment() && node.left->isRValue())
			LOG_MESSAGE(WRITE_RVALUE, &node);

		revisit(node);
	}

	void verify(Statement& node)
	{
		// DEAD_CODE
		verifyDeadCode(&node);

		revisit(node);
	}

	void verify(BranchStmt& node)
	{
		// MISSING_BREAK_TARGET
		// MISSING_CONTINUE_TARGET
		if(isBreakOrContinue(&node) && !ASTNodeHelper::getOwner<IterativeStmt>(&node))
			switch(node.opcode)
			{
			case BranchStmt::OpCode::BREAK:    LOG_MESSAGE(MISSING_BREAK_TARGET, &node); break;
			case BranchStmt::OpCode::CONTINUE: LOG_MESSAGE(MISSING_CONTINUE_TARGET, &node); break;
            case BranchStmt::OpCode::RETURN:   UNREACHABLE_CODE(); break;
			}

		// DEAD_CODE (NOTE: necessary because verify (BranchStmt&) is shadowed by verify(Statement&))
		verifyDeadCode(&node);
		if(node.opcode == BranchStmt::OpCode::RETURN)
			SemanticVerificationBlockContext_HasVisitedReturn::bind(ASTNodeHelper::getOwner<Block>(&node));

		revisit(node);
	}

	void verify(Block& node)
	{
		cleanup.push_back([&](){
			SemanticVerificationBlockContext_HasVisitedReturn::unbind(&node);
		});

		revisit(node);

		// DEAD_CODE
		if(!isConditional(node.parent) && SemanticVerificationBlockContext_HasVisitedReturn::is_bound(&node))
			SemanticVerificationBlockContext_HasVisitedReturn::bind(node.parent);
	}

	void verify(VariableDecl& node)
	{
		verifyDupeName(&node);

		// MISSING_STATIC_INIT
		if(!isa<EnumDecl>(node.parent) && !isa<FunctionDecl>(node.parent) && node.is_static && !node.initializer)
			LOG_MESSAGE(MISSING_STATIC_INIT, &node);

		revisit(node);
	}

	void verify(FunctionDecl& node)
	{
		verifyDupeName(&node);
		cleanup.push_back([&](){
			SemanticVerificationScopeContext_NameSet::unbind(&node);
		});

		std::wstring name = node.name->toString();

		// INCOMPLETE_FUNC
		if(!node.block && !ASTNodeHelper::hasNativeLinkage(&node))
			LOG_MESSAGE(INCOMPLETE_FUNC, &node, _func_id = name);

		bool has_visited_optional_param = false;
		size_t n = 0;
		foreach(i, node.parameters)
		{
			// MISSING_PARAM_INIT
			if(cast<VariableDecl>(*i)->initializer)
				has_visited_optional_param = true;
			else if(has_visited_optional_param)
				LOG_MESSAGE(MISSING_PARAM_INIT, &node, _param_index = (int)n+1, _func_id = name);

			// UNEXPECTED_VARIADIC_PARAM
			if(isEllipsis(cast<VariableDecl>(*i)->type) && !is_end_of_foreach(i, node.parameters))
				LOG_MESSAGE(UNEXPECTED_VARIADIC_PARAM, &node);

			n++;
		}

		// EXCEED_PARAM_LIMIT
		if(n>getConfigurationContext().max_param_count)
			LOG_MESSAGE(EXCEED_PARAM_LIMIT, &node);

		revisit(node);
	}

	void verify(ClassDecl& node)
	{
		verifyDupeName(&node);
		cleanup.push_back([&](){
			SemanticVerificationScopeContext_NameSet::unbind(&node);
		});

		revisit(node);
	}

private:
	static bool isEllipsis(TypeSpecifier* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return node->type == TypeSpecifier::ReferredType::PRIMITIVE
				&& node->referred.primitive == PrimitiveType::VARIADIC_ELLIPSIS;
	}

	static void verifyDupeName(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");

		// DUPE_NAME
		std::wstring name;
		if(isa<Declaration>(node))
			name = cast<Declaration>(node)->name->toString();
		else if(isa<Package>(node))
			name = cast<Package>(node)->id->toString();
		else
			BOOST_ASSERT(false && "reaching unreachable code");
		ASTNode* owner = ASTNodeHelper::getOwnerNamedScope(node);
		if(owner)
		{
			SemanticVerificationScopeContext_NameSet* context =
					SemanticVerificationScopeContext_NameSet::bind(owner);
			if(context->names.find(name) == context->names.end())
				context->names.insert(name);
			else
				LOG_MESSAGE(DUPE_NAME, node, _id = name);
		}
	}

	static void verifyDeadCode(Statement* node)
	{
		BOOST_ASSERT(node && "null pointer exception");

		// DEAD_CODE
		if(isa<Block>(node->parent) && SemanticVerificationBlockContext_HasVisitedReturn::is_bound(node->parent))
			LOG_MESSAGE(DEAD_CODE, node);
	}

	static bool isConditional(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<SelectionStmt>(node) || isa<IterativeStmt>(node);
	}

	static bool isBreakOrContinue(BranchStmt* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return node->opcode == BranchStmt::OpCode::BREAK
				|| node->opcode == BranchStmt::OpCode::CONTINUE;
	}

public:
	void applyCleanup()
	{
		foreach(i, cleanup)
			(*i)();
		cleanup.clear();
	}

private:
	std::vector<std::function<void()>> cleanup;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR0_H_ */
