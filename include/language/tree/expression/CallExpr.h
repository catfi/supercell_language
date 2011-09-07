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

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Expression>(*this);
        ar & parameters;
    }

	ASTNode* node;
	std::vector<Expression*> parameters;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::CallExpr* p, const unsigned int file_version)
{
	ar << p->node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::CallExpr* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

    ASTNode* node;
	ar >> node;
	::new(p) CallExpr(node);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_CALLEXPR_H_ */
