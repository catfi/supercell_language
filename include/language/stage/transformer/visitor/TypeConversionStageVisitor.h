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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_TYPECONVERSIONSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_TYPECONVERSIONSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/logging/LoggerWrapper.h"
#include "language/logging/StringTable.h"
#include "language/resolver/Resolver.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

// TODO implement resolve cache

struct TypeConversionStageVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(convertInvoker, convert)

	TypeConversionStageVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(convertInvoker)
	}

	void convert(ASTNode& node)
	{
		revisit(node);
	}

	void convert(IfElseStmt& node)
	{

	}

	void convert(ForStmt& node)
	{

	}

	void convert(ForeachStmt& node)
	{
		// TODO convert into range object
	}

	void convert(WhileStmt& node)
	{

	}

	void convert(BinaryExpr& node)
	{
		revisit(node);

		ASTNode* resolved_type_left = ResolvedType::get(node.left);
		ASTNode* resolved_type_right = ResolvedType::get(node.right);

		if(!resolved_type_left || !resolved_type_right)
			return;

		if(node.isAssignment())
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
						convertPrimitive(node, specifier_left, specifier_right);
					}
					else
					{
						// LHS it primitive, RHS is NOT primitive, error here
						LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
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
							LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
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
									LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
								}
							}
						}
						else
						{
							// LHS it primitive, RHS is NOT primitive, error here
							LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(specifier_left));
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
				LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(resolved_type_right), _lhs_type = ASTNodeHelper::nodeName(resolved_type_left));
			}
		}

		if(node.isArithmetic() || node.isBinary() || node.isComparison())
		{
			// LHS and RHS must be arithmetic compatible
			TypeSpecifier* specifier_left = cast<TypeSpecifier>(resolved_type_left);
			TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);

			if( (!specifier_left || !specifier_right) ||
				(specifier_left->type != TypeSpecifier::ReferredType::PRIMITIVE || specifier_right->type != TypeSpecifier::ReferredType::PRIMITIVE) ||
				(!PrimitiveType::isArithmeticCapable(specifier_left->referred.primitive) || !PrimitiveType::isArithmeticCapable(specifier_right->referred.primitive)))
			{
				LOG_MESSAGE(INVALID_ARITHMETIC, &node);
			}
			else
			{
				convertPrimitive(node, specifier_left, specifier_right);
			}
		}

		if(node.isLogical())
		{
			// we need to convert LHS and RHS into boolean type
			TypeSpecifier* specifier_left = cast<TypeSpecifier>(resolved_type_left);
			TypeSpecifier* specifier_right = cast<TypeSpecifier>(resolved_type_right);

			if( (!specifier_left || !specifier_right) ||
				(specifier_left->type != TypeSpecifier::ReferredType::PRIMITIVE || specifier_right->type != TypeSpecifier::ReferredType::PRIMITIVE) ||
				(!PrimitiveType::isIntegerType(specifier_left->referred.primitive) || !PrimitiveType::isIntegerType(specifier_right->referred.primitive)))
			{
				if(!PrimitiveType::isIntegerType(specifier_left->referred.primitive))
					LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier_left), _lhs_type = ASTNodeHelper::nodeName(getParserContext().program->internal->BooleanTy));
				if(!PrimitiveType::isIntegerType(specifier_right->referred.primitive))
					LOG_MESSAGE(INVALID_CONV, &node, _rhs_type = ASTNodeHelper::nodeName(specifier_right), _lhs_type = ASTNodeHelper::nodeName(getParserContext().program->internal->BooleanTy));
			}
			else
			{
				// insert cast expression to cast LHS into boolean type
				transforms.push_back([&]{
					CastExpr* cast_expr = new CastExpr(node.left, getParserContext().program->internal->BooleanTy);
					// replace the node with update_parent = true because we will update it manually
					node.replaceUseWith(*node.left, *cast_expr, false);
					cast_expr->parent = &node;
				});
				// insert cast expression to cast RHS into boolean type
				transforms.push_back([&]{
					CastExpr* cast_expr = new CastExpr(node.right, getParserContext().program->internal->BooleanTy);
					// replace the node with update_parent = true because we will update it manually
					node.replaceUseWith(*node.right, *cast_expr, false);
					cast_expr->parent = &node;
				});
			}
		}
	}

	void convert(CallExpr& node)
	{
		revisit(node);
	}

	void convertPrimitive(BinaryExpr& node, TypeSpecifier* specifier_left, TypeSpecifier* specifier_right)
	{
		// if the LHS primitive type is different from RHS primitive type, insert a type cast expression if appropriate
		if(specifier_left->referred.primitive != specifier_right->referred.primitive)
		{
			// check if the LHS type can be casted into RHS type
			bool precision_loss = false;
			if(PrimitiveType::isImplicitConvertible(specifier_right->referred.primitive, specifier_left->referred.primitive, precision_loss))
			{
				if(precision_loss)
				{
					LOG_MESSAGE(IMPLICIT_CAST_PRECISION_LOSS, &node);
				}

				// insert cast expression to cast RHS to LHS
				transforms.push_back([&, specifier_left]{
					CastExpr* cast_expr = new CastExpr(node.right, specifier_left);
					// replace the node with update_parent = true because we will update it manually
					node.replaceUseWith(*node.right, *cast_expr, false);
					cast_expr->parent = &node;
				});
			}
		}
	}

	void applyTransforms()
	{
		foreach(i, transforms)
		{
			(*i)();
		}
		transforms.clear();
	}

	std::vector<std::function<void()>> transforms;
};

} } } }
#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_TYPECONVERSIONSTAGEVISITOR_H_ */
