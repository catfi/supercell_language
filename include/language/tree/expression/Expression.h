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

	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(Expression, (Expression)(ASTNode))

	Expression() : annotations(NULL)
	{ }

	void setAnnotation(Annotations* anns)
	{
		if(annotations) annotations->parent = NULL;
		anns->parent = this;
		annotations = anns;
	}

	bool isLValue() { return !isRValue(); }
	virtual bool isRValue() = 0;

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(annotations)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(annotations)
    	END_REPLACE()
    }
	Annotations* annotations;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_EXPRESSION_H_ */
