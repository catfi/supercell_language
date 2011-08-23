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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_STAGE1_STRUCTUREVERIFICATIONVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_STAGE1_STRUCTUREVERIFICATIONVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericComposableVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor { namespace stage1 {

template<bool Composed = false>
struct StructureVerificationVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(verifyInvoker, verify);

	StructureVerificationVisitor() : passed(true)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(verifyInvoker)
	}

	void verify(ASTNode& node)
	{
		if(!Composed)
			revisit(node);
	}

	void verify(TypeSpecifier& node)
	{
		if(node.type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			unspecified_nodes.push_back(&node);
			passed = false;
		}
		if(!Composed)
			revisit(node);
	}

	std::vector<ASTNode*> unspecified_nodes;
	bool passed;
};

} } } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_STAGE1_STRUCTUREVERIFICATIONVISITOR_H_ */
