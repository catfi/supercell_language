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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_RESTRUCTURESTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_RESTRUCTURESTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/context/TransformerContext.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/tree/visitor/NameManglingVisitor.h"
#include "language/stage/transformer/context/ManglingStageContext.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NameManglingVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * RestructureStageVisitor is the visitation helper for RestructureStage
 *
 * @see RestructureStage
 */
struct RestructureStageVisitor : public GenericDoubleVisitor
{
    CREATE_INVOKER(restructInvoker, restruct)

	RestructureStageVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(restructInvoker)
	}

	void restruct(ASTNode& node)
	{
		revisit(node);
	}

	void restruct(ClassDecl& node)
	{
		revisit(node);

		// check if there's no default constructor,
		bool has_default_constructor = false;
		foreach(i, node.member_functions)
		{
			if((*i)->name->toString() == L"new")
			{
				if( (*i)->parameters.size() == 0 || ( (*i)->parameters.size() == 1 && (*i)->parameters[0]->name->toString() == L"this" ) )
				{
					has_default_constructor = true;
					break;
				}
			}
		}

		// if there's none, create a new one for it
		if(!has_default_constructor)
		{
			transforms.push_back([&](){
				SimpleIdentifier* name = new SimpleIdentifier(L"new");
				TypeSpecifier* type = getParserContext().tangle->internal->getPrimitiveTy(PrimitiveType::VOID_TYPE);
				Block* block = new Block();

				FunctionDecl* default_constructor = new FunctionDecl(
						name,
						type,
						true, false,
						Declaration::VisibilitySpecifier::DEFAULT,
						block);

				node.addFunction(default_constructor);

				ASTNodeHelper::propogateSourceInfo(*name, node);
				ASTNodeHelper::propogateSourceInfo(*block, node);
				ASTNodeHelper::propogateSourceInfo(*default_constructor, node);
			});
		}
	}

	void restruct(FunctionDecl& node)
	{
		revisit(node);
		// 'prepand this' will be done at code gen stage
	}

	void restruct(VariableDecl& node)
	{
		revisit(node);

		if(node.initializer)
		{
            /*
			   transform all initializer into a separate assignment binary expression
			   for example (var a = ...):
			  
			         block
			        /  |  \
			       ......  decl_stmt
			                   |
			                var_decl
			               /        \
			              a         (initializer)
			  
			   will be transformed into:
			  
			         block---------------+
			        /  |  \               \
			       ......  decl_stmt       expr_stmt
			                   |               |
			                var_decl     binary_expr(=)
			               /        \         / \
			              a         (null)	a   (initializer)
			  
			   note that we skip parameters in FunctionDecl, which shouldn't be transformed
			*/
			if(ASTNodeHelper::hasOwner<FunctionDecl>(&node) && ASTNodeHelper::hasOwner<Block>(&node))
			{
				transforms.push_back([&](){
					DeclarativeStmt* anchor = cast<DeclarativeStmt>(node.parent);
					Block* parent = (anchor && anchor->parent) ? cast<Block>(anchor->parent) : NULL;
					SimpleIdentifier* name = cast<SimpleIdentifier>(node.name);

					BOOST_ASSERT(parent != NULL && name != NULL && anchor != NULL && "variable declaration has incorrect hierarchy");

					Identifier*     new_identifier      = cast<Identifier>(ASTNodeHelper::clone(node.name));
					PrimaryExpr*    new_primary_expr    = new PrimaryExpr(new_identifier);
					BinaryExpr*     new_assignment_expr = new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new_primary_expr, node.initializer);
					ExpressionStmt* new_expr_stmt       = new ExpressionStmt(new_assignment_expr);

					SplitReferenceContext::set(new_identifier, anchor);
					SplitReferenceContext::set(new_primary_expr, anchor);
					SplitReferenceContext::set(new_assignment_expr, anchor);
					SplitReferenceContext::set(new_expr_stmt, anchor);

					parent->insertObjectAfter(anchor, new_expr_stmt, false);

					ASTNodeHelper::propogateSourceInfo(*new_identifier, node); // propagate the source info
					ASTNodeHelper::propogateSourceInfo(*new_primary_expr, node); // propagate the source info
					ASTNodeHelper::propogateSourceInfo(*new_assignment_expr, node); // propagate the source info
					ASTNodeHelper::propogateSourceInfo(*new_expr_stmt, node); // propagate the source info

					node.initializer = NULL;
				});
			}

			// transform all class member variable with initializer into a initialization statement in all constructors
			else if(node.initializer && ASTNodeHelper::hasOwner<ClassDecl>(&node))
			{
				ClassDecl* owner_class = ASTNodeHelper::getOwner<ClassDecl>(&node);
				transforms.push_back([&, owner_class](){
					foreach(i, owner_class->member_functions)
					{
						if((*i)->name->toString() == L"new")
						{
							Identifier*     new_identifier      = cast<Identifier>(ASTNodeHelper::clone(node.name));
							PrimaryExpr*    new_primary_expr    = new PrimaryExpr(new_identifier);
							BinaryExpr*     new_assignment_expr = new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new_primary_expr, node.initializer);
							ExpressionStmt* new_expr_stmt       = new ExpressionStmt(new_assignment_expr);

							SplitReferenceContext::set(new_identifier, &node);
							SplitReferenceContext::set(new_primary_expr, &node);
							SplitReferenceContext::set(new_assignment_expr, &node);
							SplitReferenceContext::set(new_expr_stmt, &node);

							(*i)->block->prependObject(new_expr_stmt);

							ASTNodeHelper::propogateSourceInfo(*new_identifier, node); // propagate the source info
							ASTNodeHelper::propogateSourceInfo(*new_primary_expr, node); // propagate the source info
							ASTNodeHelper::propogateSourceInfo(*new_assignment_expr, node); // propagate the source info
							ASTNodeHelper::propogateSourceInfo(*new_expr_stmt, node); // propagate the source info

							node.initializer = NULL;
						}
					}
				});
			}
		}
	}

	void restruct(UnaryExpr& node)
	{
		revisit(node);

        /*
		   transform simple new operator to new function call
		   for example: new X
		  
		    unary_expr(new)
		          |
		     primary_expr
		          |
		         "X"
		  
		   will be transformed into: X.new(zillians.system.objectCreate())
		  
		                      call_expr
		                     /         \
		          member_expr           \ (parameters[0])
		         /           \           \
		    primary_expr    id("new")     call_expr
		         |                       /         \
		        "X"                 member_expr  <empty parameter>
		                           /           \
		                      member_expr    id("objectCreate")
		                     /           \
		                primary_expr     id("system")
		                     |
		               id("zillians")
		  
		  
		   for example: new X(a, b)
		  
		        unary_expr(new)
		              |
		           call_expr
		          /         +--------------------+
		    primary_expr     \ (parameters[0])    \ (parameter[1])
		         |            \                    \
		      id("X")     primary_expr         primary_expr
		                       |                    |
		                    id("a")              id("b")
		  
		   will be transformed into:
		  
		                      call_expr
		                     /         +-------------------------------+-------------------+
		          member_expr           \ (parameters[0])               \ (parameters[1])   \
		         /           \           \                               \                   \
		    primary_expr    id("new")     call_expr                  primary_expr        primary_expr
		         |                       /         \                      |                   |
		        "X"                 member_expr  <empty parameter>     id("a")             id("b")
		                           /           \
		                      member_expr    id("objectCreate")
		                     /           \
		                primary_expr     id("system")
		                     |
		               id("zillians")
		*/

	}

	void restruct(BinaryExpr& node)
	{
		revisit(node);

        /*
		   transform all arithmetic assignment into separate arithmetic expression and assignment expression
		   for example: a += b
		  
		        +=
		       /  \
		      a    b
		  
		   will be transformed into: a = a + b
		  
		        =
		       / \
		      a   +
		         / \
		        a   b
		  
		   for a more advanced example: a += b += c
		  
		        +=
		       /  \
		      a    +=
		          /  \
		         b    c
		  
		   will be transformed into: a = a + (b = (b + c))
		  
		        =
		       / \
		      a   +
		         / \
		        a   =
		           / \
		          b   +
		             / \
		            b   c
		*/
		if(node.isAssignment() && node.opcode != BinaryExpr::OpCode::ASSIGN)
		{
			transforms.push_back([&](){
				BinaryExpr::OpCode::type decomposed_op = BinaryExpr::OpCode::decomposeAssignment(node.opcode);

				BOOST_ASSERT(decomposed_op != BinaryExpr::OpCode::INVALID && "invalid decomposed binary operator");

				Expression* new_lhs = cast<Expression>(ASTNodeHelper::clone(node.left));
				BinaryExpr* new_rhs = new BinaryExpr(decomposed_op, new_lhs, node.right);

				SplitReferenceContext::set(new_lhs, &node);
				SplitReferenceContext::set(new_rhs, &node);

				node.replaceUseWith(*node.right, *new_rhs, false);
				new_rhs->parent = &node;
				node.opcode = BinaryExpr::OpCode::ASSIGN;

				ASTNodeHelper::propogateSourceInfo(*new_lhs, node); // propagate the source info
				ASTNodeHelper::propogateSourceInfo(*new_rhs, node); // propagate the source info
			});
		}
	}

public:
	bool hasTransforms()
	{
		return (transforms.size() > 0);
	}

	void applyTransforms()
	{
		foreach(i, transforms)
		{
			(*i)();
		}
		transforms.clear();
	}

private:
	std::vector<std::function<void()>> transforms;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_RESTRUCTURESTAGEVISITOR_H_ */
