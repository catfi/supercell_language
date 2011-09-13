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
        if(visited.count(this))
        {
            return true ;
        }

        const MemberExpr* p = cast<const MemberExpr>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!Expression::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(!isASTNodeMemberEqual(&MemberExpr::node, *this, *p, visited))
        {
            return false;
        }
        if(!isASTNodeMemberEqual(&MemberExpr::member, *this, *p, visited))
        {
            return false;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<Expression>(*this);
    }

	ASTNode* node;
	Identifier* member;
};

} } }

namespace boost { namespace serialization {

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::MemberExpr* p, const unsigned int file_version)
{
	ar << p->node;
    ar << p->member;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::MemberExpr* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	ASTNode* node;
	Identifier* member;
	ar >> node;
    ar >> member;
	::new(p) MemberExpr(node, member);
}

} } // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_MEMBEREXPR_H_ */
