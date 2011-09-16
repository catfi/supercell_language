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

#ifndef ZILLIANS_LANGUAGE_TREE_ENUMDECL_H_
#define ZILLIANS_LANGUAGE_TREE_ENUMDECL_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct EnumDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(EnumDecl, (EnumDecl)(Declaration)(ASTNode));

	explicit EnumDecl(Identifier* name) : name(name)
	{
		BOOST_ASSERT(name && "null enumeration name is not allowed");
	}

	void addEnumeration(SimpleIdentifier* tag, Expression* value = NULL)
	{
		BOOST_ASSERT(tag && "null tag for enumeration is not allowed");

		tag->parent = this;
		if(value) value->parent = this;
		enumeration_list.push_back(std::make_pair(tag, value));
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Declaration)
		COMPARE_MEMBER(name)
		COMPARE_MEMBER(enumeration_list)
		END_COMPARE()
    }

	Identifier* name;
	std::vector<std::pair<SimpleIdentifier*, Expression*>> enumeration_list;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ENUMDECL_H_ */
