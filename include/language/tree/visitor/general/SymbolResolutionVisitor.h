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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_SYMBOLRESOLUTIONVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_SYMBOLRESOLUTIONVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct SymbolResolutionVisitor : Visitor<ASTNode, void, VisitorImplementation::iterative_bfs>
{
	CREATE_INVOKER(resolveInvoker, resolve)

	SymbolResolutionVisitor(bool allow_template_partial_match) : allow_template_partial_match(allow_template_partial_match)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(resolveInvoker)
	}

	void resolve(ASTNode& node)
	{
		// default dummy implementation
	}

	void resolve(Block& node)
	{
		foreach(i, node.objects)
			visit(**i);
	}

	void resolve(Program& node)
	{
		visit(*node.root);
	}

	void resolve(Package& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.id, is_template_partial_match))
		{
			if(!isLast())
			{
				next();
				foreach(i, node.children)
					visit(**i);
				foreach(i, node.objects)
					visit(**i);
				prev();
			}
		}
	}

	void resolve(ClassDecl& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(!isLast())
			{
				next();
				foreach(i, node.member_functions)
					visit(**i);
				foreach(i, node.member_variables)
					visit(**i);
				prev();
			}
		}
	}

	void resolve(FunctionDecl& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(isLast())
			{
				candidates.push_back(&node);
				partial_match_flags.push_back(is_template_partial_match);
			}
		}
	}

	void resolve(VariableDecl& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(isLast())
			{
				candidates.push_back(&node);
				partial_match_flags.push_back(is_template_partial_match);
			}
		}
	}

	void resolve(DeclarativeStmt& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(isLast())
			{
				candidates.push_back(&node);
				partial_match_flags.push_back(is_template_partial_match);
			}
		}
	}

private:
	bool isLast()
	{
		if(!full)
			return true;

		if(current_index == full->identifier_list.size() - 1)
			return true;

		return false;
	}

	bool isBegin()
	{
		if(!full)
			return true;

		if(current_index == 0)
			return true;

		return false;
	}

	void next()
	{
		BOOST_ASSERT(!isLast());
		current = full->identifier_list[++current_index];
	}

	void prev()
	{
		BOOST_ASSERT(!isBegin());
		current = full->identifier_list[--current_index];
	}

	bool compare(Identifier* a, Identifier* b, bool& is_template_partial_match)
	{
		BOOST_ASSERT(!isa<NestedIdentifier>(a) && "there shouldn't be any nested identifier within nested identifier");
		BOOST_ASSERT(!isa<NestedIdentifier>(b) && "there shouldn't be any nested identifier within nested identifier");

		is_template_partial_match = false;

		if(isa<TemplatedIdentifier>(a) && isa<TemplatedIdentifier>(b))
		{
			TemplatedIdentifier* ta = cast<TemplatedIdentifier>(a);
			TemplatedIdentifier* tb = cast<TemplatedIdentifier>(b);
			if(ta->id->toString() == tb->id->toString())
			{
				if(ta->templated_type_list.size() == tb->templated_type_list.size())
				{
					return true;
				}
				else
				{
					is_template_partial_match = true;
					return allow_template_partial_match;
				}
			}
			else
			{
				return false;
			}
		}
		else if(!isa<TemplatedIdentifier>(a) && !isa<TemplatedIdentifier>(b))
		{
			BOOST_ASSERT(isa<SimpleIdentifier>(a));
			BOOST_ASSERT(isa<SimpleIdentifier>(b));
			return (a->toString() == b->toString());
		}
		else
		{
			if(isa<TemplatedIdentifier>(a)) a = cast<TemplatedIdentifier>(a)->id;
			if(isa<TemplatedIdentifier>(b)) b = cast<TemplatedIdentifier>(a)->id;
			if(a->toString() == b->toString())
			{
				is_template_partial_match = true;
				return allow_template_partial_match;
			}
			else
			{
				return false;
			}
		}
	}

	Identifier* current;
	int current_index;
	NestedIdentifier* full;
	bool allow_template_partial_match;

public:
	std::vector<const ASTNode*> candidates;
	std::vector<bool> partial_match_flags;
};

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_SYMBOLRESOLUTIONVISITOR_H_ */
