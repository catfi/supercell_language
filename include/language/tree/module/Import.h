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
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Import, (Import)(ASTNode));

	explicit Import(Identifier* ns) : alias(NULL), ns(ns)
	{
		BOOST_ASSERT(ns && "null identifier for import node is not allowed");

		ns->parent = this;
	}

	explicit Import(Identifier* alias, Identifier* ns) : alias(alias), ns(ns)
	{
		BOOST_ASSERT(alias && "null identifier for import node is not allowed");
		BOOST_ASSERT(ns && "null identifier for import node is not allowed");

		alias->parent = this;
		ns->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(alias);
    	COMPARE_MEMBER(ns);
    	END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(alias)
		REPLACE_USE_WITH(ns)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	if(alias)
    		return new Import(cast<Identifier>(alias->clone()), cast<Identifier>(ns->clone()));
    	else
    		return new Import(cast<Identifier>(ns->clone()));
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & alias;
    	ar & ns;
    }

    Identifier* alias;
	Identifier* ns;

protected:
	Import() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_IMPORT_H_ */
