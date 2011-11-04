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
#include "language/tree/visitor/GenericVisitor.h"
#include "language/tree/visitor/NodeInfoVisitor.h"
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

	ResolutionVisitor(bool allow_template_partial_match = false) : matched_current(false), filter_type(0), allow_template_partial_match(allow_template_partial_match)
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
			LOG4CXX_ERROR(LoggerWrapper::Resolver, L"resolution visitor is trying to resolve type on unspecified node \"" << node_info_visitor.stream.str() << L"\"");

			ASTNode* resolved_type = ResolvedType::get(&node);
			if(resolved_type)
				tryFollow(*resolved_type);
		}

		if(isSearchForSymbol())
		{
			LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"resolution visitor is trying to resolve symbol on unspecified node \"" << node_info_visitor.stream.str() << L"\"");

			ASTNode* resolved_symbol = ResolvedSymbol::get(&node);
			if(resolved_symbol)
				tryFollow(*resolved_symbol);

			ASTNode* resolved_package = ResolvedPackage::get(&node);
			if(resolved_package)
				tryFollow(*resolved_package);

		}

		if(isSearchForPackage())
		{
			LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"resolution visitor is trying to resolve package on unspecified node \"" << node_info_visitor.stream.str() << L"\"");

			ASTNode* resolved_package = ResolvedPackage::get(&node);
			if(resolved_package)
				tryFollow(*resolved_package);
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
				{
					tryFollow(*resolved_type);
				}
				break;
			}
			}
		}

		if(isSearchForSymbol())
		{
			ASTNode* resolved_type = ResolvedType::get(&node);
			if(resolved_type)
			{
				tryFollow(*resolved_type);
			}
		}
	}

	void resolve(Tangle& node)
	{
		foreach(i, node.sources)
			tryVisit(*(i->second));
	}

	void resolve(Source& node)
	{
		tryVisit(*node.root);
	}

	void resolve(Import& node)
	{
		// find the corresponding source node
		Tangle* tangle = ASTNodeHelper::getOwner<Tangle>(&node);
		auto corresponding_sources = tangle->sources.equal_range(node.ns);

		if(isSearchForType() || isSearchForSymbol())
		{
			if(node.alias)
			{
				if(node.alias->isEmpty() || isMatched())
				{
					// if it's a global alias, all symbols or types of the imported package should be directly visible to the current scope
					// so we visit the corresponding package directly
					for(auto i = corresponding_sources.first; i != corresponding_sources.second; ++i)
					{
						// find corresponding package for each matched sources
						Package* corresponding_package = i->second->root;
						while(true)
						{
							if(corresponding_package->children.size() == 0) break;

							BOOST_ASSERT(corresponding_package->children.size() == 1 && "package should have exactly one sub-package in the new package system");
							corresponding_package = corresponding_package->children[0];
						}

						tryVisit(*corresponding_package);
					}

				}
				else
				{
					// if it's a named alias and is not matched, we should first match the alias and then dig into the imported package
					bool is_template_partial_match = false;
					if(compare(current, node.alias, is_template_partial_match) && !is_template_partial_match)
					{
						if(!isLast())
						{
							next();
							for(auto i = corresponding_sources.first; i != corresponding_sources.second; ++i)
							{
								// find corresponding package for each matched sources
								Package* corresponding_package = i->second->root;
								while(true)
								{
									if(corresponding_package->children.size() == 0) break;

									BOOST_ASSERT(corresponding_package->children.size() == 1 && "package should have exactly one sub-package in the new package system");
									corresponding_package = corresponding_package->children[0];
								}

								tryVisit(*corresponding_package);
							}
							prev();
						}
					}
				}
			}
			else
			{
				// if it's an unnamed import, try to visit the root package of the corresponding source node
				for(auto i = corresponding_sources.first; i != corresponding_sources.second; ++i)
				{
					tryVisit(*i->second->root);
				}

			}
		}

		if(isSearchForPackage())
		{
			if(node.alias)
			{
				// package can only be matched when it's a named alias
				// note that there can't be any package within the corresponding package due to the new package system design
				if(!node.alias->isEmpty())
				{
					bool is_template_partial_match = false;
					if(compare(current, node.alias, is_template_partial_match) && !is_template_partial_match)
					{
						if(isLast())
						{
							enlist(&node, is_template_partial_match);
						}
					}
				}
			}
			else
			{
				// if it's an unnamed import, try to visit the root package of the corresponding source node
				for(auto i = corresponding_sources.first; i != corresponding_sources.second; ++i)
				{
					tryVisit(*i->second->root);
				}
			}
		}
	}

	void resolve(Package& node)
	{
		// the root package always match
		// otherwise we have to make sure the package matches the the required identifier and advance to next identifier
		if(node.id->isEmpty() || isMatched())
		{
			if(isSearchForType() || isSearchForSymbol())
			{
				foreach(i, node.children)
					tryMatch(**i);
				foreach(i, node.objects)
					tryMatch(**i);
			}

			if(isSearchForPackage())
			{
				foreach(i, node.children)
					tryMatch(**i);
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
							tryMatch(**i);
						prev();
					}
				}

				foreach(i, node.objects)
					tryMatch(**i);
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
							tryMatch(**i);
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
					tryMatch(**i);
			}
		}
	}

	void resolve(ClassDecl& node)
	{
		if(isMatched())
		{
			if(isSearchForSymbol())
			{
				if(isLast())
				{
					foreach(i, node.member_functions)
						tryMatch(**i);
					foreach(i, node.member_variables)
						tryMatch(**i);

					if(node.base)
						tryVisit(*node.base);
					foreach(i, node.implements)
						tryVisit(**i);
				}
			}
		}
		else
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
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					if(isLast())
					{
						enlist(&node, is_template_partial_match);
					}
					else
					{
						foreach(i, node.member_functions)
							tryMatch(**i);
						foreach(i, node.member_variables)
							tryMatch(**i);

						if(node.base)
							tryVisit(*node.base);
						foreach(i, node.implements)
							tryVisit(**i);
					}
				}
			}
		}
	}

	void resolve(EnumDecl& node)
	{
		if(isMatched())
		{
			if(isSearchForSymbol())
			{
				if(isLast())
				{
					foreach(i, node.values)
					{
						tryMatch(**i);
					}
				}
			}
		}
		else
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
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					if(isLast())
					{
						enlist(&node, is_template_partial_match);
					}
					else
					{
						next();
						foreach(i, node.values)
						{
							tryMatch(**i);
						}
						prev();
					}
				}
			}
		}
	}

	void resolve(InterfaceDecl& node)
	{
		if(isMatched())
		{
			if(isSearchForSymbol())
			{
				if(isLast())
				{
					foreach(i, node.member_functions)
						tryMatch(**i);
				}
			}
		}
		else
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
				bool is_template_partial_match = false;
				if(compare(current, node.name, is_template_partial_match))
				{
					if(isLast())
					{
						UNREACHABLE_CODE();
					}
					else
					{
						next();
						foreach(i, node.member_functions)
							tryMatch(**i);
						prev();
					}
				}
			}
		}
	}

	void resolve(TypedefDecl& node)
	{
		if(isMatched())
		{
			if(isSearchForType())
			{
				UNREACHABLE_CODE();
			}
		}
		else
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
	}

	void resolve(FunctionDecl& node)
	{
		if(isMatched())
		{
			if(isSearchForSymbol())
			{
				// if the current scope is root scope, then we can access the parameters
				// try to match parameters in the function
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
		else
		{
			if(isSearchForSymbol())
			{
				// try to match function name
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

	void resolve(VariableDecl& node)
	{
		if(isMatched())
		{
			ASTNode* resolved_type = ResolvedType::get(node.type);
			if(resolved_type)
			{
				tryVisit(*resolved_type);
			}
		}
		else
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
						ASTNode* resolved_type = ResolvedType::get(node.type);
						if(resolved_type)
						{
							tryVisit(*resolved_type);
						}
					}
				}
			}
		}
	}

	void resolve(DeclarativeStmt& node)
	{
		if(isSearchForSymbol())
		{
			tryFollow(*node.declaration);
		}
	}

	void resolve(ForeachStmt& /*node*/)
	{
		if(isSearchForSymbol())
		{
			UNREACHABLE_CODE();
			// TODO handle the local variable
		}
	}

//	void resolve(Expression& node)
//	{
//		if(isSearchForType())
//		{
//			// for PrimaryExpr, the expression can be resolved to some package declaration
//			ASTNode* package = ResolvedPackage::get(&node);
//			if(package)
//				visit(*package);
//
//			// TODO handle lambda case
//		}
//
//		if(isSearchForSymbol())
//		{
//			// there can be symbol within package or another symbol or another type
//			ASTNode* type = ResolvedType::get(&node);
//			ASTNode* symbol = ResolvedSymbol::get(&node);
//			ASTNode* package = ResolvedPackage::get(&node);
//
//			// the only reason we reach here is we are visiting member expression and we want to find the member of the LHS, which can be a PrimaryExpr
//
//			// note that because whenever we resolved a symbol, we store the related type information using ResolvedType to that ASTNode
//			// and for most cases, these two are the same, but for resolution to VariableDecl, for example,
//			// the resolved type is the TypeSpecifier of that VariableDecl (not the VariableDecl itself),
//			// then we should visit the type as well as the resolved symbol
//			// that's why we put a condition here "type != symbol"
//			if(symbol)
//			{
//				tryFollow(*symbol);
////				if(isa<VariableDecl>(symbol))
////				{
////					// there can be member variable or member function in a non-primitive variable
////					// for example:
////					//
////					// class MyClass { function f() { } }
////					// ...
////					// var v:MyClass;
////					// v.f();
////					//
////					visit(*type);
////				}
////				else if(isa<ClassDecl>(symbol))
////				{
////					// there can be static member variable or static member function inside the resolved ClassDecl
////					// for example:
////					//
////					// class MyClass { static function f() { } }
////					// ...
////					// MyClass.f();
////					//
////					visit(*symbol);
////				}
////				else if(isa<EnumDecl>(symbol))
////				{
////					// there can be enumeration value inside the resolved EnumDecl
////					// for example:
////					//
////					// enum MyEnum { ABC, DEF }
////					// var v = MyEnum.ABC;
////					//
////					visit(*symbol);
////				}
////				else
////				{
////					// the rest of types are not qualified for symbol resolution
////					// (i.e. there can't be any symbol defined in a primary expression that is resolved to a FunctionDecl, InterfaceDecl,
////				}
//			}
//			else
//			{
//				visit(*package);
//			}
//			// TODO handle lambda case
//		}
//
//		if(isSearchForPackage())
//		{
//			// for PrimaryExpr, the expression can be resolved to some package declaration
//			ASTNode* package = ResolvedPackage::get(&node);
//			if(package)
//				visit(*package);
//			// TODO handle lambda case
//		}
//	}

public:
	void candidate(Identifier* id)
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
		matched_current = false;
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

public:
	/**
	 * Try to match the current identifier against the given node by comparing the current identifer with node's identifier
	 *
	 * @param node the node to match
	 */
	inline void tryMatch(ASTNode& node)
	{
		// push matched_current to stack
		bool t = matched_current; matched_current = false;

		// visit given node
		visit(node);

		// and pop it back when done visiting the given node
		matched_current = t;
	}

	/**
	 * Try to visit/traverse given node
	 *
	 * Note that the difference between tryMatch() and tryVisit() is that tryMatch() assume the given node is a possible candidate to match
	 * while tryVisit() assure that the node has been matched, so we only need to compare the current identifier with its children
	 *
	 * @param node the node to visit
	 */
	inline void tryVisit(ASTNode& node)
	{
		// push matched_current to stack
		bool t = matched_current; matched_current = true;

		// visit given node
		visit(node);

		// and pop it back when done visiting the given node
		matched_current = t;
	}

	inline void tryFollow(ASTNode& node)
	{
		visit(node);
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

	inline bool isMatched()
	{
		return matched_current;
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

	bool matched_current;

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
