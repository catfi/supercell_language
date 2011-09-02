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

#ifndef ZILLIANS_LANGUAGE_RESOLVER_H_
#define ZILLIANS_LANGUAGE_RESOLVER_H_

#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/general/ResolutionVisitor.h"
#include "language/tree/visitor/general/PrettyPrintVisitor.h"
#include "language/tree/visitor/general/NodeInfoVisitor.h"
#include "utility/Foreach.h"
#include "language/resolver/context/ResolverContext.h"

namespace __gnu_cxx {

template<typename T>
struct hash<T*>
{
	std::size_t operator() (T* value)
	{
		return reinterpret_cast<std::size_t>(value);
	}
};

}

namespace zillians { namespace language {

struct Resolver
{
	Resolver()
	{ }

	void enterScope(tree::ASTNode& node)
	{
		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(node);
		LOG4CXX_DEBUG(Logger::Resolver, L"entering scope: \"" << node_info_visitor.stream.str() << L"\"");

		current_scopes.insert(&node);
	}

	void leaveScope(tree::ASTNode& node)
	{
		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(node);
		LOG4CXX_DEBUG(Logger::Resolver, L"leaving scope: \"" << node_info_visitor.stream.str() << L"\"");

		__gnu_cxx::hash_set<tree::ASTNode*>::iterator scope = current_scopes.find(&node);
		if(scope != current_scopes.end())
		{
			current_scopes.erase(scope);
		}
		else
		{
			LOG4CXX_ERROR(Logger::Resolver, L"leaving unknown scope: \"" << node_info_visitor.stream.str() << L"\"");
		}
	}

