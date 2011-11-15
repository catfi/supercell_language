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
#include "language/context/ParserContext.h"
#include "language/context/ResolverContext.h"
#include "language/context/TransformerContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/tree/visitor/NodeInfoVisitor.h"

namespace zillians { namespace language { namespace tree {

struct ASTNodeHelper
{
	static void propogateSourceInfo(ASTNode& to, ASTNode& from)
	{
		stage::SourceInfoContext* to_src_info = to.get<stage::SourceInfoContext>();
		stage::SourceInfoContext* from_src_info = from.get<stage::SourceInfoContext>();

		BOOST_ASSERT(to_src_info == NULL && "invalid propagating source info propagation");

		to.set<stage::SourceInfoContext>(new stage::SourceInfoContext(*from_src_info));
	}

	static FunctionType* createFunctionTypeFromFunctionDecl(FunctionDecl* node)
	{
		BOOST_ASSERT(node && "null pointer exception");

		FunctionType* function_type = new FunctionType();

		function_type->return_type = node->type;

		if(isa<TemplatedIdentifier>(node->name))
		{
			TemplatedIdentifier* templated_name = cast<TemplatedIdentifier>(node->name);
			foreach(i, templated_name->templated_type_list)
			{
				Identifier* templated_parameter = cast<Identifier>(*i);
				function_type->appendTemplateParameter(templated_parameter);
			}
		}

		foreach(i, node->parameters)
		{
			function_type->appendParameterType((*i)->type);
		}

		return function_type;
	}

	static bool compareFunctionType(FunctionType* a, FunctionType* b)
	{
		BOOST_ASSERT(a && b && "null pointer exception");

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
		BOOST_ASSERT(a && b && "null pointer exception");

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

	static bool isInheritedFrom(ClassDecl* node_derived, ClassDecl* node_base)
	{
		BOOST_ASSERT(node_derived && node_base && "null pointer exception");
		ASTNode* current = node_derived;
		do
		{
			if(current == node_base)
				return true;
			if(!isa<ClassDecl>(current))
				return false;
		} while((current = ResolvedType::get(cast<ClassDecl>(current)->base)));
		return false;
	}

	template<class T> static bool hasOwner(ASTNode* node) { return getOwner<T>(node); }
	template<class T>
	static T* getOwner(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		for(ASTNode* p = node->parent; p && !isa<Package>(p); p = p->parent)
			if(isa<T>(p))
				return cast<T>(p);
		return NULL;
	}

	static bool hasOwnerNamedScope(ASTNode* node) { return getOwnerNamedScope(node); }
	static ASTNode* getOwnerNamedScope(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		for(ASTNode* p = node->parent; p; p = p->parent)
			if(isNamedScope(p))
				return p;
		return NULL;
	}

	static Annotation* findAnnotation(ASTNode* node, std::wstring tag)
	{
		BOOST_ASSERT(node && "null pointer exception");
		BOOST_ASSERT(!tag.empty() && "empty annotation tag");
		Annotations* annotations = NULL;
		if(isa<Declaration>(node))
			annotations = cast<Declaration>(node)->annotations;
		else if(isa<Statement>(node))
			annotations = cast<Statement>(node)->annotations;
		else
			return NULL;
		if(!annotations)
			return NULL;
		foreach(i, annotations->annotation_list)
			if((*i)->name->toString() == tag)
				return *i;
		return NULL;
	}

	static ASTNode* getDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		ASTNode* current = NULL;
		ASTNode* next = node;
		do
		{
			current = next;
			if(!(next = getSplitReferenceAttachPoint(current))) // NOTE: check split-reference first
				break;
			next = getOwnerDebugAnnotationAttachPoint(next);
		} while(next && next != current);
		return current;
	}

	static bool isFuncParam(VariableDecl* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<FunctionDecl>(node->parent);
	}

	static bool isDeclStmtVarDecl(VariableDecl* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<DeclarativeStmt>(node->parent);
	}

	static bool isRootPackage(Package* node)
	{
		return node->id->toString().empty();
	}

	static bool isInvalidDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		if(isa<VariableDecl>(node))
		{
			VariableDecl* var_decl = cast<VariableDecl>(node);
			return isFuncParam(var_decl) || isDeclStmtVarDecl(var_decl);
		}
		return false;
	}

	static std::wstring getNodeName(ASTNode* node, bool FQN = false)
	{
		BOOST_ASSERT(node && "null pointer exception");
		const int32 max_depth = 20;
		static tree::visitor::NodeInfoVisitor v(FQN ? max_depth : 1);
		v.reset();
		v.visit(*node);
		return v.stream.str();
	}

	static bool isUnspecifiedType(TypeSpecifier* node)
	{
		return node->type == TypeSpecifier::ReferredType::UNSPECIFIED;
	}

	static bool isPrimitiveType(TypeSpecifier* node)
	{
		return node->type == TypeSpecifier::ReferredType::PRIMITIVE;
	}

	static int32 getPrimitiveTypeSize(TypeSpecifier* node)
	{
		return PrimitiveType::byteSize(node->referred.primitive);
	}

private:
	static bool isNamedScope(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<FunctionDecl>(node)
				|| isa<ClassDecl>(node)
				|| isa<InterfaceDecl>(node)
				|| (isa<Package>(node) && !cast<Package>(node)->id->toString().empty());
	}

	static bool isDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return (isa<Statement>(node)
				|| isa<Declaration>(node)
				|| isa<Package>(node))
						&& !isInvalidDebugAnnotationAttachPoint(node);
	}

	static ASTNode* getSplitReferenceAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		ASTNode* current = NULL;
		ASTNode* next = node;
		do
		{
			current = next;
			next = SplitReferenceContext::get(current);
		} while(next && next != current);
		return current;
	}

	static ASTNode* getOwnerDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		for(ASTNode* p = node; p && !isa<Package>(p); p = p->parent)
			if(isDebugAnnotationAttachPoint(p))
				return p;
		return NULL;
	}

	ASTNodeHelper() { }
	~ASTNodeHelper() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_ */
