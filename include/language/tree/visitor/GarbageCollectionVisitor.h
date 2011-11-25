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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GARBAGECOLLECTIONVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GARBAGECOLLECTIONVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/GenericComposableVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

template<bool Composed = false>
struct GarbageCollectionVisitor : GenericVisitor
{
    CREATE_GENERIC_INVOKER(markInvoker);

	GarbageCollectionVisitor() : nonreachable_set(ASTNodeGC::instance()->objects)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(markInvoker)
	}

	~GarbageCollectionVisitor()
	{
		ASTNodeGC::instance()->sweep(nonreachable_set);
	}

	inline std::size_t get_sweep_count()
	{
		return nonreachable_set.size();
	}

	void apply(ASTNode& node)
	{
		nonreachable_set.erase(&node);
		if(!Composed)
			GenericVisitor::apply(node);
	}

	std::tr1::unordered_set<const ASTNode*> nonreachable_set;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GARBAGECOLLECTIONVISITOR_H_ */
