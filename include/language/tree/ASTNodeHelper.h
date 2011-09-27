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
#include "language/context/ResolverContext.h"
#include "language/tree/visitor/general/NodeInfoVisitor.h"

namespace zillians { namespace language { namespace tree {

struct ASTNodeHelper
{
	static FunctionType* createFunctionTypeFromFunctionDecl(FunctionDecl* function_decl)
	{
		FunctionType* function_type = new FunctionType();

		function_type->return_type = function_decl->type;

		if(isa<TemplatedIdentifier>(function_decl->name))
		{
			TemplatedIdentifier* templated_name = cast<TemplatedIdentifier>(function_decl->name);
			foreach(i, templated_name->templated_type_list)
			{
				Identifier* templated_parameter = cast<Identifier>(*i);
				function_type->appendTemplateParameter(templated_parameter);
			}
		}

		foreach(i, function_decl->parameters)
		{
			function_type->appendParameterType((*i)->type);
		}

		return function_type;
	}

	static bool compareFunctionType(FunctionType* a, FunctionType* b)
	{
		if(a->argument_types.size() != b->argument_types.size()) return false;
		if(a->templated_parameters.size() != b->templated_parameters.size()) return false;

		auto it_a = make_begin(a->argument_types), it_a_end = make_end(a->argument_types);
		auto it_b = make_begin(b->argument_types), it_b_end = make_end(b->argument_types);

		for(; it_a != it_a_end && it_b != it_b_end; ++it_a, ++it_b)
		{
			if(!compareTypeSpecifier(*it_a, *it_b))
				return false;
		}

		if(!compareTypeSpecifier(a->return_type, b->return_type))
			return false;

		return true;
	}

	static bool compareTypeSpecifier(TypeSpecifier* a, TypeSpecifier* b)
	{
		if(a->type != b->type)
			return false;

		if(a->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
		{
			return compareFunctionType(a->referred.function_type, b->referred.function_type);
		}
		else if(a->type == TypeSpecifier::ReferredType::PRIMITIVE)
		{
			return (a->referred.primitive == b->referred.primitive);
		}
		else if(a->type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			ASTNode* resolved_type_a = ResolvedType::get(a);
			ASTNode* resolved_type_b = ResolvedType::get(b);

			if(!resolved_type_a || !resolved_type_b)
				return false;

			if(isa<TypeSpecifier>(resolved_type_a) && isa<TypeSpecifier>(resolved_type_b))
				return compareTypeSpecifier(cast<TypeSpecifier>(resolved_type_a), cast<TypeSpecifier>(resolved_type_b));
			else if(isa<ClassDecl>(resolved_type_a) && isa<ClassDecl>(resolved_type_b))
				return (resolved_type_a == resolved_type_b);
			else if(isa<InterfaceDecl>(resolved_type_a) && isa<InterfaceDecl>(resolved_type_b))
				return (resolved_type_a == resolved_type_b);
			else if(isa<EnumDecl>(resolved_type_a) && isa<EnumDecl>(resolved_type_b))
				return (resolved_type_a == resolved_type_b);
		}

		return false;
	}

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

	static SelectionStmt* getOwnerSelectionStmt(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return NULL;
			if(isa<SelectionStmt>(current))
				return cast<SelectionStmt>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedBySelectionStmt(ASTNode& node)
	{
		return (getOwnerSelectionStmt(node) != NULL);
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

	static Annotation* getOwnerAnnotation(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<Statement>(current))
				return NULL;
			if(isa<Declaration>(current))
				return NULL;
			if(isa<Package>(current))
				return NULL;
			if(isa<Annotation>(current))
				return cast<Annotation>(current);

			current = current->parent;
		}
		return NULL;
	}

	static bool isOwnedByAnnotation(ASTNode& node)
	{
		return (getOwnerAnnotation(node) != NULL);
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

	static ASTNode* getNearestAnnotatableOwner(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<Statement>(current))
				return cast<Statement>(current);
			else if(isa<Declaration>(current))
				return cast<Declaration>(current);

			current = current->parent;
		}
		return NULL;
	}

	static Annotation* getAnnotationFromTag(ASTNode& node, std::wstring tag)
	{
		Annotations* annotations = NULL;
		if(isa<Declaration>(&node))
			annotations = cast<Declaration>(&node)->annotations;
		else if(isa<Statement>(&node))
			annotations = cast<Statement>(&node)->annotations;
		else
			return NULL;
		if(!annotations)
			return NULL;
		foreach(i, annotations->annotation_list)
			if((*i)->name->toString() == tag)
				return *i;
		return NULL;
	}

	static bool hasAnnotationTag(ASTNode& node, std::wstring tag)
	{
		return (getAnnotationFromTag(node, tag) != NULL);
	}

	static ASTNode* getOwnerScope(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<FunctionDecl>(current))
				return cast<FunctionDecl>(current);
			else if(isa<ClassDecl>(current))
				return cast<ClassDecl>(current);
			else if(isa<Package>(current))
				return cast<Package>(current);
			else if(isa<InterfaceDecl>(current))
				return cast<InterfaceDecl>(current);

			current = current->parent;
		}
		return NULL;
	}

	static ASTNode* getOwnerBlock(ASTNode& node)
	{
		if(!node.parent)
			return NULL;

		ASTNode* current = node.parent;
		while(current)
		{
			if(isa<Package>(current))
				return NULL;
			if(isa<Block>(current))
				return cast<Block>(current);

			current = current->parent;
		}
		return NULL;
	}

public:
	static std::wstring nodeName(ASTNode* node)
	{
		static tree::visitor::NodeInfoVisitor v(1);
		v.reset();
		v.visit(*node);
		return v.stream.str();
	}
private:
	ASTNodeHelper() { }
	~ASTNodeHelper() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_ */
