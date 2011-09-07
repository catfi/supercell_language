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
	TernaryExpr(Expression* cond, Expression* true_node, Expression* false_node) : cond(cond), true_node(true_node), false_node(false_node)
	{
		BOOST_ASSERT(cond && "null condition for ternary expression is not allowed");
		BOOST_ASSERT(true_node && "null \"true node\" for ternary expression is not allowed");
		BOOST_ASSERT(false_node && "null \"false node\" for ternary expression is not allowed");

		cond->parent = this;
		true_node->parent = this;
		false_node->parent = this;
	}

	virtual bool isRValue()
	{
		// it's R-value if either "true block" or "false block" is R-value
		return (true_node->isRValue() | false_node->isRValue());
	}

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const TernaryExpr* p = cast<const TernaryExpr>(&rhs);
        if (p == NULL) return false;
        // compare base class
        if (!Expression::isEqual(*p, visited)) return false;

        // compare data member
        if (!isASTNodeMemberEqual   (&TernaryExpr::cond            , *this, *p, visited)) return false;
        if (!isASTNodeMemberEqual   (&TernaryExpr::true_node       , *this, *p, visited)) return false;
        if (!isASTNodeMemberEqual   (&TernaryExpr::false_node      , *this, *p, visited)) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Expression>(*this);
    }

	Expression* cond;
	Expression* true_node;
	Expression* false_node;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TernaryExpr* p, const unsigned int file_version)
{
    ar << p->cond;
    ar << p->true_node;
    ar << p->false_node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::TernaryExpr* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	Expression* cond;
	Expression* true_node;
	Expression* false_node;

    ar >> cond;
    ar >> true_node;
    ar >> false_node;

	::new(p) TernaryExpr(cond, true_node, false_node);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_TERNARYEXPR_H_ */
