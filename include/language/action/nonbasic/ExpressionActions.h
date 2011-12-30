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
		if(_param(0).size() == 1) \
			_result = *make_deduced_begin(_param(0)); \
		else { \
			Expression* left = NULL; \
			deduced_foreach(i, _param(0)) \
				if(is_begin_of_deduced_foreach(i, _param(0))) \
					left = *i; \
				else \
					BIND_CACHED_LOCATION(left = new BinaryExpr(op_code, left, *i)); \
			_result = left; \
		} \
	}

#if 0 // NOTE: unused
#define RIGHT_TO_LEFT(op_code) \
	{ \
		if(_param(0).size() == 1) \
			_result = *make_deduced_reverse_begin(_param(0)); \
		else { \
			Expression* right = NULL; \
			deduced_reverse_foreach(i, _param(0)) \
				if(is_begin_of_deduced_reverse_foreach(i, _param(0))) \
					right = *i; \
				else \
					BIND_CACHED_LOCATION(right = new BinaryExpr(op_code, *i, right)); \
			_result = right; \
		} \
	}
#endif

#define LEFT_TO_RIGHT_VEC(op_code_vec) \
	{ \
		if(_param(0).size() == 1) \
			_result = *make_deduced_begin(_param(0)); \
		else { \
			Expression* left = NULL; \
			auto j = make_deduced_begin(op_code_vec); \
			deduced_foreach(i, _param(0)) { \
				if(is_begin_of_deduced_foreach(i, _param(0))) { \
					left = *i; \
					continue; \
				} \
				BIND_CACHED_LOCATION(left = new BinaryExpr(*j, left, *i)); \
				j++; \
			} \
			_result = left; \
		} \
	}

#define RIGHT_TO_LEFT_VEC(op_code_vec) \
	{ \
		if(_param(0).size() == 1) \
			_result = *make_deduced_reverse_begin(_param(0)); \
		else { \
			Expression* right = NULL; \
			auto j = make_deduced_reverse_begin(op_code_vec); \
			deduced_reverse_foreach(i, _param(0)) { \
				if(is_begin_of_deduced_reverse_foreach(i, _param(0))) { \
					right = *i; \
					continue; \
				} \
				BIND_CACHED_LOCATION(right = new BinaryExpr(*j, *i, right)); \
				j++; \
			} \
			_result = right; \
		} \
	}

