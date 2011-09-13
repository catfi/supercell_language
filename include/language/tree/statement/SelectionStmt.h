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
    Selection() : cond(NULL), block(NULL)
    { }

	Selection(Expression* cond, ASTNode* block) : cond(cond), block(block)
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

    bool isEqualImpl(const Selection& rhs, ASTNode::ASTNodeSet& visited) const
    {
        // compare data member
        if(!cond ->isEqualImpl(*rhs.cond , visited)) return false;
        if(!block->isEqualImpl(*rhs.block, visited)) return false;

        return true;
    }
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & cond;
        ar & block;
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
        if(visited.count(this))
        {
            return true ;
        }

        const SelectionStmt* p = cast<const SelectionStmt>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!Statement::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        // no data member

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<Statement>(*this);
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
        if(visited.count(this))
        {
            return true ;
        }

        const IfElseStmt* p = cast<const IfElseStmt>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!SelectionStmt::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(!if_branch.isEqualImpl(p->if_branch, visited))
        {
            return false;
        }
        if(elseif_branches.size() != p->elseif_branches.size())
        {
            return false ;
        }
        for(size_t i = 0 ; i < elseif_branches.size() ; ++i)
        {
            if(!elseif_branches[i].isEqualImpl(p->elseif_branches[i], visited))
            {
                return false;
            }
        }
        if(!isASTNodeMemberEqual(&IfElseStmt::else_block, *this, *p, visited))
        {
            return false;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<SelectionStmt>(*this);
        ar & elseif_branches;
        ar & else_block;
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
        if(visited.count(this))
        {
            return true ;
        }

        const SwitchStmt* p = cast<const SwitchStmt>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!SelectionStmt::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(!isASTNodeMemberEqual(&SwitchStmt::node, *this, *p, visited))
        {
            return false;
        }
        if(cases.size() != p->cases.size())
        {
            return false ;
        }
        for(size_t i = 0 ; i < cases.size() ; ++i)
        {
            if(!cases[i].isEqualImpl(p->cases[i], visited))
            {
                return false;
            }
        }
        if(!isASTNodeMemberEqual(&SwitchStmt::default_block, *this, *p, visited))
        {
            return false;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<SelectionStmt>(*this);
        ar & cases;
        ar & default_block;
    }

	Expression* node;
	std::vector<Selection> cases;
	ASTNode* default_block;
};

} } }

namespace boost { namespace serialization {

// if else
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::IfElseStmt* p, const unsigned int file_version)
{
    ar << p->if_branch;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::IfElseStmt* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

    Expression* selectionExpression;
    ASTNode*    selectionASTNode;
    ar >> selectionExpression;
    ar >> selectionASTNode;
    Selection if_branch(selectionExpression, selectionASTNode);
	::new(p) IfElseStmt(if_branch);
}

// switch
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::SwitchStmt* p, const unsigned int file_version)
{
    ar << p->node;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::SwitchStmt* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	Expression* node;
    ar >> node;
	::new(p) SwitchStmt(node);
}

} } // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_SELECTIONSTMT_H_ */
