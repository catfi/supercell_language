/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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
/**
 * @date Aug 12, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"

#define INDIRECT_REGISTER_ALL_VISITABLE_ASTNODE(impl, invoker) \
		INDIRECT_REGISTER_VISITABLE(impl, invoker, \
				/* basic */ \
				ASTNode, \
				Annotations, \
				Annotation, \
				Block, \
				Identifier, \
					SimpleIdentifier, \
					NestedIdentifier, \
					TemplatedIdentifier, \
				Literal, \
					NumericLiteral, \
					StringLiteral, \
					ObjectLiteral, \
				TypeSpecifier, \
				FunctionType, \
				/* module */ \
				Program, \
				Package, \
				Import, \
				/* declaration */ \
				Declaration, \
					ClassDecl, \
					EnumDecl, \
					FunctionDecl, \
					InterfaceDecl, \
					VariableDecl, \
					TypedefDecl, \
				/* statement */ \
				Statement, \
					DeclarativeStmt, \
					ExpressionStmt, \
					IterativeStmt, \
						ForStmt, \
						ForeachStmt, \
						WhileStmt, \
					SelectionStmt, \
						IfElseStmt, \
						SwitchStmt, \
					BranchStmt, \
				/* expression */ \
				Expression, \
					PrimaryExpr, \
					UnaryExpr, \
					BinaryExpr, \
					TernaryExpr, \
					MemberExpr, \
					CallExpr, \
					CastExpr \
				)

#define REGISTER_ALL_VISITABLE_ASTNODE(invoker) \
		REGISTER_VISITABLE(invoker, \
				/* basic */ \
				ASTNode, \
				Annotations, \
				Annotation, \
				Block, \
				Identifier, \
					SimpleIdentifier, \
					NestedIdentifier, \
					TemplatedIdentifier, \
				Literal, \
					NumericLiteral, \
					StringLiteral, \
					ObjectLiteral, \
				TypeSpecifier, \
				FunctionType, \
				/* module */ \
				Program, \
				Package, \
				Import, \
				/* declaration */ \
				Declaration, \
					ClassDecl, \
					EnumDecl, \
					FunctionDecl, \
					InterfaceDecl, \
					VariableDecl, \
					TypedefDecl, \
				/* statement */ \
				Statement, \
					DeclarativeStmt, \
					ExpressionStmt, \
					IterativeStmt, \
						ForStmt, \
						ForeachStmt, \
						WhileStmt, \
					SelectionStmt, \
						IfElseStmt, \
						SwitchStmt, \
					BranchStmt, \
				/* expression */ \
				Expression, \
					PrimaryExpr, \
					UnaryExpr, \
					BinaryExpr, \
					TernaryExpr, \
					MemberExpr, \
					CallExpr, \
					CastExpr \
				)

namespace zillians { namespace language { namespace tree { namespace visitor {

struct GenericVisitor : Visitor<const ASTNode, void, VisitorImplementation::recursive_dfs>
{
	template<typename Impl, typename Invoker>
	void registerInvoker(Impl* impl)
	{
		INDIRECT_REGISTER_ALL_VISITABLE_ASTNODE(impl, Invoker);
	}

	//////////////////////////////////////////////////////////////////////
	/// Basic

	void apply(const ASTNode& node)
	{
	}

	void apply(const Annotation& node)
	{
		if(node.name) visit(*node.name);
		foreach(i, node.attribute_list)
		{
			if(i->first)  visit(*i->first);
			if(i->second) visit(*i->second);
		}
	}

	void apply(const Annotations& node)
	{
		foreach(i, node.annotation_list) visit(**i);
	}

	void apply(const Block& node)
	{
		foreach(i, node.objects) visit(**i);
	}

	void apply(Identifier& node)
	{
		BOOST_ASSERT(false && "reaching unreachable code");
	}

	void apply(const SimpleIdentifier& node)
	{
	}

	void apply(const NestedIdentifier& node)
	{
		foreach(i, node.identifier_list) visit(**i);
	}

	void apply(const TemplatedIdentifier& node)
	{
		if(node.id) visit(*node.id);

		foreach(i, node.templated_type_list) visit(**i);
	}

	void apply(const TypeSpecifier& node)
	{
		switch(node.type)
		{
		case TypeSpecifier::ReferredType::CLASS_DECL:    if(node.referred.class_decl)    visit(*node.referred.class_decl); break;
		case TypeSpecifier::ReferredType::FUNCTION_DECL: if(node.referred.function_decl) visit(*node.referred.function_decl); break;
		case TypeSpecifier::ReferredType::ENUM_DECL:     if(node.referred.enum_decl)     visit(*node.referred.enum_decl); break;
		case TypeSpecifier::ReferredType::TYPEDEF_DECL:  if(node.referred.typedef_decl)  visit(*node.referred.typedef_decl); break;
		case TypeSpecifier::ReferredType::FUNCTION_TYPE: if(node.referred.function_type) visit(*node.referred.function_type); break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:   if(node.referred.unspecified)   visit(*node.referred.unspecified); break;
		}
	}

	void apply(const FunctionType& node)
	{
		foreach(i, node.templated_parameters) visit(**i);
		foreach(i, node.argument_types)       visit(**i);
	}

