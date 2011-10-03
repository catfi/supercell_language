/**
 * Zillians MMO
 * Copyright (C) 2007-2011 Zillians.com, Inc.
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

#ifndef ZILLIANS_LANGUAGE_TRANSFORMERCONTEXT_H_
#define ZILLIANS_LANGUAGE_TRANSFORMERCONTEXT_H_

#include "language/tree/ASTNodeFactory.h"

namespace zillians { namespace language {

/**
 * SplitReferenceContext is used when AST is transformed and new nodes are created.
 * The newly created nodes will have this context pointing to the origin node where this node is split from
 */
struct SplitReferenceContext
{
	explicit SplitReferenceContext(tree::ASTNode* ref) : ref(ref)
	{ }

	static tree::ASTNode* get(tree::ASTNode* node)
	{
		SplitReferenceContext* from = node->get<SplitReferenceContext>();
		if(from)
			return from->ref;
		else
			return NULL;
	}

	static void set(tree::ASTNode* node, tree::ASTNode* ref)
	{
		return node->set<SplitReferenceContext>(new SplitReferenceContext(ref));
	}

	tree::ASTNode* ref;
};

} }

#endif /* ZILLIANS_LANGUAGE_TRANSFORMERCONTEXT_H_ */
