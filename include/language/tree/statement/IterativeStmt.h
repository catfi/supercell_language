/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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
/**
 * @date Aug 5, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_ITERATIVESTMT_H_
#define ZILLIANS_LANGUAGE_TREE_ITERATIVESTMT_H_

#include "language/tree/statement/Statement.h"
#include "language/tree/expression/Expression.h"
#include "language/tree/basic/Block.h"

namespace zillians { namespace language { namespace tree {

struct IterativeStmt : public Statement
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(IterativeStmt, (IterativeStmt)(Statement)(ASTNode));

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Statement)
    	END_COMPARE()
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<Statement>(*this);
    }
};

struct ForStmt : public IterativeStmt
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ForStmt, (ForStmt)(IterativeStmt)(Statement)(ASTNode));

	explicit ForStmt(ASTNode* init, ASTNode* cond, ASTNode* step, ASTNode* block = NULL) : init(init), cond(cond), step(step), block(block)
	{
		BOOST_ASSERT(init && "null init for for statement is not allowed");
		BOOST_ASSERT(cond && "null cond for for  statement is not allowed");
		BOOST_ASSERT(step && "null step for for  statement is not allowed");

		init->parent = this;
		cond->parent = this;
		step->parent = this;
		if(block) block->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(IterativeStmt)
		COMPARE_MEMBER(init)
		COMPARE_MEMBER(cond)
		COMPARE_MEMBER(step)
		COMPARE_MEMBER(block)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(IterativeStmt)
		REPLACE_USE_WITH(init)
		REPLACE_USE_WITH(cond)
		REPLACE_USE_WITH(step)
		REPLACE_USE_WITH(block)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new ForStmt(
    			(init) ? init->clone() : NULL,
    			(cond) ? cond->clone() : NULL,
    			(step) ? step->clone() : NULL,
    			(block) ? block->clone() : NULL);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<IterativeStmt>(*this);
    	ar & init;
    	ar & cond;
    	ar & step;
    	ar & block;
    }

	ASTNode* init;
	ASTNode* cond;
	ASTNode* step;
	ASTNode* block;

protected:
	ForStmt() { }
};

struct ForeachStmt : public IterativeStmt
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ForeachStmt, (ForeachStmt)(IterativeStmt)(Statement)(ASTNode));

	explicit ForeachStmt(ASTNode* iterator, Expression* range, ASTNode* block = NULL) : iterator(iterator), range(range), block(block)
	{
		BOOST_ASSERT(iterator && "null iterator for foreach statement is not allowed");
		BOOST_ASSERT(range && "null range for foreach statement is not allowed");

		iterator->parent = this;
		range->parent = this;
		if(block) block->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(IterativeStmt)
		COMPARE_MEMBER(iterator)
		COMPARE_MEMBER(range)
		COMPARE_MEMBER(block)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(IterativeStmt)
		REPLACE_USE_WITH(iterator)
		REPLACE_USE_WITH(range)
		REPLACE_USE_WITH(block)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new ForeachStmt(
    			(iterator) ? iterator->clone() : NULL,
    			(range) ? cast<Expression>(range->clone()) : NULL,
    			(block) ? block->clone() : NULL);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<IterativeStmt>(*this);
    	ar & iterator;
    	ar & range;
    	ar & block;
    }

	ASTNode* iterator; // TODO semantic-check: it must be L-value expression or declarative statement
	Expression* range;
	ASTNode* block;

protected:
	ForeachStmt() { }
};

struct WhileStmt : public IterativeStmt
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(WhileStmt, (WhileStmt)(IterativeStmt)(Statement)(ASTNode));

	struct Style
	{
		enum type
		{
			WHILE, DO_WHILE
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case WHILE: return L"while";
			case DO_WHILE: return L"do-while";
			}
		}
	};

	explicit WhileStmt(Style::type style, Expression* cond, ASTNode* block = NULL) : style(style), cond(cond), block(block)
	{
		BOOST_ASSERT(cond && "null condition for while statement is not allowed");

		cond->parent = this;
		if(block) block->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(IterativeStmt)
		COMPARE_MEMBER(style)
		COMPARE_MEMBER(cond)
		COMPARE_MEMBER(block)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(IterativeStmt)
		REPLACE_USE_WITH(cond)
		REPLACE_USE_WITH(block)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new WhileStmt(
    			style,
    			(cond) ? cast<Expression>(cond->clone()) : NULL,
    			(block) ? block->clone() : NULL);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<IterativeStmt>(*this);
    	ar & (int&)style;
    	ar & cond;
    	ar & block;
    }

	Style::type style;
	Expression* cond;
	ASTNode* block;

protected:
	WhileStmt() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ITERATIVESTMT_H_ */
