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

#ifndef ZILLIANS_LANGUAGE_TREE_SELECTIONSTMT_H_
#define ZILLIANS_LANGUAGE_TREE_SELECTIONSTMT_H_

#include "language/tree/statement/Statement.h"

namespace zillians { namespace language { namespace tree {

struct Selection
{
	explicit Selection(Expression* cond, Block* block) : cond(cond), block(block)
	{
		BOOST_ASSERT(cond && "null condition for selection statement is not allowed");
		BOOST_ASSERT(block && "null block for selection statement is not allowed");
	}

	Selection(const Selection& s) : cond(s.cond), block(s.block)
	{ }

	Selection& operator= (const Selection& s)
	{
		cond = s.cond; block = s.block;
		return *this;
	}

	Expression* cond;
	Block* block;
};

struct SelectionStmt : public Statement
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(SelectionStmt, (SelectionStmt)(Statement)(ASTNode));
};

struct IfElseStmt : public SelectionStmt
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(IfElseStmt, (IfElseStmt)(SelectionStmt)(Statement)(ASTNode));

	explicit IfElseStmt(const Selection& branch) : if_branch(branch)
	{
		BOOST_ASSERT(branch.cond != NULL);
		BOOST_ASSERT(branch.block != NULL);

		branch.cond->parent = this;
		branch.block->parent = this;
	}

	void addElseIfBranch(const Selection& branch)
	{
		BOOST_ASSERT(branch.cond != NULL);
		BOOST_ASSERT(branch.block != NULL);

		branch.cond->parent = this;
		branch.block->parent = this;
		elseif_branches.push_back(branch);
	}

	void setElseBranch(Block* block)
	{
		BOOST_ASSERT(block != NULL);

		block->parent = this;
		else_block = block;
	}

	Selection if_branch;
	std::vector<Selection> elseif_branches;
	Block* else_block;
};

struct SwitchStmt : public SelectionStmt
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(SwitchStmt, (SwitchStmt)(SelectionStmt)(Statement)(ASTNode));

	explicit SwitchStmt(Expression* node) : node(node)
	{
		BOOST_ASSERT(node && "null node for switch statement is not allowed");

		node->parent = this;
	}

	void addCase(const Selection& branch)
	{
		BOOST_ASSERT(branch.cond && "null condition for selection statement is not allowed");

		branch.cond->parent = this;
		if(branch.block) branch.block->parent = this;
		cases.push_back(branch);
	}

	Expression* node;
	std::vector<Selection> cases;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_SELECTIONSTMT_H_ */
