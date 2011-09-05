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
 * @date Aug 10, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_CASTEXPR_H_
#define ZILLIANS_LANGUAGE_TREE_CASTEXPR_H_

#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct CastExpr : public Expression
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(CastExpr, (CastExpr)(Expression)(ASTNode));

	explicit CastExpr(Expression* node, TypeSpecifier* type) : node(node), type(type)
	{
		BOOST_ASSERT(node && "null node for cast expression is not allowed");
		BOOST_ASSERT(type && "null type for cast expression is not allowed");

		node->parent = this;
		type->parent = this;
	}

	virtual bool isRValue()
	{
		return node->isRValue();
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Expression>(*this);
    }

	Expression* node;
	TypeSpecifier* type;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::CastExpr* p, const unsigned int file_version)
{
	ar << p->node;
    ar << p->type;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::CastExpr* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	Expression* node;
	TypeSpecifier* type;
	ar >> node;
    ar >> type;
	::new(p) CastExpr(node, type);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_CASTEXPR_H_ */
