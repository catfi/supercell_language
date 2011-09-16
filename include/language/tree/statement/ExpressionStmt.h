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

#ifndef ZILLIANS_LANGUAGE_TREE_EXPRESSIONSTMT_H_
#define ZILLIANS_LANGUAGE_TREE_EXPRESSIONSTMT_H_

#include "language/tree/statement/Statement.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct ExpressionStmt : public Statement
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ExpressionStmt, (ExpressionStmt)(Statement)(ASTNode));

	explicit ExpressionStmt(Expression* expr) : expr(expr)
	{
		BOOST_ASSERT(expr && "null expression for expression statement is not allowed");

		expr->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Statement)
		COMPARE_MEMBER(expr)
		END_COMPARE()
    }

	Expression* expr;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_EXPRESSIONSTMT_H_ */
