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
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/tree/visitor/general/ResolutionVisitor.h"
#include "language/logging/LoggerWrapper.h"
#include "language/logging/StringTable.h"
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

	void resolve(Internal& node)
	{
		// since there won't be any unresolved type or symbol in the internal node, just skip it
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
				if((*i)->type)
					tryResolveType((*i)->type, (*i)->type);
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
			tryResolveType(node.type, node.type);
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
		// propagate the type from contained node
		ResolvedType::set(&node, ResolvedType::get(node.node));
	}

	void resolve(BinaryExpr& node)
	{
		revisit(node);
		// TODO insert necessary cast expression here? or we left it for other phase but only check for compatibility
		if(node.isArithmetic() || node.isBinary())
		{
			// get synthesized type from LHS and RHS
			ASTNode* left_type = ResolvedType::get(node.left);
			ASTNode* right_type = ResolvedType::get(node.right);

			if(left_type && right_type)
			{
				if(!isArithmeticCapableType(left_type) || !isArithmeticCapableType(right_type))
				{
					LOG_MESSAGE(INVALID_ARITHMETIC, &node);
					if(!isArithmeticCapableType(left_type)) LOG_MESSAGE(INVALID_ARITHMETIC_INFO, &node, _id = ASTNodeHelper::nodeName(node.left));
					if(!isArithmeticCapableType(right_type)) LOG_MESSAGE(INVALID_ARITHMETIC_INFO, &node, _id = ASTNodeHelper::nodeName(node.left));
				}
				else
				{
					bool precision_loss = false;
					PrimitiveType::type promoted_type = PrimitiveType::promote(cast<TypeSpecifier>(left_type)->referred.primitive, cast<TypeSpecifier>(right_type)->referred.primitive, precision_loss);
					ResolvedType::set(&node, getInternalPrimitiveType(promoted_type));
				}
			}
		}
		else if(node.isAssignment())
		{
			// always use LHS type
			// TODO perform type inference here
			ResolvedSymbol::set(&node, ResolvedSymbol::get(node.left));
			ResolvedType::set(&node, ResolvedType::get(node.left));
		}
		else if(node.isComparison() || node.isLogical())
		{
			// comparison should always yield boolean type
			ResolvedType::set(&node, getInternalPrimitiveType(PrimitiveType::BOOL));
		}
		else
		{

		}
	}

	void resolve(TernaryExpr& node)
	{
		revisit(node);
		// TODO get synthesized type from true node and false node (which should be compatible and casted to the same type)
	}

	void resolve(CallExpr& node)
	{
		revisit(node);

		ASTNode* type = ResolvedType::get(node.node);
		if(type)
		{
			if(isa<FunctionDecl>(type))
			{
				// if the callee type is function declaration, we should use its return type as call expression's type
				ResolvedType::set(&node, cast<FunctionDecl>(type)->type);
			}
			else if(isa<TypeSpecifier>(type))
			{
				// if the callee is a variable and its type is a lambda type, we should use its lambda return type as call expression's type
				TypeSpecifier* specifier = cast<TypeSpecifier>(type);
				if(specifier->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
				{
					ResolvedType::set(&node, specifier->referred.function_type->return_type);
				}
				else
				{
					// Error: Calling Non-Function
					LOG_MESSAGE(CALL_NONFUNC, &node, _id = ASTNodeHelper::nodeName(&node));
				}
			}
			else
			{
				// Error: Calling Non-Function
				LOG_MESSAGE(CALL_NONFUNC, &node, _id = ASTNodeHelper::nodeName(&node));
			}
		}
	}

	void resolve(MemberExpr& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
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
			// the cast expression should always use the specified type
			ResolvedType::set(&node, node.type);
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

private:
	void propogateType(ASTNode* to, ASTNode* from)
	{
		// recursively dig down the tree and find the actual resolved type
		ASTNode* resolved_type_from = NULL;
		while(resolved_type_from == NULL)
		{
			if(!from)
			{
				resolved_type_from = NULL;
				break;
			}

			if(isa<ClassDecl>(from) || isa<InterfaceDecl>(from) || isa<EnumDecl>(from) || isa<FunctionDecl>(from))
			{
				resolved_type_from = from;
			}
			else if(isa<VariableDecl>(from))
			{
				from = cast<VariableDecl>(from)->type;
			}
			else if(isa<TypedefDecl>(from))
			{
				from = cast<TypedefDecl>(from)->type;
			}
			else if(isa<TypeSpecifier>(from))
			{
				TypeSpecifier* specifier = cast<TypeSpecifier>(from);
				switch(specifier->type)
				{
				case TypeSpecifier::ReferredType::FUNCTION_TYPE:
				case TypeSpecifier::ReferredType::PRIMITIVE:
					resolved_type_from = specifier; break;
				case TypeSpecifier::ReferredType::UNSPECIFIED:
					from = ResolvedType::get(from); break;
				default:
					break;
				}
			}
			else
			{
				from = ResolvedType::get(from); break;
			}
		}

		ASTNode* resolved_type_to = ResolvedType::get(to);
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

private:
	bool isArithmeticCapableType(ASTNode* node)
	{
		if(!isa<TypeSpecifier>(node))
			return false;

		TypeSpecifier* specifier = cast<TypeSpecifier>(node);
		if(specifier->type != TypeSpecifier::ReferredType::PRIMITIVE)
			return false;

		if(PrimitiveType::isArithmeticCapable(specifier->referred.primitive))
			return true;
		else
			return false;
	}

	TypeSpecifier* getInternalPrimitiveType(PrimitiveType::type t)
	{
		return getParserContext().program->internal->getPrimitiveTy(t);
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
