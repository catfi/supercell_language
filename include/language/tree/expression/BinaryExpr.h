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

#ifndef ZILLIANS_LANGUAGE_TREE_BINARYEXPR_H_
#define ZILLIANS_LANGUAGE_TREE_BINARYEXPR_H_

#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct BinaryExpr : public Expression
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(BinaryExpr, (BinaryExpr)(Expression)(ASTNode));

	struct OpCode
	{
		enum type {
			ASSIGN, RSHIFT_ASSIGN, LSHIFT_ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN,
			ARITHMETIC_ADD, ARITHMETIC_SUB, ARITHMETIC_MUL, ARITHMETIC_DIV, ARITHMETIC_MOD,
			BINARY_AND, BINARY_OR, BINARY_XOR, BINARY_LSHIFT, BINARY_RSHIFT,
			LOGICAL_AND, LOGICAL_OR,
			INSTANCEOF,
			COMPARE_EQ, COMPARE_NE, COMPARE_GT, COMPARE_LT, COMPARE_GE, COMPARE_LE,
			RANGE_ELLIPSIS,
			ARRAY_SUBSCRIPT,
			INVALID,
		};
		static const wchar_t* toString(type op)
		{
			switch(op)
			{
			case ASSIGN: return L"=";
			case RSHIFT_ASSIGN: return L">>=";
			case LSHIFT_ASSIGN: return L"<<=";
			case ADD_ASSIGN: return L"+=";
			case SUB_ASSIGN: return L"-=";
			case MUL_ASSIGN: return L"*=";
			case DIV_ASSIGN: return L"/=";
			case MOD_ASSIGN: return L"%=";
			case AND_ASSIGN: return L"&=";
			case OR_ASSIGN: return L"|=";
			case XOR_ASSIGN: return L"^=";
			case ARITHMETIC_ADD: return L"+";
			case ARITHMETIC_SUB: return L"-";
			case ARITHMETIC_MUL: return L"*";
			case ARITHMETIC_DIV: return L"/";
			case ARITHMETIC_MOD: return L"%";
			case BINARY_AND: return L"&";
			case BINARY_OR: return L"|";
			case BINARY_XOR: return L"^";
			case BINARY_LSHIFT: return L"<<";
			case BINARY_RSHIFT: return L">>";
			case LOGICAL_AND: return L"&&";
			case LOGICAL_OR: return L"||";
			case INSTANCEOF: return L"instanceof";
			case COMPARE_EQ: return L"==";
			case COMPARE_NE: return L"!=";
			case COMPARE_GT: return L">";
			case COMPARE_LT: return L"<";
			case COMPARE_GE: return L">=";
			case COMPARE_LE: return L"<=";
			case RANGE_ELLIPSIS: return L"...";
			case ARRAY_SUBSCRIPT: return L"[]";
			case INVALID: return L"invalid";
			}
		}
	};


	explicit BinaryExpr(OpCode::type opcode, Expression* left, Expression* right) : opcode(opcode), left(left), right(right)
	{
		BOOST_ASSERT(left && "null left child for binary expression is not allowed");
		BOOST_ASSERT(right && "null right child for binary expression is not allowed");

		left->parent = this;
		right->parent = this;
	}

	bool isArithmetic()
	{
		if( opcode == OpCode::ARITHMETIC_ADD ||
			opcode == OpCode::ARITHMETIC_SUB ||
			opcode == OpCode::ARITHMETIC_MUL ||
			opcode == OpCode::ARITHMETIC_DIV ||
			opcode == OpCode::ARITHMETIC_MOD )
			return true;
		else
			return false;
	}

	bool isAssignment()
	{
		if( opcode == OpCode::ASSIGN ||
			opcode == OpCode::RSHIFT_ASSIGN ||
			opcode == OpCode::LSHIFT_ASSIGN ||
			opcode == OpCode::ADD_ASSIGN ||
			opcode == OpCode::SUB_ASSIGN ||
			opcode == OpCode::MUL_ASSIGN ||
			opcode == OpCode::DIV_ASSIGN ||
			opcode == OpCode::MOD_ASSIGN ||
			opcode == OpCode::AND_ASSIGN ||
			opcode == OpCode::OR_ASSIGN  ||
			opcode == OpCode::XOR_ASSIGN )
			return true;
		else
			return false;
	}

	bool isBinary()
	{
		if( opcode == OpCode::BINARY_AND ||
			opcode == OpCode::BINARY_OR ||
			opcode == OpCode::BINARY_XOR ||
			opcode == OpCode::BINARY_LSHIFT ||
			opcode == OpCode::BINARY_RSHIFT )
			return true;
		else
			return false;
	}

	bool isLogical()
	{
		if( opcode == OpCode::LOGICAL_AND ||
			opcode == OpCode::LOGICAL_OR ||
			opcode == OpCode::INSTANCEOF )
			return true;
		else
			return false;
	}

	bool isComparison()
	{
		if( opcode == OpCode::COMPARE_EQ ||
			opcode == OpCode::COMPARE_NE ||
			opcode == OpCode::COMPARE_GT ||
			opcode == OpCode::COMPARE_LT ||
			opcode == OpCode::COMPARE_GE ||
			opcode == OpCode::COMPARE_LE )
			return true;
		else
			return false;
	}

	virtual bool isRValue()
	{
		switch(opcode)
		{
		case OpCode::ASSIGN:
		case OpCode::RSHIFT_ASSIGN:
		case OpCode::LSHIFT_ASSIGN:
		case OpCode::ADD_ASSIGN:
		case OpCode::SUB_ASSIGN:
		case OpCode::MUL_ASSIGN:
		case OpCode::DIV_ASSIGN:
		case OpCode::MOD_ASSIGN:
		case OpCode::AND_ASSIGN:
		case OpCode::OR_ASSIGN:
		case OpCode::XOR_ASSIGN:
		case OpCode::ARRAY_SUBSCRIPT:
			return false;
		case OpCode::ARITHMETIC_ADD:
		case OpCode::ARITHMETIC_SUB:
		case OpCode::ARITHMETIC_MUL:
		case OpCode::ARITHMETIC_DIV:
		case OpCode::ARITHMETIC_MOD:
		case OpCode::BINARY_AND:
		case OpCode::BINARY_OR:
		case OpCode::BINARY_XOR:
		case OpCode::BINARY_LSHIFT:
		case OpCode::BINARY_RSHIFT:
		case OpCode::LOGICAL_AND:
		case OpCode::LOGICAL_OR:
		case OpCode::INSTANCEOF:
		case OpCode::COMPARE_EQ:
		case OpCode::COMPARE_NE:
		case OpCode::COMPARE_GT:
		case OpCode::COMPARE_LT:
		case OpCode::COMPARE_GE:
		case OpCode::COMPARE_LE:
		case OpCode::RANGE_ELLIPSIS:
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

        const BinaryExpr* p = cast<const BinaryExpr>(&rhs);
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
		COMPARE_MEMBER(opcode);
		COMPARE_ASTNODE_MEMBER(left);
		COMPARE_ASTNODE_MEMBER(right);

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

	OpCode::type opcode;
	Expression* left;
	Expression* right;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_BINARYEXPR_H_ */
