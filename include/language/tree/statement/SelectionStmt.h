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

struct Selection : ContextHub<ContextOwnership::transfer>
{
	Selection() : cond(NULL), block(NULL)
	{ }

	Selection(Expression* cond, ASTNode* block) : cond(cond), block(block)
	{ }

	Selection(const Selection& s) : cond(s.cond), block(s.block)
	{ }

	Selection& operator= (const Selection& s)
	{
		cond = s.cond; block = s.block;
		return *this;
	}

	bool isEqualImpl(const Selection& rhs, ASTNodeSet& visited) const
	{
		const Selection* p = &rhs;
		COMPARE_MEMBER(cond)
		COMPARE_MEMBER(block)
		return true;
	}

    bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(cond)
		REPLACE_USE_WITH(block)
    	END_REPLACE()
    }

    Selection clone() const
    {
    	Selection s((cond) ? cast<Expression>(cond->clone()) : NULL, (block) ? block->clone() : NULL);
    	return s;
    }

	Expression* cond;
	ASTNode* block;
};

struct SelectionStmt : public Statement
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(SelectionStmt, (SelectionStmt)(Statement)(ASTNode));

	virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
	{
		BEGIN_COMPARE_WITH_BASE(Statement)
		END_COMPARE()
	}

    bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Statement)
    	END_REPLACE()
    }
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

	void setElseBranch(ASTNode* block)
	{
		BOOST_ASSERT(block != NULL);

		block->parent = this;
		else_block = block;
	}

	virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
	{
		BEGIN_COMPARE_WITH_BASE(SelectionStmt)
		COMPARE_MEMBER(if_branch)
		COMPARE_MEMBER(elseif_branches)
		COMPARE_MEMBER(else_block)
		END_COMPARE()
	}

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(SelectionStmt)
		REPLACE_USE_WITH(if_branch)
		REPLACE_USE_WITH(elseif_branches)
		REPLACE_USE_WITH(else_block)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	IfElseStmt* cloned = new IfElseStmt(if_branch.clone());

    	foreach(i, elseif_branches)
    		cloned->addElseIfBranch(i->clone());

    	cloned->setElseBranch((else_block) ? else_block->clone() : NULL);

    	return cloned;
    }

	Selection if_branch;
	std::vector<Selection> elseif_branches;
	ASTNode* else_block;
};

struct SwitchStmt : public SelectionStmt
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(SwitchStmt, (SwitchStmt)(SelectionStmt)(Statement)(ASTNode));

	explicit SwitchStmt(Expression* node) : node(node), default_block(NULL)
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

	void setDefaultCase(ASTNode* block)
	{
		if(default_block) default_block->parent = NULL;
		default_block = block;
		if(default_block) default_block->parent = this;
	}

	virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
	{
		BEGIN_COMPARE_WITH_BASE(SelectionStmt)
		COMPARE_MEMBER(node)
		COMPARE_MEMBER(cases)
		COMPARE_MEMBER(default_block);
		END_COMPARE()
	}

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(SelectionStmt)
		REPLACE_USE_WITH(node)
		REPLACE_USE_WITH(cases)
		REPLACE_USE_WITH(default_block)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	SwitchStmt* cloned = new SwitchStmt((node) ? cast<Expression>(node->clone()) : NULL);

    	foreach(i, cases)
    		cloned->addCase(i->clone());

    	cloned->setDefaultCase((default_block) ? default_block->clone() : NULL);

    	return cloned;
    }

	Expression* node;
	std::vector<Selection> cases;
	ASTNode* default_block;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_SELECTIONSTMT_H_ */
