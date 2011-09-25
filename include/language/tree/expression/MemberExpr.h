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

#ifndef ZILLIANS_LANGUAGE_TREE_MEMBEREXPR_H_
#define ZILLIANS_LANGUAGE_TREE_MEMBEREXPR_H_

#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct MemberExpr : public Expression
{
	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(MemberExpr, (MemberExpr)(Expression)(ASTNode));

	explicit MemberExpr(ASTNode* node, Identifier* member) : node(node), member(member)
	{
		BOOST_ASSERT(node && "null node for member expression is not allowed");
		BOOST_ASSERT(member && "null identifier for member expression is not allowed");

		node->parent = this;
		member->parent = this;
	}

	virtual bool isRValue()
	{
		return false;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Expression)
		COMPARE_MEMBER(node)
		COMPARE_MEMBER(member)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to)
    {
    	BEGIN_REPLACE_WITH_BASE(Expression)
		REPLACE_USE_WITH(node)
		REPLACE_USE_WITH(member)
    	END_REPLACE()
    }

	ASTNode* node;
	Identifier* member;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_MEMBEREXPR_H_ */
