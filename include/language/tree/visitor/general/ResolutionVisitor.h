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
#include "language/tree/visitor/general/NodeInfoVisitor.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/context/ResolverContext.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct ResolutionVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(resolveInvoker, resolve)

	struct Filter
	{
		enum type {
			SYMBOL  = 1,
			TYPE    = 2,
			PACKAGE = 4,
		};
	};

	ResolutionVisitor(bool allow_template_partial_match = false) : filter_type(0), current_search_level(0), allow_template_partial_match(allow_template_partial_match)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(resolveInvoker)
		reset();
	}

	void resolve(ASTNode& node)
	{
		NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(node);

		if(isSearchForType())
		{
			LOG4CXX_ERROR(LoggerWrapper::Compiler, L"resolution visitor is trying to resolve type on unspecified node \"" << node_info_visitor.stream.str() << L"\"");

			ASTNode* r = ResolvedType::get(&node);
			if(r)
				visit(*r);
		}

		if(isSearchForSymbol())
		{
			LOG4CXX_DEBUG(LoggerWrapper::Compiler, L"resolution visitor is trying to resolve symbol on unspecified node \"" << node_info_visitor.stream.str() << L"\"");

			ASTNode* r = ResolvedSymbol::get(&node);
			if(r)
				visit(*r);
		}

		if(isSearchForPackage())
		{
			LOG4CXX_DEBUG(LoggerWrapper::Compiler, L"resolution visitor is trying to resolve package on unspecified node \"" << node_info_visitor.stream.str() << L"\"");

			ASTNode* r = ResolvedPackage::get(&node);
			if(r)
				visit(*r);
		}

	}

	void resolve(TypeSpecifier& node)
	{
		if(isSearchForType())
		{
			// if the type specifier has been resolved, then we should follow the type it's referring to
			// note that we don't increase search level because TypeSpecifier is basically transparent
			// (so we use 'visit()' instead of 'tryVisit()')
			switch(node.type)
			{
			case TypeSpecifier::ReferredType::FUNCTION_TYPE:
			{
				enlist(&node, false);
				break;
			}
			case TypeSpecifier::ReferredType::PRIMITIVE:
			{
				enlist(&node, false);
				break;
			}
			case TypeSpecifier::ReferredType::UNSPECIFIED:
			{
				// it's possible for resolving to unresolved type specifier,
				// and that's why we need iterative type resolution
				ASTNode* resolved_type = ResolvedType::get(&node);
				if(resolved_type)
					visit(*resolved_type);
				break;
			}
			}
		}

		if(isSearchForSymbol())
		{
			ASTNode* resolved_type = ResolvedType::get(&node);
			if(resolved_type)
				visit(*resolved_type);
		}
	}

	void resolve(Program& node)
	{
		tryVisit(*node.root);
	}

	void resolve(Package& node)
	{
		// the root package always match
		// otherwise we have to make sure the package matches the the required identifier and advance to next identifier
		if(node.id->isEmpty() || isAtRootScope())
		{
			if(isSearchForType() || isSearchForSymbol())
			{
				foreach(i, node.children)
					visit(**i);
				foreach(i, node.objects)
					visit(**i);
			}

			if(isSearchForPackage())
			{
				foreach(i, node.children)
					tryVisit(**i);
			}
		}
		else
		{
			if(isSearchForType() || isSearchForSymbol())
			{
				bool is_template_partial_match = false;
				if(compare(current, node.id, is_template_partial_match) && !is_template_partial_match)
				{
					if(!isLast())
					{
						next();
						foreach(i, node.children)
							tryVisit(**i);
						prev();
					}
				}

				foreach(i, node.objects)
					tryVisit(**i);
			}

			if(isSearchForPackage())
			{
				bool is_template_partial_match = false;
				if(compare(current, node.id, is_template_partial_match) && !is_template_partial_match)
				{
					if(isLast())
					{
						enlist(&node, is_template_partial_match);
					}
					else
					{
						next();
						foreach(i, node.children)
							tryVisit(**i);
						prev();
					}
				}
			}
		}
	}

	void resolve(Block& node)
	{
		if(isSearchForSymbol())
		{
			foreach(i, node.objects)
			{
				if(isa<DeclarativeStmt>(*i))
					visit(**i);
			}
		}
	}

	void resolve(ClassDecl& node)
	{
		if(isSearchForType())
		{
			bool is_template_partial_match = false;
			if(compare(current, node.name, is_template_partial_match))
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidate list
					enlist(&node, is_template_partial_match);
				}
			}
		}

		if(isSearchForSymbol())
		{
			if(isAtRootScope())
			{
				if(isLast())
				{
					foreach(i, node.member_functions)
						tryVisit(**i);
					foreach(i, node.member_variables)
						tryVisit(**i);

					if(node.base) visit(*node.base);
					foreach(i, node.implements) visit(**i);
				}
			}
			else
			{
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					if(isLast())
					{
						enlist(&node, is_template_partial_match);
					}
				}
			}
		}
	}

	void resolve(EnumDecl& node)
	{
		if(isSearchForType())
		{
			bool is_template_partial_match = false;
			if(compare(current, node.name, is_template_partial_match))
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidate list
					enlist(&node, is_template_partial_match);
				}
			}
		}

		if(isSearchForSymbol())
		{
			if(isAtRootScope())
			{
				if(isLast())
				{
					foreach(i, node.enumeration_list)
					{
						bool is_template_partial_match = false;
						if(compare(current, i->first, is_template_partial_match))
						{
							enlist(i->first, is_template_partial_match);
						}
					}
				}
			}
			else
			{
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					next();
					if(isLast())
					{
						foreach(i, node.enumeration_list)
						{
							bool is_template_partial_match = false;
							if(compare(current, i->first, is_template_partial_match))
							{
								enlist(i->first, is_template_partial_match);
							}
						}
					}
					prev();
				}
			}
		}
	}

	void resolve(InterfaceDecl& node)
	{
		if(isSearchForType())
		{
			bool is_template_partial_match = false;
			if(compare(current, node.name, is_template_partial_match))
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidate list
					enlist(&node, is_template_partial_match);
				}
			}
		}

		if(isSearchForSymbol())
		{
			if(!isLast())
			{
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					next();
					if(isLast())
					{
						foreach(i, node.member_functions)
							tryVisit(**i);
					}
					prev();
				}
			}
		}
	}

	void resolve(TypedefDecl& node)
	{
		if(isSearchForType())
		{
			bool is_template_partial_match = false;
			if(compare(current, node.name, is_template_partial_match))
			{
				if(isLast())
				{
					// reach the end of nested identifier, and we end up with ClassDecl, which is a type
					// save to candidcate list
					enlist(node.type, is_template_partial_match);
				}
				else
				{
					next();
					tryVisit(*node.type);
					prev();
				}
			}
		}
	}

	void resolve(FunctionDecl& node)
	{
		if(isSearchForSymbol())
		{
			// try to match function name
			{
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					if(isLast())
					{
						enlist(&node, is_template_partial_match);
					}
				}
			}

			// if the current scope is root scope, then we can access the parameters
			// try to match parameters in the function
			if(isAtRootScope())
			{
				foreach(i, node.parameters)
				{
					bool is_template_partial_match = false;
					if(compare(current, (*i)->name, is_template_partial_match))
					{
						if(isLast())
						{
							enlist(*i, is_template_partial_match);
						}
					}
				}
			}
		}
	}

	void resolve(VariableDecl& node)
	{
		if(isSearchForSymbol())
		{
			bool is_template_partial_match = false;
			if(compare(current, node.name, is_template_partial_match))
			{
				if(isLast())
				{
					enlist(&node, is_template_partial_match);
				}
				else
				{
					ASTNode* resolved_type = ResolvedType::get(&node);
					if(resolved_type)
					{
						tryVisit(*resolved_type);
					}
				}
			}
		}
	}

	void resolve(DeclarativeStmt& node)
	{
		if(isSearchForSymbol())
		{
			// note that we don't increase search level because TypeSpecifier is basically transparent
			// (so we use 'visit()' instead of 'tryVisit()')
			visit(*node.declaration);
		}
	}

	void resolve(ForeachStmt& node)
	{
		if(isSearchForSymbol())
		{
			// TODO handle the local variable
		}
	}

	void resolve(Expression& node)
	{
		if(isSearchForType())
		{
			// for PrimaryExpr, the expression can be resolved to some package declaration
			ASTNode* package = ResolvedPackage::get(&node);
			if(package)
				visit(*package);

			// TODO handle lambda case
		}

		if(isSearchForSymbol())
		{
			// there can be symbol within package or another symbol or another type
			ASTNode* type = ResolvedType::get(&node);
			ASTNode* symbol = ResolvedSymbol::get(&node);
			ASTNode* package = ResolvedPackage::get(&node);

			// the only reason we reach here is we are visiting member expression and we want to find the member of the LHS, which can be a PrimaryExpr

			// note that because whenever we resolved a symbol, we store the related type information using ResolvedType to that ASTNode
			// and for most cases, these two are the same, but for resolution to VariableDecl, for example,
			// the resolved type is the TypeSpecifier of that VariableDecl (not the VariableDecl itself),
			// then we should visit the type as well as the resolved symbol
			// that's why we put a condition here "type != symbol"
			if(!package)
			{
				if(isa<VariableDecl>(symbol))
				{
					// there can be member variable or member function in a non-primitive variable
					// for example:
					//
					// class MyClass { function f() { } }
					// ...
					// var v:MyClass;
					// v.f();
					//
					visit(*type);
				}
				else if(isa<ClassDecl>(symbol))
				{
					// there can be static member variable or static member function inside the resolved ClassDecl
					// for example:
					//
					// class MyClass { static function f() { } }
					// ...
					// MyClass.f();
					//
					visit(*symbol);
				}
				else if(isa<EnumDecl>(symbol))
				{
					// there can be enumeration value inside the resolved EnumDecl
					// for example:
					//
					// enum MyEnum { ABC, DEF }
					// var v = MyEnum.ABC;
					//
					visit(*symbol);
				}
				else
				{
					// the rest of types are not qualified for symbol resolution
					// (i.e. there can't be any symbol defined in a primary expression that is resolved to a FunctionDecl, InterfaceDecl,
				}
			}
			else
			{
				visit(*package);
			}
			// TODO handle lambda case
		}

		if(isSearchForPackage())
		{
			// for PrimaryExpr, the expression can be resolved to some package declaration
			ASTNode* package = ResolvedPackage::get(&node);
			if(package)
				visit(*package);
			// TODO handle lambda case
		}
	}

