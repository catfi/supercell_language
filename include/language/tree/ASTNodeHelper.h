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

#ifndef ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_
#define ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_

#include "language/tree/ASTNodeFactory.h"

namespace zillians { namespace language { namespace tree {

struct ASTNodeHelper
{
	static IterativeStmt* getOwnerIterativeStmt(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<IterativeStmt>(current))
				return cast<IterativeStmt>(current);
			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByIterativeStmt(ASTNode& node)
	{
		return (getOwnerIterativeStmt(node) != NULL);
	}

	static Expression* getOwnerExpression(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<Expression>(current))
				return cast<Expression>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByExpression(ASTNode& node)
	{
		return (getOwnerExpression(node) != NULL);
	}

	static Statement* getOwnerStatement(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<VariableDecl>(current))
				return NULL;
			if(isa<Statement>(current))
				return cast<Statement>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByStatement(ASTNode& node)
	{
		return (getOwnerStatement(node) != NULL);
	}

	static Package* getOwnerPackage(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<ClassDecl>(current))
				return NULL;
			if(isa<InterfaceDecl>(current))
				return NULL;
			if(isa<Package>(current))
				return cast<Package>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByPackage(ASTNode& node)
	{
		return (getOwnerPackage(node) != NULL);
	}

	static ClassDecl* getOwnerClass(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<InterfaceDecl>(current))
				return NULL;
			if(isa<Package>(current))
				return NULL;
			if(isa<ClassDecl>(current))
				return cast<ClassDecl>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByClass(ASTNode& node)
	{
		return (getOwnerClass(node) != NULL);
	}

	static InterfaceDecl* getOwnerInterface(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<Package>(current))
				return NULL;
			if(isa<ClassDecl>(current))
				return NULL;
			if(isa<InterfaceDecl>(current))
				return cast<InterfaceDecl>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByInterface(ASTNode& node)
	{
		return (getOwnerInterface(node) != NULL);
	}

	static FunctionDecl* getOwnerFunction(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<InterfaceDecl>(current))
				return NULL;
			if(isa<Package>(current))
				return NULL;
			if(isa<ClassDecl>(current))
				return NULL;
			if(isa<FunctionDecl>(current))
				return cast<FunctionDecl>(current);
			current = current->parent;
		}

		return NULL;
	}

	static bool isOwnedByFunction(ASTNode& node)
	{
		return (getOwnerFunction(node) != NULL);
	}

private:
	ASTNodeHelper() { }
	~ASTNodeHelper() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_ */
