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
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(TernaryExpr, (TernaryExpr)(Expression)(ASTNode));

	// so far there only one ternary expression: the conditional expression
	TernaryExpr(Expression* cond, Expression* true_node, Expression* false_node) : cond(cond), true_node(true_node), false_node(false_node)
	{
		BOOST_ASSERT(cond && "null condition for ternary expression is not allowed");
		BOOST_ASSERT(true_node && "null \"true node\" for ternary expression is not allowed");
		BOOST_ASSERT(false_node && "null \"false node\" for ternary expression is not allowed");

		cond->parent = this;
		true_node->parent = this;
		false_node->parent = this;
	}

	virtual bool isRValue() const
	{
		// it's R-value if either "true block" or "false block" is R-value
		return (true_node->isRValue() || false_node->isRValue());
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Expression)
		COMPARE_MEMBER(cond)
		COMPARE_MEMBER(true_node)
		COMPARE_MEMBER(false_node)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Expression)
		REPLACE_USE_WITH(cond)
		REPLACE_USE_WITH(true_node)
		REPLACE_USE_WITH(false_node)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new TernaryExpr(
    			(cond) ? cast<Expression>(cond->clone()) : NULL,
    			(true_node) ? cast<Expression>(true_node->clone()) : NULL,
    			(false_node) ? cast<Expression>(false_node->clone()) : NULL);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Expression>(*this);
    	ar & cond;
    	ar & true_node;
    	ar & false_node;
    }

	Expression* cond;
	Expression* true_node;
	Expression* false_node;

protected:
	TernaryExpr() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_TERNARYEXPR_H_ */
