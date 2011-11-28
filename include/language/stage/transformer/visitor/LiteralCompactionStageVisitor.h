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
#include "language/tree/visitor/GenericVisitor.h"
#include "language/tree/visitor/NameManglingVisitor.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/logging/StringTable.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericVisitor;
using zillians::language::tree::visitor::NameManglingVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * LiteralCompactionStageVisitor is the visitation helper for LiteralCompactionStage
 *
 * @see LiteralCompactionStage
 */
struct LiteralCompactionStageVisitor : public GenericVisitor
{
    CREATE_GENERIC_INVOKER(compactInvoker)

	LiteralCompactionStageVisitor() : program(NULL)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(compactInvoker)
	}

	void apply(ASTNode& node)
	{
		GenericVisitor::apply(node);
	}

	void apply(Source& node)
	{
		program = &node;
		GenericVisitor::apply(node);
	}

	void apply(NumericLiteral& node)
	{
		if(PrimitiveType::isIntegerType(node.type))
		{
			if(node.value.i64 <= std::numeric_limits<int8>::max())
			{
				node.type = PrimitiveType::INT8;
			}
			else if(node.value.i64 <= std::numeric_limits<int16>::max())
			{
				node.type = PrimitiveType::INT16;
			}
			else if(node.value.i64 <= std::numeric_limits<int32>::max())
			{
				node.type = PrimitiveType::INT32;
			}
			else if(node.value.i64 <= std::numeric_limits<int64>::max())
			{
				// TODO this will always matched
				node.type = PrimitiveType::INT64;
			}
			else
			{
				// potential overflowed literal, warning here
				LOG_MESSAGE(NUMERIC_LITERAL_OVERFLOW, &node);
			}
		}
		else if(PrimitiveType::isFloatType(node.type))
		{
			if(node.value.f64 <= std::numeric_limits<float>::max())
			{
				node.type = PrimitiveType::FLOAT32;
				node.value.f32 = (float)(node.value.f64);
			}
			else if(node.value.f64 <= std::numeric_limits<double>::max())
			{
				node.type = PrimitiveType::FLOAT64;
			}
			else
			{
				BOOST_ASSERT(false && "reaching unreachable code");
			}
		}
	}

	Source* program;
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LITERALCOMPACTIONSTAGEVISITOR_H_ */