public:
	void search(Identifier* id)
	{
		BOOST_ASSERT(id != NULL);

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

	void filter(Filter::type type, bool enable = true)
	{
		if(enable)
			filter_type |= (int)type;
		else
			filter_type &= ~((int)type);
	}

	bool enlist(ASTNode* node, bool is_template_partial_match)
	{
		foreach(i, candidates)
			if(*i == node)
				return false;

		candidates.push_back(node);
		partial_match_flags.push_back(is_template_partial_match);

		return true;
	}

	void reset()
	{
		filter_type = 0;
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
			if(isa<TemplatedIdentifier>(b)) b = cast<TemplatedIdentifier>(b)->id;
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
	/**
	 * Determine whether we have matched any identifier or not
	 *
	 * @return true if we haven't matched any identifier, false otherwise
	 */
	inline bool isAtRootScope()
	{
		return (current_search_level == 0);
	}

	inline bool isSearchForType()
	{
		return filter_type & ((int)Filter::TYPE);
	}

	inline bool isSearchForSymbol()
	{
		return filter_type & ((int)Filter::SYMBOL);
	}

	inline bool isSearchForPackage()
	{
		return filter_type & ((int)Filter::PACKAGE);
	}

	/**
	 * Search and increase the search level, which means there's another level of depth in the search tree
	 *
	 * Note that search level can only be introduced by a identifier match
	 *
	 * @param node
	 */
	inline void tryVisit(ASTNode& node)
	{
		++current_search_level;
		visit(node);
		--current_search_level;
	}

private:
	bool isLast()
	{
		if(!full)
			return true;

		if(current_index == (int)full->identifier_list.size() - 1)
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

	int filter_type;
	Identifier* current;
	int current_index;
	int current_search_level;
	NestedIdentifier* full;
	bool allow_template_partial_match;

public:
	std::vector<ASTNode*> candidates;
	std::vector<bool> partial_match_flags;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_RESOLUTIONVISITOR_H_ */
