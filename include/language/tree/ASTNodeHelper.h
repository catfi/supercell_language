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
#include "language/tree/visitor/general/NodeInfoVisitor.h"

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

	static bool extends(ClassDecl& derived, ClassDecl& base)
	{
		ASTNode* target = &derived;
		do
		{
			if(target == &base)
				return true;
			if(!isa<ClassDecl>(target))
				return false;
		} while(!!(target = ResolvedType::get(cast<ClassDecl>(target)->base)));
		return false;
	}

	template<class T>
	static T* owner(ASTNode& node)
	{
		for(ASTNode* p = node.parent; !!p && !isa<Package>(p); p = p->parent)
			if(isa<T>(p))
				return cast<T>(p);
		return NULL;
	}

	static ASTNode* ownerNamedScope(ASTNode& node)
	{
		for(ASTNode* p = node.parent; !!p; p = p->parent)
			if(_is_named_scope(p))
				return p;
		return NULL;
	}

	static bool isDirectlyOwnedByPackage(ASTNode& node) { return !!getDirectOwnerPackage(node); }
	static Package* getDirectOwnerPackage(ASTNode& node)
	{
		for(ASTNode* p = node.parent; !!p; p = p->parent)
		{
			if(isa<FunctionDecl>(p))  return NULL;
			if(isa<ClassDecl>(p))     return NULL;
			if(isa<InterfaceDecl>(p)) return NULL;
			if(isa<Package>(p))       return cast<Package>(p);
		}
		return NULL;
	}

	static Annotation* findAnnotation(ASTNode& node, std::wstring tag)
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

	static ASTNode* attachPoint(ASTNode& node)
	{
		ASTNode* target = NULL;
		ASTNode* new_target = &node;
		do
		{
			target = new_target;
			if(!(new_target = _split_reference_attach_point(*target))) // NOTE: check split-reference first
				break;
			new_target = _owner_debug_annotation_attach_point(*new_target);
		} while(!!new_target && new_target != target);
		return target;
	}

	static bool isFuncParam(VariableDecl* var_decl)
	{
		return isa<FunctionDecl>(var_decl->parent);
	}

	static std::wstring nodeName(ASTNode* node)
	{
		static tree::visitor::NodeInfoVisitor v(1);
		v.reset();
		v.visit(*node);
		return v.stream.str();
	}

private:
	static bool _is_named_scope(ASTNode* node)
	{
		return isa<FunctionDecl>(node)
				|| isa<ClassDecl>(node)
				|| isa<InterfaceDecl>(node)
				|| isa<Package>(node);
	}

	static bool _is_debug_annotation_attach_point(ASTNode* node)
	{
		return isa<Statement>(node)
				|| (isa<Declaration>(node)
						&& (!isa<VariableDecl>(node) || !isFuncParam(cast<VariableDecl>(node)))); // exclude function parameters
	}

	static ASTNode* _owner_debug_annotation_attach_point(ASTNode& node)
	{
		for(ASTNode* p = &node; !!p && !isa<Package>(p); p = p->parent)
			if(_is_debug_annotation_attach_point(p))
				return p;
		return NULL;
	}

	static ASTNode* _split_reference_attach_point(ASTNode& node)
	{
		ASTNode* target = NULL;
		ASTNode* new_target = &node;
		do
		{
			target = new_target;
			new_target = SplitReferenceContext::get(target);
		} while(!!new_target && new_target != target);
		return target;
	}

	ASTNodeHelper() { }
	~ASTNodeHelper() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_ */
