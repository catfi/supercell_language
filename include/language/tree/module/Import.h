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

#ifndef ZILLIANS_LANGUAGE_TREE_IMPORT_H_
#define ZILLIANS_LANGUAGE_TREE_IMPORT_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"

namespace zillians { namespace language { namespace tree {

/**
 * Package is used to represent the hierarchical structure of a program.
 * Every ASTNode, except Package and Program, must be contained by a Package.
 */
struct Import : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Import, (Import)(ASTNode));

	explicit Import(Identifier* _ns) : ns(_ns)
	{
		BOOST_ASSERT(_ns && "null identifier for import node is not allowed");

		ns->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const Import* p = cast<const Import>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        // The base is ASTNode, don't need to be compared.

        // compare data member
        if(!isASTNodeMemberEqual(&Import::ns, *this, *p, visited))
        {
            return false;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

	Identifier* ns;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_IMPORT_H_ */
