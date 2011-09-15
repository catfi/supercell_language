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

#ifndef ZILLIANS_LANGUAGE_TREE_UNARYEXPR_H_
#define ZILLIANS_LANGUAGE_TREE_UNARYEXPR_H_

#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct UnaryExpr : public Expression
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(UnaryExpr, (UnaryExpr)(Expression)(ASTNode));

	struct OpCode
	{
		enum type {
			POSTFIX_INCREMENT, POSTFIX_DECREMENT, PREFIX_INCREMENT, PREFIX_DECREMENT,
			BINARY_NOT, LOGICAL_NOT,
			ARITHMETIC_NEGATE,
			NEW,
			NOOP,
			INVALID,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case POSTFIX_INCREMENT: return L"postfix_inc";
			case POSTFIX_DECREMENT: return L"postfix_dec";
			case PREFIX_INCREMENT: return L"prefix_inc";
			case PREFIX_DECREMENT: return L"prefix_dec";
			case BINARY_NOT: return L"~";
			case LOGICAL_NOT: return L"!";
			case ARITHMETIC_NEGATE: return L"-";
			case NEW: return L"new";
			case NOOP: return L"no-op";
			case INVALID: return L"invalid";
			}
		}
	};

	explicit UnaryExpr(OpCode::type opcode, ASTNode* node) : opcode(opcode), node(node)
	{
		BOOST_ASSERT(node && "null node for unary expression is not allowed");

		node->parent = this;
	}

	virtual bool isRValue()
	{
		switch(opcode)
		{
		case OpCode::PREFIX_INCREMENT:
		case OpCode::PREFIX_DECREMENT:
			return false;
		case OpCode::POSTFIX_INCREMENT:
		case OpCode::POSTFIX_DECREMENT:
		case OpCode::BINARY_NOT:
		case OpCode::LOGICAL_NOT:
		case OpCode::ARITHMETIC_NEGATE:
		case OpCode::NEW:
		case OpCode::NOOP:
		case OpCode::INVALID:
			return true;
		}
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const UnaryExpr* p = cast<const UnaryExpr>(&rhs);
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
        if(opcode != p->opcode)
        {
            return false;
        }
        if(!isASTNodeMemberEqual(&UnaryExpr::node, *this, *p, visited))
        {
            return false;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

	OpCode::type opcode;
	ASTNode* node;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_UNARYEXPR_H_ */
