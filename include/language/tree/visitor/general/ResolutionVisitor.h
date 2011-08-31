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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_RESOLUTIONVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_RESOLUTIONVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/visitor/general/GenericVisitor.h"
#include "language/tree/ASTNodeFactory.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct ResolutionVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(resolveInvoker, resolve)

	struct Target
	{
		enum type {
			SYMBOL,
			TYPE,
			PACKAGE,
		};
	};

	ResolutionVisitor(bool allow_template_partial_match = false) : type(type), allow_template_partial_match(allow_template_partial_match)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(resolveInvoker)
		reset();
	}

	void resolve(ASTNode& node)
	{
		// default dummy implementation
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
			if(type == Target::TYPE || type == Target::SYMBOL)
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
			else if(type == Target::PACKAGE)
			{
				if(isLast())
				{
					candidates.push_back(&node);
					partial_match_flags.push_back(is_template_partial_match);
				}
				else
				{
					next();
					foreach(i, node.children)
						visit(**i);
					prev();
				}
			}
		}
	}

	void resolve(ClassDecl& node)
	{
		BOOST_ASSERT(type != Target::PACKAGE);

		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(type == Target::TYPE)
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidcate list
					candidates.push_back(&node);
					partial_match_flags.push_back(is_template_partial_match);
				}
			}
			else if(type == Target::SYMBOL)
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
	}

	void resolve(EnumDecl& node)
	{
		BOOST_ASSERT(type != Target::PACKAGE);

		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(type == Target::TYPE)
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidcate list
					candidates.push_back(&node);
					partial_match_flags.push_back(is_template_partial_match);
				}
			}
			else if(type == Target::SYMBOL)
			{
				if(!isLast())
				{
					next();
					foreach(i, node.enumeration_list)
					{
						visit(*i->first);
					}
					prev();
				}
			}
		}
	}

	void resolve(InterfaceDecl& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.name, is_template_partial_match))
		{
			if(type == Target::TYPE)
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidcate list
					candidates.push_back(&node);
					partial_match_flags.push_back(is_template_partial_match);
				}
			}
			else if(type == Target::SYMBOL)
			{
				if(!isLast())
				{
					next();
					foreach(i, node.member_functions)
						visit(**i);
					prev();
				}
			}
		}
	}

	void resolve(TypedefDecl& node)
	{
		bool is_template_partial_match = false;
		if(compare(current, node.to, is_template_partial_match))
		{
			if(type == Target::TYPE)
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidcate list
					candidates.push_back(&node);
					partial_match_flags.push_back(is_template_partial_match);
				}
			}
		}
	}

	void resolve(FunctionDecl& node)
	{
		if(type == Target::SYMBOL)
		{
			// try to match function name
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

			// try to match parameters in the function
			{
				foreach(i, node.parameters)
				{
					bool is_template_partial_match = false;
					if(compare(current, i->first, is_template_partial_match))
					{
						if(isLast())
						{
							candidates.push_back(i->first);
							partial_match_flags.push_back(is_template_partial_match);
						}
					}
				}
			}
		}
	}

	void resolve(VariableDecl& node)
	{
		if(type == Target::SYMBOL)
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
	}

	void resolve(DeclarativeStmt& node)
	{
		visit(*node.declaration);
	}

	void resolve(ForeachStmt& node)
	{
		if(type == Target::SYMBOL)
		{
			// TODO handle the local variable
		}
	}

	void target(Target::type type, Identifier* id)
	{
		BOOST_ASSERT(id != NULL);

		this->type = type;

		if(isa<NestedIdentifier>(id))
		{
			full = cast<NestedIdentifier>(id);
			BOOST_ASSERT(full->identifier_list.size() > 0 && "given nested identifier has no nested identifier");

			current_index = 0;
			current = full->identifier_list[0];
		}
		else
		{
			full = NULL;
			current_index = 0;
			current = id;
		}
	}

	void reset()
	{
		candidates.clear();
		partial_match_flags.clear();
		current_index = 0;
		full = NULL;
		current = NULL;
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

	Target::type type;
	Identifier* current;
	int current_index;
	NestedIdentifier* full;
	bool allow_template_partial_match;

public:
	std::vector<ASTNode*> candidates;
	std::vector<bool> partial_match_flags;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_RESOLUTIONVISITOR_H_ */
