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
	friend class boost::serialization::access;

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
			default: UNREACHABLE_CODE(); return NULL;
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

	virtual bool isRValue() const
	{
		switch(catagory)
		{
		case Catagory::IDENTIFIER: return false;
		case Catagory::LITERAL: return true;
		case Catagory::LAMBDA: return true;
		default: UNREACHABLE_CODE(); return false;
		}
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Expression)
		COMPARE_MEMBER(catagory)
        switch (catagory)
        {
        case Catagory::IDENTIFIER: COMPARE_MEMBER(value.identifier); break;
        case Catagory::LITERAL   : COMPARE_MEMBER(value.literal   ); break;
		case Catagory::LAMBDA    : COMPARE_MEMBER(value.lambda    ); break;
		default: UNREACHABLE_CODE(); break;
        }
    	END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Expression)
		switch (catagory)
		{
		case Catagory::IDENTIFIER: REPLACE_USE_WITH(value.identifier); break;
		case Catagory::LITERAL: REPLACE_USE_WITH(value.literal); break;
		case Catagory::LAMBDA: REPLACE_USE_WITH(value.lambda); break;
		default: UNREACHABLE_CODE(); break;
		}
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
        switch (catagory)
        {
        case Catagory::IDENTIFIER: return new PrimaryExpr(cast<Identifier>(value.identifier->clone()));
        case Catagory::LITERAL   : return new PrimaryExpr(cast<Literal>(value.literal->clone()));
		case Catagory::LAMBDA    : return new PrimaryExpr(cast<FunctionDecl>(value.lambda->clone()));
		default: UNREACHABLE_CODE(); break;
        }
        return NULL;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Expression>(*this);
    	ar & (int&)catagory;
        switch (catagory)
        {
        case Catagory::IDENTIFIER: ar & value.identifier; break;
        case Catagory::LITERAL   : ar & value.literal; break;
		case Catagory::LAMBDA    : ar & value.lambda; break;
		default: UNREACHABLE_CODE(); break;
        }
    }

	Catagory::type catagory;

	union ValueUnion
	{
		Identifier* identifier;
		Literal* literal;
		FunctionDecl* lambda;
	} value;

protected:
	PrimaryExpr() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PRIMARYEXPR_H_ */
