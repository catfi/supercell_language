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

#ifndef ZILLIANS_LANGUAGE_ACTION_SEMANTICACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_SEMANTICACTIONS_H_

#include "core/Prerequisite.h"
#include "utility/Foreach.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/repository/include/qi_iter_pos.hpp>
#include "language/tree/ASTNodeFactory.h"
#include <boost/mpl/bool.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/add_reference.hpp>

// prerequisite
#include "language/action/detail/SemanticActionsDetail.h"

// basic
#include "language/action/basic/BasicActions.h"
#include "language/action/basic/IdentifierActions.h"
#include "language/action/basic/LiteralActions.h"
#include "language/action/basic/SpecifierActions.h"

// non-basic
#include "language/action/nonbasic/DeclarationActions.h"
#include "language/action/nonbasic/ExpressionActions.h"
#include "language/action/nonbasic/ProgramActions.h"
#include "language/action/nonbasic/StatementActions.h"

namespace zillians { namespace language { namespace action {

struct ThorScriptTreeAction
{
	//////////////////////////////////////////////////////////////////////
	/// Semantic Actions for Terminals

	// terminals
	typedef action::identifier      identifier;
	typedef action::integer_literal integer_literal;
	typedef action::float_literal   float_literal;
	typedef action::string_literal  string_literal;

	// basicvariable_decl_list
	typedef action::location                  location;
	typedef action::variable_decl_list        variable_decl_list;
	typedef action::init_specifier            init_specifier;
	typedef action::type_specifier            type_specifier;
	typedef action::thor_type                 thor_type;
	typedef action::template_param_identifier template_param_identifier;
	typedef action::template_arg_identifier   template_arg_identifier;
	typedef action::type_list                 type_list;
	typedef action::class_member_visibility   class_member_visibility;
	typedef action::annotation_list           annotation_list;
	typedef action::annotation                annotation;
	typedef action::annotation_body           annotation_body;
	typedef action::nested_identifier         nested_identifier;

	//////////////////////////////////////////////////////////////////////
	/// Semantic Actions for Non-terminals

	// expression
	typedef action::primary_expression          primary_expression;
	typedef action::postfix_expression          postfix_expression;
	typedef action::prefix_expression           prefix_expression;
	typedef action::left_to_right_binary_op_vec left_to_right_binary_op_vec;
	typedef action::right_to_left_binary_op_vec right_to_left_binary_op_vec;
	typedef action::left_to_right_binary_op     left_to_right_binary_op;
#if 0 // NOTE: unused
	typedef action::right_to_left_binary_op     right_to_left_binary_op;
#endif
	typedef action::range_expression            range_expression;
	typedef action::ternary_expression          ternary_expression;

	// global_decl
	typedef action::global_decl                    global_decl;
	typedef action::variable_decl_stem             variable_decl_stem;
	typedef action::param_decl_with_init           param_decl_with_init;
	typedef action::variable_decl                  variable_decl;
	typedef action::const_decl                     const_decl;
	typedef action::function_decl                  function_decl;
	typedef action::typedef_decl                   typedef_decl;
	typedef action::class_decl                     class_decl;
	typedef action::class_member_decl              class_member_decl;
	typedef action::interface_decl                 interface_decl;
	typedef action::interface_member_function_decl interface_member_function_decl;
	typedef action::enum_decl                      enum_decl;

	// module
	typedef action::program program;

	// statement
	typedef action::statement            statement;
	typedef action::decl_statement       decl_statement;
	typedef action::block                block;
	typedef action::optional_brace_block optional_brace_block;
	typedef action::expression_statement expression_statement;
	typedef action::selection_statement  selection_statement;
	typedef action::iteration_statement  iteration_statement;
	typedef action::branch_statement     branch_statement;

	/**
	 * start is the entry rule for the ThorScript parser
	 */
	struct start
	{
		DEFINE_ATTRIBUTES(void)
		DEFINE_LOCALS(LOCATION_TYPE)

		BEGIN_ACTION(reset)
		{
#ifdef DEBUG
			printf("start::reset param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
			BIND_CACHED_LOCATION(getParserContext().active_source);
			//getParserContext().active_package = getParserContext().active_source->root;
		}
		END_ACTION
	};
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_SEMANTICACTIONS_H_ */
