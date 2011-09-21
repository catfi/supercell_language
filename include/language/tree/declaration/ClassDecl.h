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

#ifndef ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_
#define ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_

#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/declaration/Declaration.h"
#include "language/tree/declaration/FunctionDecl.h"
#include "language/tree/declaration/VariableDecl.h"
#include "language/tree/declaration/TypedefDecl.h"

namespace zillians { namespace language { namespace tree {

struct ClassDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ClassDecl, (ClassDecl)(Declaration)(ASTNode));

	explicit ClassDecl(Identifier* name) : Declaration(name), base(NULL)
	{
		BOOST_ASSERT(name && "null class name identifier is not allowed");
	}

	void addFunction(FunctionDecl* func)
	{
		BOOST_ASSERT(name && "null member function declaration is not allowed");

		func->parent = this;
		member_functions.push_back(func);
	}

	void addVariable(VariableDecl* var)
	{
		BOOST_ASSERT(name && "null member variable declaration is not allowed");

		var->parent = this;
		member_variables.push_back(var);
	}

	void setBase(TypeSpecifier* extends_from)
	{
		BOOST_ASSERT(extends_from && "null base class is not allowed");

		extends_from->parent = this;

		if(base)
			base->parent = NULL;

		base = extends_from;
	}

	void addInterface(TypeSpecifier* interface)
	{
		BOOST_ASSERT(interface && "null interface is not allowed");

		interface->parent = this;
		implements.push_back(interface);
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Declaration)
		COMPARE_MEMBER(base)
		COMPARE_MEMBER(implements)
		COMPARE_MEMBER(member_functions)
		COMPARE_MEMBER(member_variables)
		END_COMPARE()
    }

	TypeSpecifier* base;
	std::vector<TypeSpecifier*> implements;
	std::vector<FunctionDecl*> member_functions;
	std::vector<VariableDecl*> member_variables;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_ */
