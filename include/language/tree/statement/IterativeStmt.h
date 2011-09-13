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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const IterativeStmt* p = cast<const IterativeStmt>(&rhs);
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

struct ForeachStmt : public IterativeStmt
{
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
        if(visited.count(this))
        {
            return true ;
        }

        const ForeachStmt* p = cast<const ForeachStmt>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!IterativeStmt::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(!isASTNodeMemberEqual   (&ForeachStmt::iterator            , *this, *p, visited)) return false;
        if(!isASTNodeMemberEqual   (&ForeachStmt::range               , *this, *p, visited)) return false;
        if(!isASTNodeMemberEqual   (&ForeachStmt::block               , *this, *p, visited)) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<IterativeStmt>(*this);
        ar & iterator;
        ar & range;
        ar & block;
    }

	ASTNode* iterator; // TODO semantic-check: it must be L-value expression or declarative statement
	Expression* range;
	ASTNode* block;
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

	explicit WhileStmt(Style::type style, Expression* cond, ASTNode* block = NULL) : style(style), cond(cond), block(block)
	{
		BOOST_ASSERT(cond && "null condition for while statement is not allowed");

		cond->parent = this;
		if(block) block->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const WhileStmt* p = cast<const WhileStmt>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!IterativeStmt::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(style != p->style                                                             ) return false;
        if(!isASTNodeMemberEqual   (&WhileStmt::cond                , *this, *p, visited)) return false;
        if(!isASTNodeMemberEqual   (&WhileStmt::block               , *this, *p, visited)) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<IterativeStmt>(*this);
        ar & cond;
        ar & block;
    }

	Style::type style;
	Expression* cond;
	ASTNode* block;
};

} } }

namespace boost { namespace serialization {

// ForStmt
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ForeachStmt* p, const unsigned int file_version)
{
    ar << p->iterator;
    ar << p->range;
    ar << p->block;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ForeachStmt* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	ASTNode* iterator; // TODO semantic-check: it must be L-value expression or declarative statement
	Expression* range;
	ASTNode* block;

    ar >> iterator;
    ar >> range;
    ar >> block;

	::new(p) ForeachStmt(iterator, range, block);
}

// WhileStmt
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::WhileStmt* p, const unsigned int file_version)
{
    ar << (int&)p->style;
    ar << p->cond;
    ar << p->block;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::WhileStmt* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	int style;
	Expression* cond;
	ASTNode* block;

    ar >> style;
    ar >> cond;
    ar >> block;

	::new(p) WhileStmt(static_cast<WhileStmt::Style::type>(style), cond, block);
}

} } // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_ITERATIVESTMT_H_ */
