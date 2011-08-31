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
#include <vector>

#define LEFT_TO_RIGHT(op_code) \
	{ \
		if(_attr(0).size() == 1) \
			_value = *beginof(_attr(0)); \
		else { \
			Expression* left = NULL; \
			foreach(i, _attr(0)) \
				left = (i == beginof(_attr(0))) ? *i : new BinaryExpr(op_code, left, *i); \
			_value = left; \
		} \
	}

#if 0 // NOTE: unused
#define RIGHT_TO_LEFT(op_code) \
	{ \
		if(_attr(0).size() == 1) \
			_value = *r_beginof(_attr(0)); \
		else { \
			Expression* right = NULL; \
			r_foreach(i, _attr(0)) \
				right = (i == r_beginof(_attr(0))) ? *i : new BinaryExpr(op_code, *i, right); \
			_value = right; \
		} \
	}
#endif

#define LEFT_TO_RIGHT_VEC(op_code_vec) \
	{ \
		if(_attr(0).size() == 1) \
			_value = *beginof(_attr(0)); \
		else { \
			Expression* left = NULL; \
			decltype(beginof(op_code_vec)) j = beginof(op_code_vec); \
			foreach(i, _attr(0)) { \
				if(i == beginof(_attr(0))) { \
					left = *i; \
					continue; \
				} \
				left = new BinaryExpr(*j, left, *i); \
				j++; \
			} \
			_value = left; \
		} \
	}

#define RIGHT_TO_LEFT_VEC(op_code_vec) \
	{ \
		if(_attr(0).size() == 1) \
			_value = *r_beginof(_attr(0)); \
		else { \
			Expression* right = NULL; \
			decltype(r_beginof(op_code_vec)) j = r_beginof(op_code_vec); \
			r_foreach(i, _attr(0)) { \
				if(i == r_beginof(_attr(0))) { \
					right = *i; \
					continue; \
				} \
				right = new BinaryExpr(*j, *i, right); \
				j++; \
			} \
			_value = right; \
		} \
	}

