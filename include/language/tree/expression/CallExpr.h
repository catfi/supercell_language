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

#ifndef ZILLIANS_LANGUAGE_TREE_CALLEXPR_H_
#define ZILLIANS_LANGUAGE_TREE_CALLEXPR_H_

#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct CallExpr : public Expression
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(CallExpr, (CallExpr)(Expression)(ASTNode));

	explicit CallExpr(ASTNode* node) : node(node)
	{
		BOOST_ASSERT(node && "null callee for call expression is not allowed");

		node->parent = this;
	}

	void appendParameter(Expression* parameter)
	{
		BOOST_ASSERT(parameter && "null parameter for call expression is not allowed");

		parameter->parent = this;
		parameters.push_back(parameter);
	}

	virtual bool isRValue()
	{
		return true;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Expression)
		COMPARE_MEMBER(node)
		COMPARE_MEMBER(parameters)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to)
    {
    	BEGIN_REPLACE_WITH_BASE(Expression)
		REPLACE_USE_WITH(node)
		REPLACE_USE_WITH(parameters)
    	END_REPLACE()
    }

	ASTNode* node;
	std::vector<Expression*> parameters;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_CALLEXPR_H_ */
