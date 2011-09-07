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

	void verify(FunctionDecl& node)
	{
		// CHECK:
	}

	void verify(VariableDecl& node)
	{

	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_SEMANTICVERIFICATIONSTAGEVISITOR0_H_ */
