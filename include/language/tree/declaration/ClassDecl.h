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
#include "language/tree/declaration/Declaration.h"
#include "language/tree/declaration/FunctionDecl.h"
#include "language/tree/declaration/VariableDecl.h"
#include "language/tree/declaration/TypedefDecl.h"

namespace zillians { namespace language { namespace tree {

struct ClassDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ClassDecl, (ClassDecl)(Declaration)(ASTNode));

	explicit ClassDecl(Identifier* name) : name(name)
	{
		name->parent = this;
	}

	void appendFunction(FunctionDecl* func)
	{
		func->parent = this;
		member_functions.push_back(func);
	}

	void appendVariable(VariableDecl* var)
	{
		var->parent = this;
		member_variables.push_back(var);
	}

	void setBase(ASTNode* extends_from)
	{
		extends_from->parent = this;
		base = extends_from;
	}

	void addInterface(ASTNode* interface)
	{
		interface->parent = this;
		implements.push_back(interface);
	}

	Identifier* name;
	ASTNode* base;
	std::vector<ASTNode*> implements;
	std::vector<FunctionDecl*> member_functions;
	std::vector<VariableDecl*> member_variables;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_ */
