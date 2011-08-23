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
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(IterativeStmt, (IterativeStmt)(Statement)(ASTNode));
};

struct ForeachStmt : public IterativeStmt
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ForeachStmt, (ForeachStmt)(IterativeStmt)(Statement)(ASTNode));

	explicit ForeachStmt(Expression* iterator, Expression* range, Block* block = NULL) : iterator(iterator), range(range), block( (block == NULL) ? new Block() : block )
	{
		BOOST_ASSERT(iterator && "null iterator for foreach statement is not allowed");
		BOOST_ASSERT(range && "null range for foreach statement is not allowed");

		iterator->parent = this;
		range->parent = this;
		if(block) block->parent = this;
	}

	Expression* iterator;
	Expression* range;
	Block* block;
};

struct WhileStmt : public IterativeStmt
{
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

	explicit WhileStmt(Expression* cond, Style::type style, Block* block = NULL) : cond(cond), style(style), block( (block == NULL) ? new Block() : block )
	{
		BOOST_ASSERT(cond && "null condition for while statement is not allowed");

		cond->parent = this;
		if(block) block->parent = this;
	}

	Expression* cond;
	Style::type style;
	Block* block;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ITERATIVESTMT_H_ */
