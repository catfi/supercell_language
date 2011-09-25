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

#ifndef ZILLIANS_LANGUAGE_TREE_BLOCK_H_
#define ZILLIANS_LANGUAGE_TREE_BLOCK_H_

#include "language/tree/ASTNode.h"

namespace zillians { namespace language { namespace tree {

struct Block : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Block, (Block)(ASTNode));

	explicit Block(bool is_pipelined = false, bool is_async = false) : is_pipelined_block(is_pipelined), is_async_block(is_async)
	{ }

	void appendObject(ASTNode* object)
	{
		object->parent = this;
		objects.push_back(object);
	}

	template<typename T>
	void appendObjects(std::vector<T*>& object_list)
	{
		deduced_foreach(i, object_list)
		{
			(*i)->parent = this;
			objects.push_back(*i);
		}
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(is_pipelined_block)
		COMPARE_MEMBER(is_async_block)
		COMPARE_MEMBER(objects)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(objects)
    	END_REPLACE()
    }

	bool is_pipelined_block;
	bool is_async_block;
	std::vector<ASTNode*> objects;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_BLOCK_H_ */