namespace zillians { namespace language { namespace action {

typedef std::vector<BinaryExpr::OpCode::type> binary_ops_t;

struct primary_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("primary_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new PrimaryExpr(_attr(0));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init_bool, bool Value)
	{
		printf("primary_expression::init_bool attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new PrimaryExpr(new NumericLiteral(Value));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init_object_literal, ObjectLiteral::LiteralType::type Type)
	{
		printf("primary_expression::init_object_literal attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new PrimaryExpr(new ObjectLiteral(Type));
	}
	END_ACTION

	BEGIN_ACTION(init_paren_expression)
	{
		printf("primary_expression::init_paren_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = _attr(0);
	}
	END_ACTION

	BEGIN_ACTION(init_lambda)
	{
		printf("primary_expression::lambda_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("primary_expression::lambda_expression attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("primary_expression::lambda_expression attr(2) type = %s\n", typeid(_attr_t(2)).name());
		typed_parameter_list::value_t*         parameters = _attr(0).is_initialized() ? (*_attr(0)).get() : NULL;
		TypeSpecifier*                         type       = _attr(1).is_initialized() ? *_attr(1) : NULL;
		Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;
		Declaration::StorageSpecifier::type    storage    = Declaration::StorageSpecifier::NONE;
		bool                                   is_member  = false;
		FunctionDecl* function_decl = new FunctionDecl(NULL, type, is_member, visibility, storage, _attr(2));
		if(!!parameters)
			BOOST_FOREACH(auto i, *parameters)
				function_decl->appendParameter(i.first, i.second);
		_value = new PrimaryExpr(function_decl);
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

	BEGIN_ACTION(append_postfix_array)
	{
		printf("postfix_expression::init_postfix_array attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new BinaryExpr(BinaryExpr::OpCode::ARRAY_SUBSCRIPT, _value, _attr(0));
	}
	END_ACTION

	BEGIN_ACTION(append_postfix_call)
	{
		printf("postfix_expression::init_postfix_call attr(0) type = %s\n", typeid(_attr_t(0)).name());
		CallExpr* call_expr = new CallExpr(_value);
		if(_attr(0).is_initialized())
			BOOST_FOREACH(auto i, *(_attr(0)))
				call_expr->appendParameter(i);
		_value = call_expr;
	}
	END_ACTION

	BEGIN_ACTION(append_postfix_member)
	{
		printf("postfix_expression::init_postfix_member attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new MemberExpr(_value, _attr(0));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(append_postfix_step, UnaryExpr::OpCode::type Type)
	{
		printf("postfix_expression::append_postfix_step attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new UnaryExpr(Type, _value);
	}
	END_ACTION
};

struct prefix_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("prefix_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		switch(_attr(0).which())
		{
		case 0:
			_value = boost::get<Expression*>(_attr(0));
			break;
		case 1:
			typedef boost::fusion::vector2<UnaryExpr::OpCode::type, Expression*> fusion_vec_t;
			fusion_vec_t &vec = boost::get<fusion_vec_t>(_attr(0));
			UnaryExpr::OpCode::type type = boost::fusion::at_c<0>(vec);
			Expression*             expr = boost::fusion::at_c<1>(vec);
			_value = new UnaryExpr(type, expr);
			break;
		}
	}
	END_ACTION
};

struct left_to_right_binary_op_vec
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(shared_ptr<binary_ops_t>)

	BEGIN_ACTION(append_op)
	{
		printf("left_to_right_binary_op_vec::append_op attr(0) type = %s\n", typeid(_attr_t(0)).name());
		if(!_local(0))
			_local(0).reset(new binary_ops_t());
		_local(0)->push_back(_attr(0));
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
		printf("left_to_right_binary_op_vec attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT_VEC(*_local(0));
	}
	END_ACTION
};

struct right_to_left_binary_op_vec
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(shared_ptr<binary_ops_t>)

	BEGIN_ACTION(append_op)
	{
		printf("right_to_left_binary_op_vec::append_op attr(0) type = %s\n", typeid(_attr_t(0)).name());
		if(!_local(0))
			_local(0).reset(new binary_ops_t());
		_local(0)->push_back(_attr(0));
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
		printf("right_to_left_binary_op_vec attr(0) type = %s\n", typeid(_attr_t(0)).name());
		RIGHT_TO_LEFT_VEC(*_local(0));
	}
	END_ACTION
};

struct left_to_right_binary_op
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_TEMPLATED_ACTION(init, BinaryExpr::OpCode::type Type)
	{
		printf("left_to_right_binary_op attr(0) type = %s\n", typeid(_attr_t(0)).name());
		LEFT_TO_RIGHT(Type);
	}
	END_ACTION
};

#if 0 // NOTE: unused
struct right_to_left_binary_op
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()

	BEGIN_TEMPLATED_ACTION(init, BinaryExpr::OpCode::type Type)
	{
		printf("right_to_left_binary_op attr(0) type = %s\n", typeid(_attr_t(0)).name());
		RIGHT_TO_LEFT(Type);
	}
	END_ACTION
};
#endif

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
		if(!_attr(1).is_initialized())
		{
			_value = _attr(0);
			return;
		}
		Expression* true_node  = boost::fusion::at_c<0>(*_attr(1));
		Expression* false_node = boost::fusion::at_c<1>(*_attr(1));
		_value = new TernaryExpr(_attr(0), true_node, false_node);
	}
	END_ACTION
};

struct expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(shared_ptr<binary_ops_t>)

	BEGIN_ACTION(append_op)
	{
		printf("assignment_expression::append_op attr(0) type = %s\n", typeid(_attr_t(0)).name());
		if(!_local(0))
			_local(0).reset(new binary_ops_t());
		_local(0)->push_back(_attr(0));
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
		printf("assignment_expression attr(0) type = %s\n", typeid(_attr_t(0)).name());
		RIGHT_TO_LEFT_VEC(*_local(0));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_EXPRESSIONACTIONS_H_ */
