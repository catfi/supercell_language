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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_TYPEINFERENCEVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_TYPEINFERENCEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/resolver/TypeResolver.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct TypeInferenceVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(inferInvoker, infer)

	TypeInferenceVisitor(resolver::TypeResolver& type_resolver) : type_resolver(type_resolver), resolved_count(0), unresolved_count(0)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(inferInvoker)
	}

	void infer(ASTNode& node)
	{
		revisit(node);
	}

	void infer(Package& node)
	{
		type_resolver.enterScope(node);
		revisit(node);
		type_resolver.leaveScope(node);
	}

	void infer(ClassDecl& node)
	{
		type_resolver.enterScope(node);
		revisit(node);
		type_resolver.leaveScope(node);
	}

	void infer(Import& node)
	{
		//type_resolver.enterScope(node);
		//revisit(node);
	}

	void infer(TypeSpecifier& node)
	{
		if(node.type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			if(type_resolver.resolve(node))
				++resolved_count;
			else
				++unresolved_count;
		}
	}

	std::size_t get_unresolved_count()
	{
		return unresolved_count;
	}

	std::size_t get_resolved_count()
	{
		return resolved_count;
	}

	void reset_count()
	{
		resolved_count = 0;
		unresolved_count = 0;
	}

private:
	resolver::TypeResolver& type_resolver;
	std::size_t resolved_count;
	std::size_t unresolved_count;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_TYPEINFERENCEVISITOR_H_ */
