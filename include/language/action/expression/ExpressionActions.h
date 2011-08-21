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

#ifndef ZILLIANS_LANGUAGE_ACTION_DECLARATION_EXPRESSIONACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_DECLARATION_EXPRESSIONACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

#define RIGHT_TO_LEFT(op_code) \
	{ \
		Expression* right = NULL; \
		r_foreach(i, _attr(0)) \
			right = (i == r_beginof(_attr(0))) ? *i : new BinaryExpr(op_code, *i, right); \
		_value = right; \
	}

#define LEFT_TO_RIGHT(op_code) \
	{ \
		Expression* left = NULL; \
		foreach(i, _attr(0)) \
			left = (i == _attr(0).begin()) ? *i : new BinaryExpr(op_code, left, *i); \
		_value = left; \
	}

namespace zillians { namespace language { namespace action {

struct primary_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_template_arg_identifier)
	{
		printf("primary_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
//		_value = _attr(0);
	}
	END_ACTION
};

struct postfix_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_primary_expression)
	{
		printf("postfix_expression::init_primary_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = _attr(0);
	}
	END_ACTION

	BEGIN_ACTION(init_postfix_array)
	{
		printf("postfix_expression::init_postfix_array attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION

	BEGIN_ACTION(init_postfix_call)
	{
		printf("postfix_expression::init_postfix_call attr(0) type = %s\n", typeid(_attr_t(0)).name());
//		CallExpr* call_expr = new CallExpr(_value);
//		if(_attr(0).is_initialized())
//		{
//			foreach(i, *_attr(0))
//				call_expr->appendParameter(*i);
//		}
//		_value = call_expr;
	}
	END_ACTION

	BEGIN_ACTION(init_postfix_member)
	{
		printf("postfix_expression::init_postfix_member attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new MemberExpr(_value, _attr(0));
	}
	END_ACTION

	BEGIN_ACTION(init_postfix_inc)
	{
		printf("postfix_expression::init_postfix_inc attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new UnaryExpr(UnaryExpr::OpCode::POSTFIX_INCREMENT, _value);
	}
	END_ACTION

	BEGIN_ACTION(init_postfix_dec)
	{
		printf("postfix_expression::init_postfix_dec attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new UnaryExpr(UnaryExpr::OpCode::POSTFIX_DECREMENT, _value);
	}
	END_ACTION
};

struct prefix_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(UnaryExpr::OpCode::type)

	BEGIN_ACTION(init_postfix_expression)
	{
		printf("prefix_expression::init_postfix_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = _attr(0);
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
		printf("prefix_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new UnaryExpr(_local(0), _attr(0));
	}
	END_ACTION
};

struct multiplicative_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(BinaryExpr::OpCode::type)

	BEGIN_ACTION(init)
	{
		printf("multiplicative_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(_local(0));
	}
	END_ACTION
};

struct additive_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(BinaryExpr::OpCode::type)

	BEGIN_ACTION(init)
	{
		printf("additive_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(_local(0));
	}
	END_ACTION
};

struct shift_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(BinaryExpr::OpCode::type)

	BEGIN_ACTION(init)
	{
		printf("shift_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(_local(0));
	}
	END_ACTION
};

struct relational_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(BinaryExpr::OpCode::type)

	BEGIN_ACTION(init)
	{
		printf("relational_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(_local(0));
	}
	END_ACTION
};

struct equality_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(BinaryExpr::OpCode::type)

	BEGIN_ACTION(init)
	{
		printf("equality_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(_local(0));
	}
	END_ACTION
};

struct and_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("and_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(BinaryExpr::OpCode::BINARY_AND);
	}
	END_ACTION
};

struct xor_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("xor_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(BinaryExpr::OpCode::BINARY_XOR);
	}
	END_ACTION
};

struct or_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("or_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(BinaryExpr::OpCode::BINARY_OR);
	}
	END_ACTION
};

struct logical_and_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("logical_and_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(BinaryExpr::OpCode::LOGICAL_AND);
	}
	END_ACTION
};

struct logical_or_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("logical_or_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(BinaryExpr::OpCode::LOGICAL_OR);
	}
	END_ACTION
};

struct range_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("range_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("range_expression attr(1) type = %s\n", typeid(_attr_t(1)).name());
		if(!_attr(1).is_initialized())
		{
			_value = _attr(0);
			return;
		}
		_value = new BinaryExpr(BinaryExpr::OpCode::RANGE_ELLIPSIS, _attr(0), *_attr(1));
	}
	END_ACTION
};

struct ternary_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("ternary_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("ternary_expression attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("ternary_expression attr(2) type = %s\n", typeid(_attr_t(2)).name());
		if(!_attr(1).is_initialized())
		{
			_value = _attr(0);
			return;
		}
		ASTNode* true_node  = boost::fusion::at_c<0>(*_attr(1));
		ASTNode* false_node = boost::fusion::at_c<1>(*_attr(1));
		_value = new TernaryExpr(_attr(0), true_node, false_node);
	}
	END_ACTION
};

struct expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(BinaryExpr::OpCode::type)

	BEGIN_ACTION(init)
	{
		printf("assignment_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		RIGHT_TO_LEFT(_local(0));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_EXPRESSIONACTIONS_H_ */