	//////////////////////////////////////////////////////////////////////
	/// Module

	void apply(const Program& node)
	{
		if(node.root) visit(*node.root);
	}

	void apply(const Package& node)
	{
		if(node.id) visit(*node.id);
		foreach(i, node.children) visit(**i);
		foreach(i, node.objects)  visit(**i);
	}

	void apply(const Import& node)
	{
		if(node.ns) visit(*node.ns);
	}

	//////////////////////////////////////////////////////////////////////
	/// Declaration
	void apply(Declaration& node)
	{
		BOOST_ASSERT(false && "reaching unreachable code");
	}

	void apply(const ClassDecl& node)
	{
		if(node.name) visit(*node.name);
		if(node.base) visit(*node.base);
		foreach(i, node.implements)       visit(**i);
		foreach(i, node.member_functions) visit(**i);
		foreach(i, node.member_variables) visit(**i);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const EnumDecl& node)
	{
		if(node.name) visit(*node.name);
		foreach(i, node.enumeration_list)
		{
			if(i->first)  visit(*i->first);
			if(i->second) visit(*i->second);
		}

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const FunctionDecl& node)
	{
		if(node.name) visit(*node.name);
		foreach(i, node.parameters)
		{
			if(i->get<0>()) visit(*i->get<0>());
			if(i->get<1>()) visit(*i->get<1>());
			if(i->get<2>()) visit(*i->get<2>());
		}
		if(node.type)  visit(*node.type);
		if(node.block) visit(*node.block);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const InterfaceDecl& node)
	{
		if(node.name) visit(*node.name);
		foreach(i, node.member_functions) visit(**i);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const TypedefDecl& node)
	{
		if(node.from) visit(*node.from);
		if(node.to) visit(*node.to);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const VariableDecl& node)
	{
		if(node.name)        visit(*node.name);
		if(node.type)        visit(*node.type);
		if(node.initializer) visit(*node.initializer);

		if(node.annotations) visit(*node.annotations);
	}

	//////////////////////////////////////////////////////////////////////
	/// Statement
	void apply(Statement& node)
	{
		BOOST_ASSERT(false && "reaching unreachable code");
	}

	void apply(const DeclarativeStmt& node)
	{
		if(node.declaration) visit(*node.declaration);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const ExpressionStmt& node)
	{
		if(node.expr) visit(*node.expr);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const ForeachStmt& node)
	{
		if(node.iterator) visit(*node.iterator);
		if(node.range)    visit(*node.range);
		if(node.block)    visit(*node.block);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const ForStmt& node)
	{
		if(node.init)  visit(*node.init);
		if(node.cond)  visit(*node.cond);
		if(node.step) visit(*node.step);
		if(node.block) visit(*node.block);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const WhileStmt& node)
	{
		if(node.cond)  visit(*node.cond);
		if(node.block) visit(*node.block);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const IfElseStmt& node)
	{
		if(node.if_branch.cond)  visit(*node.if_branch.cond);
		if(node.if_branch.block) visit(*node.if_branch.block);
		foreach(i, node.elseif_branches)
		{
			if(i->cond)  visit(*i->cond);
			if(i->block) visit(*i->block);
		}
		if(node.else_block) visit(*node.else_block);

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const SwitchStmt& node)
	{
		if(node.node) visit(*node.node);
		foreach(i, node.cases)
		{
			if(i->cond)  visit(*i->cond);
			if(i->block) visit(*i->block);
		}

		if(node.annotations) visit(*node.annotations);
	}

	void apply(const BranchStmt& node)
	{
		if(node.annotations) visit(*node.annotations);
	}

	//////////////////////////////////////////////////////////////////////
	/// Expression
	void apply(Expression& node)
	{
		BOOST_ASSERT(false && "reaching unreachable code");
	}

	void apply(const PrimaryExpr& node)
	{
		switch(node.catagory)
		{
		case PrimaryExpr::Catagory::IDENTIFIER: if(node.value.identifier) visit(*node.value.identifier); break;
		case PrimaryExpr::Catagory::LITERAL:    if(node.value.literal)    visit(*node.value.literal); break;
		case PrimaryExpr::Catagory::LAMBDA:     if(node.value.lambda)     visit(*node.value.lambda); break;
		}
	}

	void apply(const UnaryExpr& node)
	{
		if(node.node) visit(*node.node);
	}

	void apply(const BinaryExpr& node)
	{
		if(node.left)  visit(*node.left);
		if(node.right) visit(*node.right);
	}

	void apply(const TernaryExpr& node)
	{
		if(node.cond)       visit(*node.cond);
		if(node.true_node)  visit(*node.true_node);
		if(node.false_node) visit(*node.false_node);
	}

	void apply(const MemberExpr& node)
	{
		if(node.node)   visit(*node.node);
		if(node.member) visit(*node.member);
	}

	void apply(const CallExpr& node)
	{
		if(node.node) visit(*node.node);
		foreach(i, node.parameters) visit(**i);
	}

	void apply(const CastExpr& node)
	{
		if(node.node) visit(*node.node);
		if(node.type) visit(*node.type);
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICVISITOR_H_ */
