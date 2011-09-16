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

#ifndef ZILLIANS_LANGUAGE_TREE_DECLARATIVESTMT_H_
#define ZILLIANS_LANGUAGE_TREE_DECLARATIVESTMT_H_

#include "language/tree/statement/Statement.h"
#include "language/tree/declaration/Declaration.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct DeclarativeStmt : public Statement
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(DeclarativeStmt, (DeclarativeStmt)(Statement)(ASTNode));

	explicit DeclarativeStmt(Declaration* declaration) : declaration(declaration)
	{
		BOOST_ASSERT(declaration && "null declaration for declarative statement is not allowed");

		declaration->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const DeclarativeStmt* p = cast<const DeclarativeStmt>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!Statement::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
		COMPARE_ASTNODE_MEMBER(declaration);

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

	Declaration* declaration;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_DECLARATIVESTMT_H_ */