	bool resolveSymbol(tree::ASTNode& scope, tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(scope);
		LOG4CXX_DEBUG(Logger::Resolver, L"trying to resolve symbol: \"" << node.toString() << L"\" from scope \"" << node_info_visitor.stream.str() << "\"");
		node_info_visitor.reset();

		if(!ResolvedSymbol::get(&node))
		{
			resolution_visitor.reset();
			resolution_visitor.search(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::SYMBOL);

			{
				tree::visitor::NodeInfoVisitor node_info_visitor;
				node_info_visitor.visit(scope);
				LOG4CXX_DEBUG(Logger::Resolver, L"looking at scope: " << node_info_visitor.stream.str());
			}

			resolution_visitor.visit(scope);

			return checkResolvedSymbol(node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool resolveSymbol(tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		LOG4CXX_DEBUG(Logger::Resolver, L"trying to resolve symbol: \"" << node.toString() << L"\"");

		if(!ResolvedSymbol::get(&node))
		{
			// set the look-for target
			resolution_visitor.reset();
			resolution_visitor.search(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::SYMBOL);

			for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
			{
				tree::visitor::NodeInfoVisitor node_info_visitor;
				node_info_visitor.visit(**scope);
				LOG4CXX_DEBUG(Logger::Resolver, L"looking at scope: \"" << node_info_visitor.stream.str() << L"\"");

				resolution_visitor.visit(**scope);
			}

			return checkResolvedSymbol(node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool checkResolvedSymbol(tree::Identifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

		if(resolution_visitor.candidates.size() == 1)
		{
			ASTNode* ref = resolution_visitor.candidates[0];

			tree::visitor::NodeInfoVisitor node_info_visitor;
			node_info_visitor.visit(*ref);
			LOG4CXX_DEBUG(Logger::Resolver, L"symbol \"" << node.toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
			node_info_visitor.reset();

			bool valid = true;
			if(isa<Identifier>(ref))
			{
				if(!no_action) ResolvedSymbol::set(&node, ref);
			}
			else if(isa<VariableDecl>(ref))
			{
				if(!no_action) ResolvedSymbol::set(&node, ref);
			}
			else if(isa<FunctionDecl>(ref))
			{
				if(!no_action) ResolvedSymbol::set(&node, ref);
			}
			else
			{
				LOG4CXX_ERROR(Logger::Resolver, L"resolve symbol \"" << node.toString() << L"\" to unkown symbol");
				valid = false;
			}

			resolution_visitor.reset();
			return valid;
		}
		else
		{
			if(resolution_visitor.candidates.size() > 1)
			{
				// mode than one candidate
				LOG4CXX_ERROR(Logger::Resolver, L"ambiguous symbol \"" << node.toString() << L"\"");

				tree::visitor::NodeInfoVisitor node_info_visitor;
				foreach(i, resolution_visitor.candidates)
				{
					node_info_visitor.visit(**i);
					LOG4CXX_DEBUG(Logger::Resolver, L"symbol can be resolved to: \"" << node_info_visitor.stream.str());
					node_info_visitor.reset();
				}
			}
			else
			{
				// no candidate
				LOG4CXX_ERROR(Logger::Resolver, L"unresolved symbol \"" << node.toString() << L"\"");
			}

			resolution_visitor.reset();
			return false;
		}
	}

	bool resolveType(tree::ASTNode& scope, tree::TypeSpecifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(scope);
		LOG4CXX_DEBUG(Logger::Resolver, L"trying to resolve type: \"" << node.referred.unspecified->toString() << L"\" from scope \"" << node_info_visitor.stream.str() << L"\"");
		node_info_visitor.reset();

		if(node.type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			resolution_visitor.reset();
			resolution_visitor.search(node.referred.unspecified);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::TYPE);

			resolution_visitor.visit(scope);

			return checkResolvedType(node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool resolveType(tree::TypeSpecifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		LOG4CXX_DEBUG(Logger::Resolver, L"trying to resolve type: \"" << node.referred.unspecified->toString() << L"\"");

		if(node.type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			// set the look-for target
			resolution_visitor.reset();
			resolution_visitor.search(node.referred.unspecified);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::TYPE);

			for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
			{
				resolution_visitor.visit(**scope);
			}

			return checkResolvedType(node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool checkResolvedType(tree::TypeSpecifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

		if(resolution_visitor.candidates.size() == 1)
		{
			ASTNode* ref = resolution_visitor.candidates[0];

			tree::visitor::NodeInfoVisitor node_info_visitor;
			node_info_visitor.visit(*ref);
			LOG4CXX_DEBUG(Logger::Resolver, L"type \"" << node.referred.unspecified->toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
			node_info_visitor.reset();

			bool valid = true;
			if(isa<ClassDecl>(ref))
			{
				LOG4CXX_DEBUG(Logger::Resolver, L"resolve type \"" << node.referred.unspecified->toString() << L"\" to \"" << cast<ClassDecl>(ref)->name->toString() << L"\"");
				if(!no_action) node.update(cast<ClassDecl>(ref));
			}
			else if(isa<InterfaceDecl>(ref))
			{
				if(!no_action) node.update(cast<InterfaceDecl>(ref));
			}
			else if(isa<FunctionDecl>(ref))
			{
				if(!no_action) node.update(cast<FunctionDecl>(ref));
			}
			else if(isa<EnumDecl>(ref))
			{
				if(!no_action) node.update(cast<EnumDecl>(ref));
			}
			else if(isa<TypedefDecl>(ref))
			{
				if(!no_action) node.update(cast<TypedefDecl>(ref));
			}
			else if(isa<FunctionType>(ref))
			{
				if(!no_action) node.update(cast<FunctionType>(ref));
			}
			else
			{
				LOG4CXX_FATAL(Logger::Resolver, L"resolve type \"" << node.referred.unspecified->toString() << L"\" to unknown type");
				valid = false;
			}

			resolution_visitor.reset();
			return valid;
		}
		else
		{
			if(resolution_visitor.candidates.size() > 1)
			{
				// mode than one candidate
				LOG4CXX_ERROR(Logger::Resolver, L"ambiguous type \"" << node.referred.unspecified->toString() << L"\"");

				tree::visitor::NodeInfoVisitor node_info_visitor;
				foreach(i, resolution_visitor.candidates)
				{
					node_info_visitor.visit(**i);
					LOG4CXX_DEBUG(Logger::Resolver, L"type can be resolved to: \"" << node_info_visitor.stream.str() << L"\"");
					node_info_visitor.reset();
				}
			}
			else
			{
				// no candidate
				LOG4CXX_ERROR(Logger::Resolver, L"unresolved type \"" << node.referred.unspecified->toString() << L"\"");
			}

			resolution_visitor.reset();
			return false;
		}
	}

	bool resolvePackage(tree::ASTNode& scope, tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(scope);
		LOG4CXX_DEBUG(Logger::Resolver, L"trying to resolve package: \"" << node.toString() << L"\" from scope \"" << node_info_visitor.stream.str() << L"\"");
		node_info_visitor.reset();

		if(!ResolvedPackage::get(&node))
		{
			resolution_visitor.reset();
			resolution_visitor.search(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::PACKAGE);

			resolution_visitor.visit(scope);

			return checkResolvedPackage(node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool resolvePackage(tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		LOG4CXX_DEBUG(Logger::Resolver, L"trying to resolve package: \"" << node.toString() << L"\"");

		if(!ResolvedPackage::get(&node))
		{
			resolution_visitor.reset();
			resolution_visitor.search(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::PACKAGE);

			for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
			{
				resolution_visitor.visit(**scope);
			}

			return checkResolvedPackage(node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool checkResolvedPackage(tree::Identifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

		if(resolution_visitor.candidates.size() == 1)
		{
			ASTNode* ref = resolution_visitor.candidates[0];

			tree::visitor::NodeInfoVisitor node_info_visitor;
			node_info_visitor.visit(*ref);
			LOG4CXX_DEBUG(Logger::Resolver, L"package \"" << node.toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
			node_info_visitor.reset();

			bool valid = true;
			if(isa<Package>(ref))
			{
				if(!no_action) ResolvedPackage::set(&node, ref);
			}
			else
			{
				LOG4CXX_FATAL(Logger::Resolver, L"resolve package \"" << node.toString() << L"\" to unknown type");
				valid = false;
			}

			resolution_visitor.reset();
			return valid;
		}
		else
		{
			if(resolution_visitor.candidates.size() > 1)
			{
				// mode than one candidate
				LOG4CXX_ERROR(Logger::Resolver, L"ambiguous package \"" << node.toString() << L"\"");

				tree::visitor::NodeInfoVisitor node_info_visitor;
				foreach(i, resolution_visitor.candidates)
				{
					node_info_visitor.visit(**i);
					LOG4CXX_DEBUG(Logger::Resolver, L"package can be resolved to: \"" << node_info_visitor.stream.str() << L"\"");
					node_info_visitor.reset();
				}
			}
			else
			{
				// no candidate
				LOG4CXX_ERROR(Logger::Resolver, L"unresolved package \"" << node.toString() << L"\"");
			}

			resolution_visitor.reset();
			return false;

		}
	}

	__gnu_cxx::hash_set<tree::ASTNode*> current_scopes;
	tree::visitor::ResolutionVisitor resolution_visitor;
};

} }

#endif /* ZILLIANS_LANGUAGE_RESOLVER_H_ */
