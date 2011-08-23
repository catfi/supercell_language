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

#ifndef ZILLIANS_LANGUAGE_TREE_PRIMARYEXPR_H_
#define ZILLIANS_LANGUAGE_TREE_PRIMARYEXPR_H_

#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/Literal.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct PrimaryExpr : public Expression
{
	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(PrimaryExpr, (PrimaryExpr)(Expression)(ASTNode));

	struct Catagory
	{
		enum type {
			IDENTIFIER, LITERAL, LAMBDA,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case IDENTIFIER: return L"identifier";
			case LITERAL: return L"literal";
			case LAMBDA: return L"lambda";
			}
		}
	};

	explicit PrimaryExpr(Identifier* identifier) : type(Catagory::IDENTIFIER)
	{
		BOOST_ASSERT(identifier && "null identifier for primary expression is not allowed");

		identifier->parent = this;
		value.identifier = identifier;
	}

	explicit PrimaryExpr(Literal* literal) : type(Catagory::LITERAL)
	{
		BOOST_ASSERT(literal && "null literal for primary expression is not allowed");

		literal->parent = this;
		value.literal = literal;
	}

	explicit PrimaryExpr(FunctionDecl* lambda) : type(Catagory::LAMBDA)
	{
		BOOST_ASSERT(lambda && "null lambda for primary expression is not allowed");

		lambda->parent = this;
		value.lambda = lambda;
	}

	Catagory::type type;

	union
	{
		Identifier* identifier;
		Literal* literal;
		FunctionDecl* lambda;
	} value;
};

} } }


#endif /* ZILLIANS_LANGUAGE_TREE_PRIMARYEXPR_H_ */