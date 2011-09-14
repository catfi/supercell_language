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

	explicit PrimaryExpr(Identifier* identifier) : catagory(Catagory::IDENTIFIER)
	{
		BOOST_ASSERT(identifier && "null identifier for primary expression is not allowed");

		identifier->parent = this;
		value.identifier = identifier;
	}

	explicit PrimaryExpr(Literal* literal) : catagory(Catagory::LITERAL)
	{
		BOOST_ASSERT(literal && "null literal for primary expression is not allowed");

		literal->parent = this;
		value.literal = literal;
	}

	explicit PrimaryExpr(FunctionDecl* lambda) : catagory(Catagory::LAMBDA)
	{
		BOOST_ASSERT(lambda && "null lambda for primary expression is not allowed");

		lambda->parent = this;
		value.lambda = lambda;
	}

	virtual bool isRValue()
	{
		switch(catagory)
		{
		case Catagory::IDENTIFIER: return false;
		case Catagory::LITERAL: return true;
		case Catagory::LAMBDA: return true;
		}
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const PrimaryExpr* p = cast<const PrimaryExpr>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!Expression::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(catagory != p->catagory)
        {
            return false;
        }
        switch (catagory)
        {
        case Catagory::IDENTIFIER: if(!isASTNodeMemberEqual(&PrimaryExpr::ValueUnion::identifier, value, p->value, visited)) return false; break;
        case Catagory::LITERAL   : if(!isASTNodeMemberEqual(&PrimaryExpr::ValueUnion::literal   , value, p->value, visited)) return false; break;
        case Catagory::LAMBDA    : if(!isASTNodeMemberEqual(&PrimaryExpr::ValueUnion::lambda    , value, p->value, visited)) return false; break;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

	Catagory::type catagory;

	union ValueUnion
	{
		Identifier* identifier;
		Literal* literal;
		FunctionDecl* lambda;
	} value;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PRIMARYEXPR_H_ */
