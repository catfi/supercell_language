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
 * @date Aug 5, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_TERNARYEXPR_H_
#define ZILLIANS_LANGUAGE_TREE_TERNARYEXPR_H_

#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct TernaryExpr : public Expression
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(TernaryExpr, (TernaryExpr)(Expression)(ASTNode));

	// so far there only one ternary expression: the conditional expression
	TernaryExpr(Expression* cond, ASTNode* true_node, ASTNode* false_node) : cond(cond), true_node(true_node), false_node(false_node)
	{
		cond->parent = this;
		true_node->parent = this;
		false_node->parent = this;
	}

	Expression* cond;
	ASTNode* true_node;
	ASTNode* false_node;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_TERNARYEXPR_H_ */
