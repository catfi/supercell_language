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
 * @date Aug 11, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_GARBAGECOLLECTOR_H_
#define ZILLIANS_LANGUAGE_TREE_GARBAGECOLLECTOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "core/Singleton.h"
#include "utility/TemplateTricks.h"
#include <tr1/unordered_set>

namespace zillians { namespace language { namespace tree {

template<typename Base>
struct GarbageCollectorWrapper
{
	GarbageCollectorWrapper(std::tr1::unordered_set<Base*>& reachable_set) : reachable_set(reachable_set)
	{ }

	inline void mark(Base* object)
	{
		reachable_set.erase(object);
	}

	std::tr1::unordered_set<Base*>& reachable_set;
};

template<typename Base>
struct GarbageCollector : Singleton<GarbageCollector<Base>, SingletonInitialization::automatic>
{
	GarbageCollector()
	{ }

	~GarbageCollector()
	{
		cleanup();
	}

	bool find(Base* object)
	{
		typename std::tr1::unordered_set<Base*>::iterator it = objects.find(object);
		return (it == objects.end());
	}

	void add(Base* object)
	{
		objects.insert(object);
	}

	void remove(Base* object)
	{
		typename std::tr1::unordered_set<Base*>::iterator it = objects.find(object);
		if(it != objects.end())
		{
			objects.erase(it);
		}
	}
//
//  // for some reason the following code does not work due to buggy template support?
//	template<typename MarkVisitor>
//	void gc(Base* root)
//	{
//		std::tr1::unordered_set<Base*> temporary(objects);
//
//		MarkVisitor visitor_impl(GarbageCollectorWrapper<Base>(temporary));
//		//visitor_impl;
//		//visitor_impl.visit(*root);
//		//visitor_impl.dummy();
//
//		printf("objects to remove = %ld\n", temporary.size());
//		//foreach(i, temporary)
//		for(typename std::tr1::unordered_set<Base*>::iterator i = temporary.begin(); i != temporary.end(); ++i)
//		{
//			remove(*i);
//			delete *i;
//		}
//	}

	void sweep(std::tr1::unordered_set<Base*>& nonreachable_set)
	{
		//printf("objects to remove = %ld\n", nonreachable_set.size());
		//foreach(i, nonreachable_set)
		for(typename std::tr1::unordered_set<Base*>::iterator i = nonreachable_set.begin(); i != nonreachable_set.end(); ++i)
		{
			remove(*i);
			delete *i;
		}
	}

	std::size_t cleanup()
	{
		std::size_t object_count_to_remove = objects.size();
		// TODO somehow this is not supported by GCC 4.4, so we have to explicit specify the iterator types
		//foreach(i, objects)
		for(typename std::tr1::unordered_set<Base*>::iterator i = objects.begin(); i != objects.end(); ++i)
		{
			delete *i;
		}
		objects.clear();
		return object_count_to_remove;
	}

	std::tr1::unordered_set<Base*> objects;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_GARBAGECOLLECTOR_H_ */
