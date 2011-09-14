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

#ifndef ZILLIANS_LANGUAGE_TREE_STATEMENT_H_
#define ZILLIANS_LANGUAGE_TREE_STATEMENT_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Annotations.h"

namespace zillians { namespace language { namespace tree {

struct Statement : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Statement, (Statement)(ASTNode));

	Statement() : annotations(NULL)
	{ }

	void setAnnotation(Annotations* anns)
	{
		if(annotations) annotations->parent = NULL;
		anns->parent = this;
		annotations = anns;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const Statement* p = cast<const Statement>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        // base is ASTNode, no need to compare

        // compare data member
        if(!isASTNodeMemberEqual(&Statement::annotations, *this, *p, visited))
        {
            return false;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

	Annotations* annotations;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_STATEMENT_H_ */
