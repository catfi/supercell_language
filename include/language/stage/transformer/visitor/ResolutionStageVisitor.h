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

/**
 * ResolutionStageVisitor is the visitation helper for ResolutionStage
 *
 * @see ResolutionStage
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

		// TODO how to handle mutual import?
		package_visitor.visit(*getParserContext().program);

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

			if(node.initializer)
			{
				convertImpl(node, node, *node.initializer, true /*is_assignment*/, false /*is_arithmetic*/, false /*is_logica*/);
			}
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

		if(!node.result)
		{
			if(!ResolvedType::get(&node))
			{
				ResolvedType::set(&node, getInternalPrimitiveType(PrimitiveType::VOID));
			}
		}
		else
		{
			propogateType(node, *node.result);
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
		convertLogical(*node.cond);
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
		convertLogical(*node.cond);
	}

	void resolve(IfElseStmt& node)
	{
		revisit(node);

		convertLogical(*node.if_branch.cond);
		foreach(i, node.elseif_branches)
		{
			convertLogical(*i->cond);
		}
	}

	void resolve(SwitchStmt& node)
	{
		revisit(node);
	}

	void resolve(UnaryExpr& node)
	{
		revisit(node);
		// propagate the type from contained node
		propogateType(node, *node.node);
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
							LOG_MESSAGE(INVALID_ARITHMETIC_INFO, &node, _id = ASTNodeHelper::nodeName(node.left));
							unresolved_nodes.insert(node.left);
							++unresolved_count;
						}
						if(!isArithmeticCapableType(right_type))
						{
							LOG_MESSAGE(INVALID_ARITHMETIC_INFO, &node, _id = ASTNodeHelper::nodeName(node.right));
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
			if(!ResolvedType::get(&node))
			{
				ResolvedType::set(&node, getInternalPrimitiveType(PrimitiveType::BOOL));
				++resolved_count;
			}
		}
		else
		{
			BOOST_ASSERT(false && "unimplemented");
		}

		convertImpl(node, *node.left, *node.right, node.isAssignment(), node.isArithmetic() || node.isBinary() || node.isComparison(), node.isLogical());
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
					if(specifier->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
					{
						ResolvedType::set(&node, specifier->referred.function_type->return_type);
						++resolved_count;
					}
					else
					{
						// Error: Calling Non-Function
						LOG_MESSAGE(CALL_NONFUNC, &node, _id = ASTNodeHelper::nodeName(&node));
						unresolved_nodes.insert(&node);
						++unresolved_count;
					}
				}
				else
				{
					// Error: Calling Non-Function
					LOG_MESSAGE(CALL_NONFUNC, &node, _id = ASTNodeHelper::nodeName(&node));
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
//			switch(node.catagory)
//			{
//			case PrimaryExpr::Catagory::IDENTIFIER: ResolvedType::set(&node, ResolvedType::get(node.value.identifier)); break;
//			case PrimaryExpr::Catagory::LAMBDA: ResolvedType::set(&node, node.value.lambda); break;
//			case PrimaryExpr::Catagory::LITERAL: ResolvedType::set(&node, ResolvedType::get(node.value.literal)); break;
//			default: break;
//			}
		}
		else if(type == Target::SYMBOL_RESOLUTION)
		{
			if(node.catagory == PrimaryExpr::Catagory::IDENTIFIER)
			{
				tryResolveSymbolOrPackage(&node, node.value.identifier);
			}
			else if(node.catagory == PrimaryExpr::Catagory::LAMBDA)
			{
				if(!ResolvedType::get(&node))
				{
					ResolvedType::set(&node, node.value.lambda);
					++resolved_count;
				}
			}
			else if(node.catagory == PrimaryExpr::Catagory::LITERAL)
			{
				visit(*node.value.literal);

				if(!ResolvedType::get(&node))
				{
					ResolvedType::set(&node, ResolvedType::get(node.value.literal));
					++resolved_count;
				}
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
			if(!ResolvedType::get(&node))
			{
				ResolvedType::set(&node, node.type);
				++resolved_count;
			}
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
		{
			(*i)();
		}
		transforms.clear();
	}

private:
	bool tryResolveType(ASTNode* attach, TypeSpecifier* node, bool no_action = false)
	{
		if(!node)
			return false;

		if(ResolvedType::get(attach))
			return true;

		if(node->type == TypeSpecifier::ReferredType::UNSPECIFIED)
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

		if(ResolvedSymbol::get(attach) && ResolvedPackage::get(attach))
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

		if(ResolvedSymbol::get(attach) && ResolvedPackage::get(attach))
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

	void convertImpl(ASTNode& node_to_debug, ASTNode& lhs, ASTNode& rhs, bool is_assignment, bool is_arithmetic, bool is_logical)
	{
		ASTNode* resolved_type_left = ResolvedType::get(&lhs);
		ASTNode* resolved_type_right = ResolvedType::get(&rhs);

		if(!resolved_type_left || !resolved_type_right)
			return;

		if(is_assignment)
		{
			if(isa<TypeSpecifier>(resolved_type_left))
			{
				TypeSpecifier* specifier_left = cast<TypeSpecifier>(resolved_type_left);
				if(specifier_left->type == TypeSpecifier::ReferredType::PRIMITIVE)
				{
					// if LHS type is primitive type, the RHS can only be primitive type
					TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);
					if(specifier_right && specifier_right->type == TypeSpecifier::ReferredType::PRIMITIVE)
					{
						convertPrimitive(rhs, specifier_left, specifier_right);
					}
					else
					{
						// LHS it primitive, RHS is NOT primitive, error here
						LOG_MESSAGE(INVALID_CONV, &node_to_debug, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
					}
				}
				else if(specifier_left->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
				{
					// if LHS is lambda function type, RHS must be another lambda function type
					TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);
					if(specifier_right)
					{
						// check if the LHS function type is compatible with RHS function type
						if(!ASTNodeHelper::compareTypeSpecifier(specifier_left, specifier_right))
						{
							// LHS it primitive, RHS is NOT primitive, error here
							LOG_MESSAGE(INVALID_CONV, &node_to_debug, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
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
									LOG_MESSAGE(INVALID_CONV, &node_to_debug, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
								}
							}
						}
						else
						{
							// LHS it primitive, RHS is NOT primitive, error here
							LOG_MESSAGE(INVALID_CONV, &node_to_debug, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
						}
					}
				}
				else if(specifier_left->type == TypeSpecifier::ReferredType::UNSPECIFIED)
				{
					BOOST_ASSERT(false && "reaching unreachable code");
				}
				else
				{
					BOOST_ASSERT(false && "reaching unreachable code");
				}
			}
			else if(isa<ClassDecl>(resolved_type_left))
			{
				// TODO check if the RHS class can be casted into LHS class by look at the class extension structure
			}
			else if(isa<InterfaceDecl>(resolved_type_left))
			{

			}
			else
			{
				LOG_MESSAGE(INVALID_CONV, &node_to_debug, _rhs_type = ASTNodeHelper::nodeName(resolved_type_right), _lhs_type = ASTNodeHelper::nodeName(resolved_type_left));
			}
		}

		if(is_arithmetic)
		{
			// LHS and RHS must be arithmetic compatible
			TypeSpecifier* specifier_left = cast<TypeSpecifier>(resolved_type_left);
			TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);

			if( (!specifier_left || !specifier_right) ||
				(specifier_left->type != TypeSpecifier::ReferredType::PRIMITIVE || specifier_right->type != TypeSpecifier::ReferredType::PRIMITIVE) ||
				(!PrimitiveType::isArithmeticCapable(specifier_left->referred.primitive) || !PrimitiveType::isArithmeticCapable(specifier_right->referred.primitive)))
			{
				LOG_MESSAGE(INVALID_ARITHMETIC, &node_to_debug);
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

	void convertPrimitive(ASTNode& node, TypeSpecifier* specifier_left, TypeSpecifier* specifier_right)
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
					LOG_MESSAGE(IMPLICIT_CAST_PRECISION_LOSS, &node);
				}

				// insert cast expression to cast RHS to LHS
				transforms.push_back([&, specifier_left]{
					ASTNode* parent = node.parent; // save the parent pointer for later use

					CastExpr* cast_expr = new CastExpr(cast<Expression>(&node), specifier_left);
					// replace the node with update_parent = true because we will update it manually
					parent->replaceUseWith(node, *cast_expr, false);
					cast_expr->parent = parent;

					ASTNodeHelper::propogateSourceInfo(*cast_expr, node); // propagate the source info
				});
			}
		}
	}

	void convertLogical(ASTNode& node)
	{
		ASTNode* resolved_type = ResolvedType::get(&node);

		if(!resolved_type)
			return;

		if(!isa<TypeSpecifier>(resolved_type))
		{
			LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(resolved_type), _lhs_type = ASTNodeHelper::nodeName(getParserContext().program->internal->BooleanTy));
			return;
		}

		TypeSpecifier* specifier = cast<TypeSpecifier>(resolved_type);
		if(specifier->type != TypeSpecifier::ReferredType::PRIMITIVE/* || !PrimitiveType::isIntegerType(specifier->referred.primitive)*/)
		{
			LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier), _lhs_type = ASTNodeHelper::nodeName(getParserContext().program->internal->BooleanTy));
			return;
		}

		if(specifier->referred.primitive != PrimitiveType::BOOL)
		{
			transforms.push_back([&, specifier]{
				ASTNode* parent = node.parent; // save the parent pointer for later use

				BinaryExpr* compare_expr = new BinaryExpr(BinaryExpr::OpCode::COMPARE_GT, cast<Expression>(&node), new PrimaryExpr(new NumericLiteral(specifier->referred.primitive, 0)));
				// replace the node with update_parent = true because we will update it manually
				parent->replaceUseWith(node, *compare_expr, false);
				compare_expr->parent = parent;

				ASTNodeHelper::propogateSourceInfo(*compare_expr, node); // propagate the source info
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
