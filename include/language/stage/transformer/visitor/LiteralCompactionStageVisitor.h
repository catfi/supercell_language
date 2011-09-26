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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LITERALCOMPACTIONSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LITERALCOMPACTIONSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/tree/visitor/general/NameManglingVisitor.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/logging/StringTable.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NameManglingVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct LiteralCompactionStageVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(compactInvoker, compact)

	LiteralCompactionStageVisitor() : program(NULL)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(compactInvoker)
	}

	void compact(ASTNode& node)
	{
		revisit(node);
	}

	void compact(Program& node)
	{
		program = &node;
		revisit(node);
	}

	void compact(NumericLiteral& node)
	{
		bool negate = false;
		UnaryExpr* negate_expr_for_negation = getOwnerUnaryExprForNegation(node);
		if(negate_expr_for_negation)
		{
			BOOST_ASSERT(negate_expr_for_negation->parent && "dangling ASTNode");

			negate = true;

			// assign the negate expression to a reference so that it will can be captured by reference in the following lambda expression
			ASTNode& pivot = *negate_expr_for_negation;

			// remove the parent nagate operator as we merge the result into the literal value
			transforms.push_back([&]{
				pivot.parent->replaceUseWith(pivot, node);
				LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, "merge unary negate expression into literal");
			});
		}

		if(PrimitiveType::isIntegerType(node.type))
		{
			if(!negate)
			{
				if(node.value.u64 <= std::numeric_limits<uint8>::max())
				{
					node.type = PrimitiveType::UINT8;
				}
				else if(node.value.u64 <= std::numeric_limits<uint16>::max())
				{
					node.type = PrimitiveType::UINT16;
				}
				else if(node.value.u64 <= std::numeric_limits<uint32>::max())
				{
					node.type = PrimitiveType::UINT32;
				}
				else if(node.value.u64 <= std::numeric_limits<uint64>::max())
				{
					// TODO this will always matched
					node.type = PrimitiveType::UINT64;
				}
				else
				{
					BOOST_ASSERT(false && "reaching unreachable code");
				}
			}
			else
			{
				if(node.value.u64 < (uint64)std::numeric_limits<int8>::max())
				{
					node.type = PrimitiveType::INT8;
					node.value.i8 = -((int8)node.value.u64);
				}
				else if(node.value.u64 < (uint64)std::numeric_limits<int16>::max())
				{
					node.type = PrimitiveType::INT16;
					node.value.i16 = -((int16)node.value.u64);
				}
				else if(node.value.u64 < (uint64)std::numeric_limits<int32>::max())
				{
					node.type = PrimitiveType::INT32;
					node.value.i32 = -((int32)node.value.u64);
				}
				else if(node.value.u64 < (uint64)std::numeric_limits<int64>::max())
				{
					node.type = PrimitiveType::INT64;
					node.value.i64 = -((int64)node.value.u64);
				}
				else
				{
					// potential overflowed literal, warning here
					LOG_MESSAGE(NUMERIC_LITERAL_OVERFLOW, &node);
				}
			}
		}
		else if(PrimitiveType::isFloatType(node.type))
		{
			if(!negate)
			{
				if(node.value.f64 <= std::numeric_limits<float>::max())
				{
					node.type = PrimitiveType::FLOAT32;
					node.value.f32 = (float)(node.value.f64);
				}
				else if(node.value.f64 <= std::numeric_limits<double>::max())
				{
					// TODO this will always matched
				}
				else
				{
					BOOST_ASSERT(false && "reaching unreachable code");
				}
			}
			else
			{
				if(std::numeric_limits<float>::min() < (-node.value.f64) && (-node.value.f64) <= std::numeric_limits<float>::max())
				{
					node.type = PrimitiveType::FLOAT32;
					node.value.f32 = (float)(-node.value.f64);
				}
				else if(std::numeric_limits<double>::min() < (-node.value.f64) && (-node.value.f64) <= std::numeric_limits<double>::max())
				{
					// TODO this will always matched
				}
				else
				{
					BOOST_ASSERT(false && "reaching unreachable code");
				}
			}
		}
	}

	UnaryExpr* getOwnerUnaryExprForNegation(NumericLiteral& node)
	{
		if(!node.parent)
			return NULL;

		PrimaryExpr* primary_expr = cast<PrimaryExpr>(node.parent);
		if(!primary_expr)
			return NULL;

		if(!primary_expr->parent)
			return NULL;

		UnaryExpr* unary_expr = cast<UnaryExpr>(primary_expr->parent);
		if(!unary_expr)
			return NULL;

		if(unary_expr->opcode != UnaryExpr::OpCode::ARITHMETIC_NEGATE)
			return NULL;

		return unary_expr;
	}

	void applyTransforms()
	{
		foreach(i, transforms)
		{
			(*i)();
		}
		transforms.clear();
	}

	Program* program;
	std::vector<std::function<void()>> transforms;
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LITERALCOMPACTIONSTAGEVISITOR_H_ */
