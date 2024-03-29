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

#ifndef ZILLIANS_LANGUAGE_TREE_BRANCHSTMT_H_
#define ZILLIANS_LANGUAGE_TREE_BRANCHSTMT_H_

#include "language/tree/statement/Statement.h"

namespace zillians { namespace language { namespace tree {

struct BranchStmt : public Statement
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(BranchStmt, (BranchStmt)(Statement)(ASTNode));

	struct OpCode
	{
		enum type {
			BREAK, CONTINUE, RETURN,
		};
		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case BREAK: return L"break";
			case CONTINUE: return L"continue";
			case RETURN: return L"return";
			default: UNREACHABLE_CODE(); return NULL;
			}
		}
	};

	explicit BranchStmt(OpCode::type opcode, ASTNode* result = NULL) : opcode(opcode), result(result)
	{
		if(result) result->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Statement)
		COMPARE_MEMBER(opcode)
		COMPARE_MEMBER(result)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Statement)
		REPLACE_USE_WITH(result)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new BranchStmt(opcode, (result) ? result->clone() : NULL);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Statement>(*this);
    	ar & (int&)opcode;
    	ar & result;
    }

	OpCode::type opcode;
	ASTNode* result;

protected:
	BranchStmt() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_BRANCHSTMT_H_ */
