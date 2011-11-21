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
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/tree/visitor/ResolutionVisitor.h"
#include "language/logging/LoggerWrapper.h"
#include "language/logging/StringTable.h"
#include "language/resolver/Resolver.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::ResolutionVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * ResolutionStageVisitor is the visitation helper for ResolutionStage
 *
 * ResolutionStageVisitor will visit every node in the AST that needs to be resolved as type or symbol by using Resolver
 *
 * @see ResolutionStage, Resolver
 * @todo implement resolution cache
 */
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

	ResolutionStageVisitor(Target::type type, Resolver& type_resolver) : type(type), resolver(type_resolver), resolved_count(0), unresolved_count(0)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(resolveInvoker)
	}

	void resolve(ASTNode& node)
	{
		revisit(node);
	}

	void resolve(NumericLiteral& node)
	{
		revisit(node);

		if(!ResolvedType::get(&node))
		{
			ResolvedType::set(&node, getInternalPrimitiveType(node.type));
			++resolved_count;
		}
	}

	void resolve(ObjectLiteral& node)
	{
		revisit(node);

		if(!ResolvedType::get(&node))
		{
			ResolvedType::set(&node, getInternalPrimitiveType(PrimitiveType::OBJECT));
			++resolved_count;
		}
	}

	void resolve(StringLiteral& node)
	{
		revisit(node);

		if(!ResolvedType::get(&node))
		{
			ResolvedType::set(&node, getInternalPrimitiveType(PrimitiveType::STRING));
			++resolved_count;
		}
	}

	void resolve(TemplatedIdentifier& node)
	{
		if(node.type == TemplatedIdentifier::Usage::ACTUAL_ARGUMENT)
		{
			revisit(node);
		}
	}

	void resolve(Internal& node)
	{
		UNUSED_ARGUMENT(node);
		// since there won't be any unresolved type or symbol in the internal node, just skip it
	}

	void resolve(Package& node)
	{
		// tell resolver that we're entering this package scope
		resolver.enterScope(node);

		// and then visit the sub-elements of this package
		// (note that this visitor is actually a DFS visitor)
		revisit(node);

		// tell resolver that we're leaving this package scope
		resolver.leaveScope(node);
	}

	void resolve(Block& node)
	{
		// tell resolver that we're entering this block scope
		resolver.enterScope(node);

		// and then visit the sub-elements of this block, which could be a bunch of statements
		// note that there can be unresolved type specifier in those statements,
		// for example, for type resolution, DecarlativeStmt pointing to a VariableDecl, so the type of that VariableDecl should be properly resolved
		// another example, for symbol resolution, PrimaryExpr must be resolved to a symbol, which can be pointing to a VariableDecl
		revisit(node);

		// tell resolver that we're leaving this block scope
		resolver.leaveScope(node);
	}

	void resolve(Source& node)
	{
		foreach(i, node.imports)
			resolver.enterScope(**i);

		visit(*node.root);

		foreach(i, node.imports)
			resolver.leaveScope(**i);
	}

	void resolve(TypeSpecifier& node)
	{
		// this can be reached from visiting type specifier of a variable declaration
		// or visiting type specifier of the return type of function declaration
		// or visiting
		tryResolveType(&node, &node);
	}

	void resolve(ClassDecl& node)
	{
		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(InterfaceDecl& node)
	{
		resolver.enterScope(node);
		revisit(node);
		resolver.leaveScope(node);
	}

	void resolve(FunctionDecl& node)
	{
		// when entering FunctionDecl scope
		// all parameters can be seen by the resolver
		// see ResolutionVisitor.h
		resolver.enterScope(node);

		LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, L"trying to resolve function: " << node.name->toString());

		if(type == Target::TYPE_RESOLUTION)
		{
			// try to resolve return type of the function
			if(node.type)
				visit(*node.type);

			// try to resolve parameters, which contains type specifier needed to be resolved
			foreach(i, node.parameters)
				visit(**i);

		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// there's no direct symbol needed to be resolved of a function declaration
			// do nothing
		}

		// visit all statements in the function block, which might contain resolvable type or symbol
		if(node.block)
			visit(*node.block);

		// leaving FunctionDecl scope
		resolver.leaveScope(node);
	}

	void resolve(EnumDecl& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			// there couldn't be any type specifier needs to be resolved inside enum,
			// do nothing
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// there can be some symbol (which should be constant expressions) defined in the enumeration value
			foreach(i, node.values)
			{
				visit(**i);
			}
		}
	}

	void resolve(TypedefDecl& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			if(node.type)
				visit(*node.type);
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// we should never reach here because there can't be any symbol inside a typedef declaration
			UNREACHABLE_CODE();
		}
	}

	void resolve(VariableDecl& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			if(node.type)
				visit(*node.type);
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			if(node.initializer)
				visit(*node.initializer);
		}
	}

	void resolve(BranchStmt& node)
	{
		revisit(node);

		// for all branch statement of return type, we should resolve the type of it
		if(node.opcode == BranchStmt::OpCode::RETURN)
		{
			// get owner function's return type
			TypeSpecifier* function_return_type = ASTNodeHelper::getOwner<FunctionDecl>(&node)->type;

			if(!node.result)
			{
				// if the result node is invalid, we should check if the function's return type is also void, otherwise it's invalid
				if(function_return_type->isPrimitiveType() && function_return_type->referred.primitive == PrimitiveType::VOID)
				{
					propogateType(node, *getInternalPrimitiveType(PrimitiveType::VOID));
				}
				else
				{
					LOG_MESSAGE(MISSING_RETURN_VALUE, &node, _type = ASTNodeHelper::getNodeName(function_return_type));
				}
			}
			else
			{
				// otherwise we should set the type to function's return type
				propogateType(node, *function_return_type);
				convertDispatch(&node, function_return_type, node.result, true /*is_assignment*/, false /*is_arithmetic*/, false /*is_logical*/);
			}
		}
	}

	void resolve(DeclarativeStmt& node)
	{
		revisit(node);
	}

	void resolve(ExpressionStmt& node)
	{
		revisit(node);
	}

	void resolve(ForStmt& node)
	{
		revisit(node);
		convertLogical(node.cond);
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
		convertLogical(node.cond);
	}

	void resolve(IfElseStmt& node)
	{
		revisit(node);

		convertLogical(node.if_branch.cond);
		foreach(i, node.elseif_branches)
			convertLogical(i->cond);
	}

	void resolve(SwitchStmt& node)
	{
		revisit(node);
	}

	void resolve(UnaryExpr& node)
	{
		revisit(node);

		if(node.opcode == UnaryExpr::OpCode::NEW)
		{
			if(!ResolvedType::get(&node))
			{

			}
		}
		else if(node.opcode == UnaryExpr::OpCode::LOGICAL_NOT)
		{
			propogateType(node, *getInternalPrimitiveType(PrimitiveType::BOOL));
		}
		else
		{
			// propagate the type from contained node
			propogateType(node, *node.node);
		}
	}

	void resolve(BinaryExpr& node)
	{
		revisit(node);
		// TODO insert necessary cast expression here? or we left it for other phase but only check for compatibility
		if(node.isArithmetic() || node.isBinary())
		{
			if(!ResolvedType::get(&node))
			{
				// get synthesized type from LHS and RHS
				ASTNode* left_type = ResolvedType::get(node.left);
				ASTNode* right_type = ResolvedType::get(node.right);

				if(left_type && right_type)
				{
					if(!isArithmeticCapableType(left_type) || !isArithmeticCapableType(right_type))
					{
						LOG_MESSAGE(INVALID_ARITHMETIC, &node);
						if(!isArithmeticCapableType(left_type))
						{
							LOG_MESSAGE(INVALID_ARITHMETIC_INFO, &node, _id = ASTNodeHelper::getNodeName(node.left));
							unresolved_nodes.insert(node.left);
							++unresolved_count;
						}
						if(!isArithmeticCapableType(right_type))
						{
							LOG_MESSAGE(INVALID_ARITHMETIC_INFO, &node, _id = ASTNodeHelper::getNodeName(node.right));
							unresolved_nodes.insert(node.right);
							++unresolved_count;
						}
					}
					else
					{
						bool precision_loss = false;
						PrimitiveType::type promoted_type = PrimitiveType::promote(cast<TypeSpecifier>(left_type)->referred.primitive, cast<TypeSpecifier>(right_type)->referred.primitive, precision_loss);
						ResolvedType::set(&node, getInternalPrimitiveType(promoted_type));
						++resolved_count;
					}
				}
			}
		}
		else if(node.isAssignment())
		{
			// always use LHS type
			// TODO perform type inference here
			propogateSymbol(node, *node.left);
			propogateType(node, *node.left);
		}
		else if(node.isComparison() || node.isLogical())
		{
			// comparison should always yield boolean type
			propogateType(node, *getInternalPrimitiveType(PrimitiveType::BOOL));
		}
		else
		{
			BOOST_ASSERT(false && "unimplemented");
		}

		convertDispatch(&node, node.left, node.right, node.isAssignment(), node.isArithmetic() || node.isBinary() || node.isComparison(), node.isLogical());
	}

	void resolve(TernaryExpr& node)
	{
		revisit(node);
		// TODO get synthesized type from true node and false node (which should be compatible and casted to the same type)
	}

	void resolve(CallExpr& node)
	{
		revisit(node);

		if(!ResolvedType::get(&node))
		{
			ASTNode* type = ResolvedType::get(node.node);
			if(type)
			{
				if(isa<FunctionDecl>(type))
				{
					// if the callee type is function declaration, we should use its return type as call expression's type
					ResolvedType::set(&node, cast<FunctionDecl>(type)->type);
					++resolved_count;
				}
				else if(isa<TypeSpecifier>(type))
				{
					// if the callee is a variable and its type is a lambda type, we should use its lambda return type as call expression's type
					TypeSpecifier* specifier = cast<TypeSpecifier>(type);
					if(specifier->isFunctionType())
					{
						ResolvedType::set(&node, specifier->referred.function_type->return_type);
						++resolved_count;
					}
					else
					{
						// Error: Calling Non-Function
						LOG_MESSAGE(CALL_NONFUNC, &node, _id = ASTNodeHelper::getNodeName(&node));
						unresolved_nodes.insert(&node);
						++unresolved_count;
					}
				}
				else
				{
					// Error: Calling Non-Function
					LOG_MESSAGE(CALL_NONFUNC, &node, _id = ASTNodeHelper::getNodeName(&node));
					unresolved_nodes.insert(&node);
					++unresolved_count;
				}
			}
		}
	}

	void resolve(MemberExpr& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			// there's no type needed to be resolved inside a member expression
			// do nothing
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
			// there's no type needed to be resolved inside a primary expression
			// do nothing
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			if(node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
			{
				// for general primary expression which is specified by an identifier
				// we should resolve its type to a symbol or package
				tryResolveSymbolOrPackage(&node, node.value.identifier);
			}
			else if(node.catagory == PrimaryExpr::Catagory::LAMBDA)
			{
				// no symbol is defined for lambda expression
				// just visit its children and propagate type
				// TODO this should be unreachable because we will restruct lambda expression beforehand
				visit(*node.value.lambda);

				propogateType(node, *node.value.lambda);
			}
			else if(node.catagory == PrimaryExpr::Catagory::LITERAL)
			{
				// no symbol is defined for literal
				// just visit its children and propagate type
				visit(*node.value.literal);

				propogateType(node, *node.value.literal);
			}
		}
	}

	void resolve(CastExpr& node)
	{
		if(type == Target::TYPE_RESOLUTION)
		{
			// the type we are casting to is a type specifier, so we have to resolve its type
			if(node.type)
				visit(*node.type);
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			// there can be symbol declared inside the expression to cast
			// just visit the expression node
			visit(*node.node);

			// the cast expression should always use the specified type
			propogateType(node, *node.type);
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

	void reset()
	{
		unresolved_nodes.clear();
		resolved_count = 0;
		unresolved_count = 0;
	}

	bool hasTransforms()
	{
		return (transforms.size() > 0);
	}

	void applyTransforms()
	{
		foreach(i, transforms)
			(*i)();

		transforms.clear();
	}

private:
	bool tryResolveType(ASTNode* attach, TypeSpecifier* node, bool no_action = false)
	{
		if(!node)
			return false;

		if(ResolvedType::get(attach))
			return true;

		if(node->isUnspecified())
		{
			if(resolver.resolveType(*attach, *node, no_action))
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
		else
		{
			return true;
		}
	}

	bool tryResolveSymbol(ASTNode* attach, ASTNode* scope, Identifier* node, bool no_action = false)
	{
		if(!scope || !node)
			return false;

		if(ResolvedSymbol::get(attach))
			return true;

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

		if(ResolvedSymbol::get(attach))
			return true;

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

		if(ResolvedPackage::get(attach))
			return true;

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

		if(ResolvedPackage::get(attach))
			return true;

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

		if(ResolvedSymbol::get(attach) || ResolvedPackage::get(attach))
			return true;

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

		if(ResolvedSymbol::get(attach) || ResolvedPackage::get(attach))
			return true;

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
	void propogateType(ASTNode& to, ASTNode& from)
	{
		ASTNode* resolved_type_from = ResolvedType::get(&from);
		ASTNode* resolved_type_to   = ResolvedType::get(&to);

		if(!resolved_type_to)
		{
			if(!resolved_type_from)
			{
				unresolved_nodes.insert(&to);
				++unresolved_count;
			}
			else
			{
				++resolved_count;
				ResolvedType::set(&to, resolved_type_from);
			}
		}
	}

	void propogateType(ASTNode& to, TypeSpecifier& from)
	{
		ASTNode* resolved_type_to = ResolvedType::get(&to);

		if(!resolved_type_to)
		{
			++resolved_count;
			ResolvedType::set(&to, &from);
		}
	}

	void propogateSymbol(ASTNode& to, ASTNode& from)
	{
		ASTNode* resolved_symbol_from = ResolvedSymbol::get(&from);
		ASTNode* resolved_symbol_to   = ResolvedSymbol::get(&to);

		if(!resolved_symbol_to)
		{
			if(!resolved_symbol_from)
			{
				unresolved_nodes.insert(&to);
				++unresolved_count;
			}
			else
			{
				++resolved_count;
				ResolvedSymbol::set(&to, resolved_symbol_from);
			}
		}
	}

private:
	bool isArithmeticCapableType(ASTNode* node)
	{
		if(!isa<TypeSpecifier>(node))
			return false;

		TypeSpecifier* specifier = cast<TypeSpecifier>(node);
		if(!specifier->isPrimitiveType())
			return false;

		if(PrimitiveType::isArithmeticCapable(specifier->referred.primitive))
			return true;
		else
			return false;
	}

	TypeSpecifier* getInternalPrimitiveType(PrimitiveType::type t)
	{
		return getParserContext().tangle->internal->getPrimitiveTy(t);
	}

	void convertDispatch(ASTNode* node_to_debug, ASTNode* lhs, ASTNode* rhs, bool is_assignment, bool is_arithmetic, bool is_logical)
	{
		ASTNode* resolved_type_left;
		ASTNode* resolved_type_right;

		if(!isa<TypeSpecifier>(lhs) || (isa<TypeSpecifier>(lhs) && cast<TypeSpecifier>(lhs)->isUnspecified()))
			resolved_type_left = ResolvedType::get(lhs);
		else
			resolved_type_left = lhs;

		if(!isa<TypeSpecifier>(rhs) || (isa<TypeSpecifier>(rhs) && cast<TypeSpecifier>(rhs)->isUnspecified()))
			resolved_type_right = ResolvedType::get(rhs);
		else
			resolved_type_right = rhs;

		if(!resolved_type_left || !resolved_type_right)
			return;

		if(is_assignment)
		{
			if(isa<TypeSpecifier>(resolved_type_left))
			{
				TypeSpecifier* specifier_left = cast<TypeSpecifier>(resolved_type_left);
				if(specifier_left->isPrimitiveType())
				{
					// if LHS type is primitive type, the RHS can only be primitive type
					TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);
					if(specifier_right && specifier_right->isPrimitiveType())
					{
						convertPrimitive(rhs, specifier_left, specifier_right);
					}
					else
					{
						// LHS it primitive, RHS is NOT primitive, error here
						LOG_MESSAGE(INVALID_CONV, node_to_debug, _rhs_type = ASTNodeHelper::getNodeName(specifier_right), _lhs_type = ASTNodeHelper::getNodeName(specifier_left));
					}
				}
				else if(specifier_left->isFunctionType())
				{
					// if LHS is lambda function type, RHS must be another lambda function type
					TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);
					if(specifier_right)
					{
						// check if the LHS function type is compatible with RHS function type
						if(!ASTNodeHelper::compareTypeSpecifier(specifier_left, specifier_right))
						{
							// LHS it primitive, RHS is NOT primitive, error here
							LOG_MESSAGE(INVALID_CONV, node_to_debug, _rhs_type = ASTNodeHelper::getNodeName(specifier_right), _lhs_type = ASTNodeHelper::getNodeName(specifier_left));
						}
					}
					else
					{
						FunctionDecl* function_decl_right = cast<FunctionDecl>(resolved_type_right);
						if(function_decl_right)
						{
							// check if the LHS function type is compatible with RHS function declaration
							if(isa<TemplatedIdentifier>(function_decl_right->name))
							{
								BOOST_ASSERT(false && "not yet implemented");
							}
							else
							{
								FunctionType* function_type_right = ASTNodeHelper::createFunctionTypeFromFunctionDecl(function_decl_right);
								if(!ASTNodeHelper::compareFunctionType(specifier_left->referred.function_type, function_type_right))
								{
									LOG_MESSAGE(INVALID_CONV, node_to_debug, _rhs_type = ASTNodeHelper::getNodeName(specifier_right), _lhs_type = ASTNodeHelper::getNodeName(specifier_left));
								}
							}
						}
						else
						{
							// LHS it primitive, RHS is NOT primitive, error here
							LOG_MESSAGE(INVALID_CONV, node_to_debug, _rhs_type = ASTNodeHelper::getNodeName(specifier_right), _lhs_type = ASTNodeHelper::getNodeName(specifier_left));
						}
					}
				}
				else if(specifier_left->isUnspecified())
				{
					UNIMPLEMENTED_CODE();
				}
				else
				{
					UNREACHABLE_CODE();
				}
			}
			else if(isa<ClassDecl>(resolved_type_left))
			{
				// TODO check if the RHS class can be casted into LHS class by look at the class extension structure
				UNIMPLEMENTED_CODE();
			}
			else if(isa<InterfaceDecl>(resolved_type_left))
			{
				UNIMPLEMENTED_CODE();
			}
			else
			{
				LOG_MESSAGE(INVALID_CONV, node_to_debug, _rhs_type = ASTNodeHelper::getNodeName(resolved_type_right), _lhs_type = ASTNodeHelper::getNodeName(resolved_type_left));
			}
		}

		if(is_arithmetic)
		{
			// LHS and RHS must be arithmetic compatible
			TypeSpecifier* specifier_left = cast<TypeSpecifier>(resolved_type_left);
			TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);

			if( (!specifier_left || !specifier_right) ||
				(!specifier_left->isPrimitiveType() || !specifier_right->isPrimitiveType()) ||
				(!PrimitiveType::isArithmeticCapable(specifier_left->referred.primitive) || !PrimitiveType::isArithmeticCapable(specifier_right->referred.primitive)))
			{
				LOG_MESSAGE(INVALID_ARITHMETIC, node_to_debug);
			}
			else
			{
				convertPrimitive(rhs, specifier_left, specifier_right);
			}
		}

		if(is_logical)
		{
			convertLogical(lhs);
			convertLogical(rhs);
		}
	}

	void convertPrimitive(ASTNode* node, TypeSpecifier* specifier_left, TypeSpecifier* specifier_right)
	{
		if(!specifier_left || !specifier_right)
			return;

		// if the LHS primitive type is different from RHS primitive type, insert a type cast expression if appropriate
		if(specifier_left->referred.primitive != specifier_right->referred.primitive)
		{
			// check if the LHS type can be casted into RHS type
			// TODO ERROR! should use promote for arithmetic operation such as add/sub/div/mul
			// TODO But for assignment, we should use LHS type
			// TODO For += or -= or ... (assignment with arithmetic), we should convert the structure into a simpler form for processing beforehand (a += b := a = a + b)
			bool precision_loss = false;
			if(PrimitiveType::isImplicitConvertible(specifier_right->referred.primitive, specifier_left->referred.primitive, precision_loss))
			{
				if(precision_loss)
				{
					LOG_MESSAGE(IMPLICIT_CAST_PRECISION_LOSS, node);
				}

				// insert cast expression to cast RHS to LHS
				transforms.push_back([=]{
					ASTNode* parent = node->parent; // save the parent pointer for later use

					CastExpr* cast_expr = new CastExpr(cast<Expression>(node), specifier_left);
					// replace the node with update_parent = false because we will update it manually
					parent->replaceUseWith(*node, *cast_expr, false);
					cast_expr->parent = parent;

					propogateType(*cast_expr, *getInternalPrimitiveType(cast_expr->type->referred.primitive));

					ASTNodeHelper::propogateSourceInfo(*cast_expr, *node); // propagate the source info
				});
			}
			else
			{
				LOG_MESSAGE(INVALID_CONV, node, _rhs_type = ASTNodeHelper::getNodeName(node), _lhs_type = ASTNodeHelper::getNodeName(specifier_left));
			}
		}
	}

	void convertLogical(ASTNode* node)
	{
		ASTNode* resolved_type = ResolvedType::get(node);

		if(!resolved_type)
			return;

		if(!isa<TypeSpecifier>(resolved_type))
		{
			LOG_MESSAGE(INVALID_CONV, node, _rhs_type = ASTNodeHelper::getNodeName(resolved_type), _lhs_type = ASTNodeHelper::getNodeName(getParserContext().tangle->internal->BooleanTy));
			return;
		}

		TypeSpecifier* specifier = cast<TypeSpecifier>(resolved_type);
		if(!specifier->isPrimitiveType()/* || !PrimitiveType::isIntegerType(specifier->referred.primitive)*/)
		{
			LOG_MESSAGE(INVALID_CONV, node, _rhs_type = ASTNodeHelper::getNodeName(specifier), _lhs_type = ASTNodeHelper::getNodeName(getParserContext().tangle->internal->BooleanTy));
			return;
		}

		if(specifier->referred.primitive != PrimitiveType::BOOL)
		{
			transforms.push_back([=]{
				ASTNode* parent = node->parent; // save the parent pointer for later use

				BinaryExpr* compare_expr = new BinaryExpr(BinaryExpr::OpCode::COMPARE_GT, cast<Expression>(node), new PrimaryExpr(new NumericLiteral(specifier->referred.primitive, 0)));
				// replace the node with update_parent = false because we will update it manually
				parent->replaceUseWith(*node, *compare_expr, false);
				compare_expr->parent = parent;

				ASTNodeHelper::propogateSourceInfo(*compare_expr, *node); // propagate the source info
			});
		}
	}

public:
	Target::type type;
	Resolver& resolver;

	__gnu_cxx::hash_set<ASTNode*> unresolved_nodes;
	std::size_t resolved_count;
	std::size_t unresolved_count;

private:
	ResolutionVisitor package_visitor;
	std::vector<std::function<void()>> transforms;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_RESOLUTIONSTAGEVISITOR_H_ */
