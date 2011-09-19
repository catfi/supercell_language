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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_RESOLUTIONSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_RESOLUTIONSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/tree/visitor/general/ResolutionVisitor.h"
#include "language/resolver/Resolver.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::ResolutionVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

// TODO implement resolve cache

struct ResolutionStageVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(resolveInvoker, resolve)

	struct Target
	{
		enum type {
			TYPE_RESOLUTION,
			SYMBOL_RESOLUTION
		};
	};

	ResolutionStageVisitor(Target::type type, Program& program, Resolver& type_resolver) : type(type), program(program), resolver(type_resolver), resolved_count(0), unresolved_count(0), unspecified_type_count(0)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(resolveInvoker)
	}

	void resolve(ASTNode& node)
	{
		revisit(node);
	}

	void resolve(Package& node)
	{
		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(Block& node)
	{
		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(Import& node)
	{
		package_visitor.reset();
		package_visitor.search(node.ns);
		package_visitor.filter(ResolutionVisitor::Filter::PACKAGE);

		package_visitor.visit(program);

		if(package_visitor.candidates.size() == 1)
		{
			resolver.enterScope(*package_visitor.candidates[0]);
		}

		package_visitor.reset();
	}

	void resolve(TypeSpecifier& node)
	{
		// TODO we should never reach here
		BOOST_ASSERT(false && "reaching code that shouldn't be reached");
	}

	void resolve(ClassDecl& node)
	{
		BOOST_ASSERT(type == Target::TYPE_RESOLUTION || type == Target::SYMBOL_RESOLUTION);

		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(InterfaceDecl& node)
	{
		BOOST_ASSERT(type == Target::TYPE_RESOLUTION || type == Target::SYMBOL_RESOLUTION);

		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(FunctionDecl& node)
	{
		BOOST_ASSERT(type == Target::TYPE_RESOLUTION || type == Target::SYMBOL_RESOLUTION);

		resolver.enterScope(node);

		LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, L"trying to resolve function: " << node.name->toString());

		if(type == Target::TYPE_RESOLUTION)
		{
			// try to resolve return type
			if(node.type)
				tryResolveType(node.type, node.type);

			// try to resolve parameter type
			foreach(i, node.parameters)
			{
				if(i->get<1>())
					tryResolveType(i->get<1>(), i->get<1>());
			}

		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// do nothing
		}

		// visit all statements in the function block
		if(node.block)
		{
			visit(*node.block);
		}

		resolver.leaveScope(node);
	}

	void resolve(EnumDecl& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			// for type resolution, there couldn't be any type specifier needs to be resolved inside enum, do nothing
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// for symbol resolution, there can be some symbol (which should be constant expression) defined in the enumeration value
			foreach(i, node.enumeration_list)
			{
				if(i->second)
					visit(*i->second);
			}
		}
	}

	void resolve(TypedefDecl& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			tryResolveType(node.from, node.from);
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// we should never reach here
			BOOST_ASSERT(false && "reaching code that shouldn't be reached");
		}
	}

	void resolve(VariableDecl& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			tryResolveType(node.type, node.type);
			propogateType(&node, node.type);
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
//			if(tryResolveSymbol(node.name, true))
//			{
//				// error, the variable declared name should be resolvable because that would lead to ambiguous name conflict
//				// TODO make the error message cleaner
//				LOG4CXX_ERROR(LoggerWrapper::Resolver, L"ambiguous variable declared: " << node.name->toString());
//			}
			if(node.initializer)
				visit(*node.initializer);
		}
	}

	void resolve(BranchStmt& node)
	{
		revisit(node);
	}

	void resolve(DeclarativeStmt& node)
	{
		revisit(node);
	}

	void resolve(ExpressionStmt& node)
	{
		revisit(node);
	}

	void resolve(ForeachStmt& node)
	{
		// we have to enter scope for ForeachStmt because variable can be declared in the iterator
		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(WhileStmt& node)
	{
		revisit(node);
	}

	void resolve(IfElseStmt& node)
	{
		revisit(node);
	}

	void resolve(SwitchStmt& node)
	{
		revisit(node);
	}

	void resolve(UnaryExpr& node)
	{
		revisit(node);
	}

	void resolve(BinaryExpr& node)
	{
		revisit(node);
	}

	void resolve(TernaryExpr& node)
	{
		revisit(node);
	}

	void resolve(CallExpr& node)
	{
		revisit(node);
	}

	void resolve(MemberExpr& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			visit(*node.node);

			propogateType(&node, node.node);
			// we should never reach here
			//BOOST_ASSERT(false && "reaching code that shouldn't be reached");
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			visit(*node.node);
			tryResolveSymbolOrPackage(&node, node.node, node.member);
		}
	}

	void resolve(PrimaryExpr& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			// we should never reach here
			//BOOST_ASSERT(false && "reaching code that shouldn't be reached");
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			if(node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
			{
				tryResolveSymbolOrPackage(&node, node.value.identifier);
			}
		}
	}

	void resolve(CastExpr& node)
	{
		visit(*node.node);

		if(type == Target::TYPE_RESOLUTION)
		{
			tryResolveType(node.type, node.type);
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
		}
	}

	std::size_t getUnresolvedCount()
	{
		return unresolved_count;
	}

	std::size_t getResolvedCount()
	{
		return resolved_count;
	}

	std::size_t getUnspecifiedCount()
	{
		return unspecified_type_count;
	}

	void reset()
	{
		unresolved_nodes.clear();
		resolved_count = 0;
		unresolved_count = 0;
		unspecified_type_count = 0;
	}

