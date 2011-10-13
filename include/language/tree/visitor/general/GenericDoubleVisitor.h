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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICDOUBLEVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICDOUBLEVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/visitor/general/GenericVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

// TODO change visitor implementation into a templated parameter (due to lack of support in GCC 4.4)
struct GenericDoubleVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	struct ApplyVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
	{
		Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>* user_visitor;

		CREATE_INVOKER(applyInvoker, apply);

		ApplyVisitor()
		{
			REGISTER_ALL_VISITABLE_ASTNODE(applyInvoker)
		}
		//////////////////////////////////////////////////////////////////////
		/// Basic

		void apply(ASTNode& node)
		{
		}

		void apply(Annotation& node)
		{
			if(node.name) user_visitor->visit(*node.name);
			foreach(i, node.attribute_list)
			{
				if(i->first) user_visitor->visit(*i->first);
				if(i->second) user_visitor->visit(*i->second);
			}
		}

		void apply(Annotations& node)
		{
			foreach(i, node.annotation_list)	user_visitor->visit(**i);
		}

		void apply(Block& node)
		{
			foreach(i, node.objects)	user_visitor->visit(**i);
		}

		void apply(Identifier& node)
		{
			BOOST_ASSERT(false && "reaching unreachable code");
		}

		void apply(SimpleIdentifier& node)
		{
		}

		void apply(NestedIdentifier& node)
		{
			foreach(i, node.identifier_list)	user_visitor->visit(**i);
		}

		void apply(TemplatedIdentifier& node)
		{
			if(node.id) user_visitor->visit(*node.id);

			foreach(i, node.templated_type_list)	user_visitor->visit(**i);
		}

		void apply(TypeSpecifier& node)
		{
			switch(node.type)
			{
			case TypeSpecifier::ReferredType::FUNCTION_TYPE: if(node.referred.function_type) user_visitor->visit(*node.referred.function_type); break;
			case TypeSpecifier::ReferredType::UNSPECIFIED: if(node.referred.unspecified) user_visitor->visit(*node.referred.unspecified); break;
			default: break;
			}
		}

		void apply(FunctionType& node)
		{
			foreach(i, node.templated_parameters)	user_visitor->visit(**i);
			foreach(i, node.argument_types)			user_visitor->visit(**i);
		}

		//////////////////////////////////////////////////////////////////////
		/// Module
		void apply(Internal& node)
		{
			if(node.VoidTy)     user_visitor->visit(*node.VoidTy);
			if(node.BooleanTy)  user_visitor->visit(*node.BooleanTy);
			if(node.Int8Ty)     user_visitor->visit(*node.Int8Ty);
			if(node.Int16Ty)    user_visitor->visit(*node.Int16Ty);
			if(node.Int32Ty)    user_visitor->visit(*node.Int32Ty);
			if(node.Int64Ty)    user_visitor->visit(*node.Int64Ty);
			if(node.Float32Ty)  user_visitor->visit(*node.Float32Ty);
			if(node.Float64Ty)  user_visitor->visit(*node.Float64Ty);
			if(node.ObjectTy)   user_visitor->visit(*node.ObjectTy);
			if(node.FunctionTy) user_visitor->visit(*node.FunctionTy);
			if(node.StringTy)   user_visitor->visit(*node.StringTy);

			foreach(i, node.others) user_visitor->visit(**i);
		}

		void apply(Program& node)
		{
			foreach(i, node.imports) user_visitor->visit(**i);

			if(node.root) user_visitor->visit(*node.root);
			if(node.internal) user_visitor->visit(*node.internal);
		}

		void apply(Package& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.id) user_visitor->visit(*node.id);
			foreach(i, node.children)	user_visitor->visit(**i);
			foreach(i, node.objects)	user_visitor->visit(**i);
		}

		void apply(Import& node)
		{
			if(node.ns) user_visitor->visit(*node.ns);
		}

		//////////////////////////////////////////////////////////////////////
		/// Declaration
		void apply(Declaration& node)
		{
			BOOST_ASSERT(false && "reaching unreachable code");
		}

		void apply(ClassDecl& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.name) user_visitor->visit(*node.name);
			if(node.base) user_visitor->visit(*node.base);
			foreach(i, node.implements)			user_visitor->visit(**i);
			foreach(i, node.member_functions)	user_visitor->visit(**i);
			foreach(i, node.member_variables)	user_visitor->visit(**i);
		}

		void apply(EnumDecl& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.name) user_visitor->visit(*node.name);
			foreach(i, node.enumeration_list)
			{
				if(i->first) user_visitor->visit(*i->first);
				if(i->second) user_visitor->visit(*i->second);
			}
		}

		void apply(FunctionDecl& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.name) user_visitor->visit(*node.name);
			foreach(i, node.parameters)
				user_visitor->visit(**i);
			if(node.type) user_visitor->visit(*node.type);
			if(node.block) user_visitor->visit(*node.block);
		}

		void apply(InterfaceDecl& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.name) user_visitor->visit(*node.name);
			foreach(i, node.member_functions)
				user_visitor->visit(**i);
		}

		void apply(TypedefDecl& node)
		{
			if(node.type) user_visitor->visit(*node.type);
			if(node.name) user_visitor->visit(*node.name);

			if(node.annotations) user_visitor->visit(*node.annotations);
		}

		void apply(VariableDecl& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.initializer) user_visitor->visit(*node.initializer);
			if(node.name) user_visitor->visit(*node.name);
			if(node.type) user_visitor->visit(*node.type);
		}

		//////////////////////////////////////////////////////////////////////
		/// Statement
		void apply(Statement& node)
		{
			BOOST_ASSERT(false && "reaching unreachable code");
		}

		void apply(DeclarativeStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.declaration) user_visitor->visit(*node.declaration);
		}

		void apply(ExpressionStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.expr) user_visitor->visit(*node.expr);
		}

		void apply(ForStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.init) user_visitor->visit(*node.init);
			if(node.cond) user_visitor->visit(*node.cond);
			if(node.block) user_visitor->visit(*node.block);
			if(node.step) user_visitor->visit(*node.step);
		}

		void apply(ForeachStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.iterator) user_visitor->visit(*node.iterator);
			if(node.range) user_visitor->visit(*node.range);
			if(node.block) user_visitor->visit(*node.block);
		}

		void apply(WhileStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.cond) user_visitor->visit(*node.cond);
			if(node.block) user_visitor->visit(*node.block);
		}

		void apply(IfElseStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.if_branch.cond) user_visitor->visit(*node.if_branch.cond);
			if(node.if_branch.block) user_visitor->visit(*node.if_branch.block);
			foreach(i, node.elseif_branches)
			{
				if(i->cond) user_visitor->visit(*i->cond);
				if(i->block) user_visitor->visit(*i->block);
			}
			if(node.else_block) user_visitor->visit(*node.else_block);
		}

		void apply(SwitchStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.node) user_visitor->visit(*node.node);
			foreach(i, node.cases)
			{
				if(i->cond) user_visitor->visit(*i->cond);
				if(i->block) user_visitor->visit(*i->block);
			}
		}

		void apply(BranchStmt& node)
		{
			if(node.annotations) user_visitor->visit(*node.annotations);

			if(node.result) user_visitor->visit(*node.result);
		}

		//////////////////////////////////////////////////////////////////////
		/// Expression
		void apply(Expression& node)
		{
			BOOST_ASSERT(false && "reaching unreachable code");
		}

		void apply(PrimaryExpr& node)
		{
			switch(node.catagory)
			{
			case PrimaryExpr::Catagory::IDENTIFIER: if(node.value.identifier) user_visitor->visit(*node.value.identifier); break;
			case PrimaryExpr::Catagory::LITERAL: if(node.value.literal) user_visitor->visit(*node.value.literal); break;
			case PrimaryExpr::Catagory::LAMBDA: if(node.value.lambda) user_visitor->visit(*node.value.lambda); break;
			}
		}

		void apply(UnaryExpr& node)
		{
			if(node.node) user_visitor->visit(*node.node);
		}

		void apply(BinaryExpr& node)
		{
			if(node.isRighAssociative())
			{
				if(node.right) user_visitor->visit(*node.right);
				if(node.left) user_visitor->visit(*node.left);
			}
			else
			{
				if(node.left) user_visitor->visit(*node.left);
				if(node.right) user_visitor->visit(*node.right);
			}
		}

		void apply(TernaryExpr& node)
		{
			if(node.cond) user_visitor->visit(*node.cond);
			if(node.true_node) user_visitor->visit(*node.true_node);
			if(node.false_node) user_visitor->visit(*node.false_node);
		}

		void apply(MemberExpr& node)
		{
			if(node.node) user_visitor->visit(*node.node);
			if(node.member) user_visitor->visit(*node.member);
		}

		void apply(CallExpr& node)
		{
			if(node.node) user_visitor->visit(*node.node);
			foreach(i, node.parameters)
			user_visitor->visit(**i);
		}

		void apply(CastExpr& node)
		{
			if(node.node) user_visitor->visit(*node.node);
			if(node.type) user_visitor->visit(*node.type);
		}
	};

	ApplyVisitor revisitor;

	GenericDoubleVisitor()
	{
		revisitor.user_visitor = this;
	}

	void revisit(ASTNode& node)
	{
		revisitor.visit(node);
	}

	void terminateRevisit()
	{
		revisitor.terminate();
	}
};

} } } }


#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICDOUBLEVISITOR_H_ */
