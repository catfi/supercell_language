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

#ifndef ZILLIANS_LANGUAGE_TREE_EXPRESSION_H_
#define ZILLIANS_LANGUAGE_TREE_EXPRESSION_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Annotations.h"

namespace zillians { namespace language { namespace tree {

struct Selection;

struct Expression : public ASTNode
{
    friend class Selection;
	friend class boost::serialization::access;

	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(Expression, (Expression)(ASTNode))

	Expression()
	{ }

	bool isLValue() const { return !isRValue(); }
	virtual bool isRValue() const = 0;

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	UNUSED_ARGUMENT(rhs);
    	UNUSED_ARGUMENT(visited);

    	return true;
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	UNUSED_ARGUMENT(from);
    	UNUSED_ARGUMENT(to);
    	UNUSED_ARGUMENT(update_parent);

    	return false;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_EXPRESSION_H_ */