private:
	void propogateType(ASTNode* to, ASTNode* from)
	{
		ASTNode* resolved_type_to = ResolvedType::get(to);
		ASTNode* resolved_type_from = ResolvedType::get(from);
		if(resolved_type_to)
		{
			// the resolved type should be changed during resolution stage
			BOOST_ASSERT(resolved_type_to == resolved_type_from && "type resolution changed during resolution stage");
		}
		else if(resolved_type_from)
		{
			ResolvedType::set(to, resolved_type_from);
		}
		else
		{
			// both resolved_type_to and resolved_type_from are NULL, so there's no need to update
		}
	}

	void tryResolveType(ASTNode* attach, TypeSpecifier* node, bool no_action = false)
	{
		if(!node)
			return;

		if(node->type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			if(resolver.resolveType(*attach, *node, no_action))
			{
				++resolved_count;
			}
			else
			{
				unresolved_nodes.insert(node);
				++unresolved_count;
			}
		}
		else
		{
			++unspecified_type_count;
		}
	}

	bool tryResolveSymbol(ASTNode* attach, ASTNode* scope, Identifier* node, bool no_action = false)
	{
		if(!scope || !node)
			return false;

		if(resolver.resolveSymbol(*attach, *scope, *node, no_action))
		{
			++resolved_count;
			return true;
		}
		else
		{
			unresolved_nodes.insert(node);
			++unresolved_count;
			return false;
		}
	}

	bool tryResolveSymbol(ASTNode* attach, Identifier* node, bool no_action = false)
	{
		if(!node)
			return false;

		if(resolver.resolveSymbol(*attach, *node, no_action))
		{
			++resolved_count;
			return true;
		}
		else
		{
			unresolved_nodes.insert(node);
			++unresolved_count;
			return false;
		}
	}

	bool tryResolvePackage(ASTNode* attach, ASTNode* scope, Identifier* node, bool no_action = false)
	{
		if(!scope || !node)
			return false;

		if(resolver.resolvePackage(*attach, *scope, *node, no_action))
		{
			++resolved_count;
			return true;
		}
		else
		{
			unresolved_nodes.insert(node);
			++unresolved_count;
			return false;
		}
	}

	bool tryResolvePackage(ASTNode* attach, Identifier* node, bool no_action = false)
	{
		if(!node)
			return false;

		if(resolver.resolvePackage(*attach, *node, no_action))
		{
			++resolved_count;
			return true;
		}
		else
		{
			unresolved_nodes.insert(node);
			++unresolved_count;
			return false;
		}
	}

	bool tryResolveSymbolOrPackage(ASTNode* attach, ASTNode* scope, Identifier* node, bool no_action = false)
	{
		if(!scope || !node)
			return false;

		if(resolver.resolveSymbol(*attach, *scope, *node, no_action))
		{
			++resolved_count;
			return true;
		}
		else
		{
			if(resolver.resolvePackage(*attach, *scope, *node, no_action))
			{
				++resolved_count;
				return true;
			}
			else
			{
				unresolved_nodes.insert(node);
				++unresolved_count;
				return false;
			}
		}
	}

	bool tryResolveSymbolOrPackage(ASTNode* attach, Identifier* node, bool no_action = false)
	{
		if(!node)
			return false;

		if(resolver.resolveSymbol(*attach, *node, no_action))
		{
			++resolved_count;
			return true;
		}
		else
		{
			if(resolver.resolvePackage(*attach, *node, no_action))
			{
				++resolved_count;
				return true;
			}
			else
			{
				unresolved_nodes.insert(node);
				++unresolved_count;
				return false;
			}
		}
	}

public:
	Target::type type;
	Program& program;
	Resolver& resolver;

	__gnu_cxx::hash_set<ASTNode*> unresolved_nodes;
	std::size_t resolved_count;
	std::size_t unresolved_count;
	std::size_t unspecified_type_count;

private:
	ResolutionVisitor package_visitor;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_RESOLUTIONSTAGEVISITOR_H_ */
