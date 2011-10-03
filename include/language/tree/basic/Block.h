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
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Block, (Block)(ASTNode));

	explicit Block(bool is_pipelined = false, bool is_async = false) : is_pipelined_block(is_pipelined), is_async_block(is_async)
	{ }

	void appendObject(ASTNode* object)
	{
		object->parent = this;
		objects.push_back(object);
	}

	bool insertObject(ASTNode* after, ASTNode* object, bool replace_after = false)
	{
		if(!after)
		{
			appendObject(object);
			return true;
		}
		else
		{
			auto it = std::find(objects.begin(), objects.end(), after);
			if(it != objects.end())
			{
				++it;
				object->parent = this;
				objects.insert(it, object);

				if(replace_after)
					objects.erase(it);

				return true;
			}
			else
			{
				return false;
			}
		}
	}

	template<typename T>
	void appendObjects(T& object_list)
	{
		deduced_foreach(i, object_list)
			(*i)->parent = this;
		objects.insert(objects.end(), object_list.begin(), object_list.end());
	}

	template<typename T>
	bool insertObjects(ASTNode* after, T& object_list, bool replace_after = false)
	{
		if(!after)
		{
			appendObjects(object_list);
			return true;
		}
		else
		{
			auto it = std::find(objects.begin(), objects.end(), after);
			if(it != objects.end())
			{
				deduced_foreach(i, object_list)
					(*i)->parent = this;
				++it;
				objects.insert(it, object_list.begin(), object_list.end());

				if(replace_after)
					objects.erase(it);

				return true;
			}
			else
			{
				return false;
			}
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

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(objects)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Block* cloned = new Block(is_pipelined_block, is_async_block);
    	foreach(i, objects)
    		cloned->objects.push_back((*i) ? (*i)->clone() : NULL);
    	return cloned;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & is_pipelined_block;
    	ar & is_async_block;
    	ar & objects;
    }

	bool is_pipelined_block;
	bool is_async_block;
	std::list<ASTNode*> objects;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_BLOCK_H_ */