namespace zillians { namespace language { namespace action {

typedef std::vector<BinaryExpr::OpCode::type> binary_ops_t;

struct primary_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("primary_expression param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new PrimaryExpr(_param(0)));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init_bool, bool Value)
	{
#ifdef DEBUG
		printf("primary_expression::init_bool param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		Literal* literal = new NumericLiteral(Value); BIND_CACHED_LOCATION(literal);
		BIND_CACHED_LOCATION(_result = new PrimaryExpr(literal));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init_object_literal, ObjectLiteral::LiteralType::type Type)
	{
#ifdef DEBUG
		printf("primary_expression::init_object_literal param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		Literal* literal = new ObjectLiteral(Type); BIND_CACHED_LOCATION(literal);
		BIND_CACHED_LOCATION(_result = new PrimaryExpr(literal));
	}
	END_ACTION

	BEGIN_ACTION(init_paren_expression)
	{
#ifdef DEBUG
		printf("primary_expression::init_paren_expression param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		_result = _param(0);
	}
	END_ACTION

	BEGIN_ACTION(init_lambda)
	{
#ifdef DEBUG
		printf("primary_expression::lambda_expression param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("primary_expression::lambda_expression param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("primary_expression::lambda_expression param(2) type = %s\n", typeid(_param_t(2)).name());
#endif
		std::vector<VariableDecl*>*            parameters = _param(0).is_initialized() ? &(*_param(0)) : NULL;
		TypeSpecifier*                         type       = _param(1).is_initialized() ? *_param(1) : NULL;
		Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;
		bool                                   is_member  = false;
		bool                                   is_static  = false;
		FunctionDecl* function_decl =
				new FunctionDecl(NULL, type, is_member, is_static, visibility, _param(2)); BIND_CACHED_LOCATION(function_decl);
		if(parameters)
		{
			deduced_foreach_value(i, *parameters)
				function_decl->appendParameter(i);
		}
		BIND_CACHED_LOCATION(_result = new PrimaryExpr(function_decl));
	}
	END_ACTION
};

struct postfix_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_primary_expression)
	{
#ifdef DEBUG
		printf("postfix_expression::init_primary_expression param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		_result = _param(0);
	}
	END_ACTION

	BEGIN_ACTION(append_postfix_array)
	{
#ifdef DEBUG
		printf("postfix_expression::init_postfix_array param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new BinaryExpr(BinaryExpr::OpCode::ARRAY_SUBSCRIPT, _result, _param(0)));
	}
	END_ACTION

	BEGIN_ACTION(append_postfix_call)
	{
#ifdef DEBUG
		printf("postfix_expression::init_postfix_call param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new CallExpr(_result));
		if(_param(0).is_initialized())
		{
			deduced_foreach_value(i, *_param(0))
				cast<CallExpr>(_result)->appendParameter(i);
		}
	}
	END_ACTION

	BEGIN_ACTION(append_postfix_member)
	{
#ifdef DEBUG
		printf("postfix_expression::init_postfix_member param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new MemberExpr(_result, _param(0)));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(append_postfix_step, UnaryExpr::OpCode::type Type)
	{
#ifdef DEBUG
		printf("postfix_expression::append_postfix_step param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new UnaryExpr(Type, _result));
	}
	END_ACTION
};

struct prefix_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("prefix_expression param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		switch(_param(0).which())
		{
		case 0:
			{
				typedef boost::fusion::vector2<boost::optional<UnaryExpr::OpCode::type>, Expression*> fusion_vec_t;
				fusion_vec_t& vec = boost::get<fusion_vec_t>(_param(0));
				boost::optional<UnaryExpr::OpCode::type> optional_type = boost::fusion::at_c<0>(vec);
				_result = boost::fusion::at_c<1>(vec);
				if(optional_type.is_initialized())
					BIND_CACHED_LOCATION(_result = new UnaryExpr(UnaryExpr::OpCode::NEW, _result));
			}
			break;
		case 1:
			{
				typedef boost::fusion::vector2<UnaryExpr::OpCode::type, Expression*> fusion_vec_t;
				fusion_vec_t& vec = boost::get<fusion_vec_t>(_param(0));
				UnaryExpr::OpCode::type type = boost::fusion::at_c<0>(vec);
				Expression*             expr = boost::fusion::at_c<1>(vec);
				BIND_CACHED_LOCATION(_result = new UnaryExpr(type, expr));
			}
			break;
		default:
			_result = NULL;
		}
	}
	END_ACTION
};

struct left_to_right_binary_op_vec
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE, shared_ptr<binary_ops_t>)

	BEGIN_ACTION(append_op)
	{
#ifdef DEBUG
		printf("left_to_right_binary_op_vec::append_op param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		if(!_local(1))
			_local(1).reset(new binary_ops_t());
		_local(1)->push_back(_param(0));
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("left_to_right_binary_op_vec param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		LEFT_TO_RIGHT_VEC(*_local(1));
	}
	END_ACTION
};

struct right_to_left_binary_op_vec
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE, shared_ptr<binary_ops_t>)

	BEGIN_ACTION(append_op)
	{
#ifdef DEBUG
		printf("right_to_left_binary_op_vec::append_op param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		if(!_local(1))
			_local(1).reset(new binary_ops_t());
		_local(1)->push_back(_param(0));
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("right_to_left_binary_op_vec param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		RIGHT_TO_LEFT_VEC(*_local(1));
	}
	END_ACTION
};

struct left_to_right_binary_op
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_TEMPLATED_ACTION(init, BinaryExpr::OpCode::type Type)
	{
#ifdef DEBUG
		printf("left_to_right_binary_op param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		LEFT_TO_RIGHT(Type);
	}
	END_ACTION
};

#if 0 // NOTE: unused
struct right_to_left_binary_op
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_TEMPLATED_ACTION(init, BinaryExpr::OpCode::type Type)
	{
#ifdef DEBUG
		printf("right_to_left_binary_op param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		RIGHT_TO_LEFT(Type);
	}
	END_ACTION
};
#endif

struct range_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("range_expression param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("range_expression param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		if(_param(1).is_initialized())
			BIND_CACHED_LOCATION(_result = new BinaryExpr(BinaryExpr::OpCode::RANGE_ELLIPSIS, _param(0), *_param(1))) // NOTE: omit SEMICOLON
		else
			_result = _param(0);
	}
	END_ACTION
};

struct ternary_expression
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("ternary_expression param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("ternary_expression param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		if(_param(1).is_initialized())
		{
			Expression* true_node  = boost::fusion::at_c<0>(*_param(1));
			Expression* false_node = boost::fusion::at_c<1>(*_param(1));
			BIND_CACHED_LOCATION(_result = new TernaryExpr(_param(0), true_node, false_node));
		}
		else
			_result = _param(0);
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_EXPRESSIONACTIONS_H_ */
