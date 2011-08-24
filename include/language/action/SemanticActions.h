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
#include "utility/TemplateTricks.h"
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
#include "language/action/basic/IdentifierActions.h"
#include "language/action/basic/LiteralActions.h"
#include "language/action/basic/SpecifierActions.h"

// declaration
#include "language/action/declaration/DeclarationActions.h"

// expression
#include "language/action/expression/ExpressionActions.h"

// module
#include "language/action/module/ImportActions.h"
#include "language/action/module/PackageActions.h"
#include "language/action/module/ProgramActions.h"

// statement
#include "language/action/statement/StatementActions.h"

namespace zillians { namespace language { namespace action {

struct ThorScriptTreeAction
{
	//////////////////////////////////////////////////////////////////////
	/// Semantic Actions for Terminals

	// literal
	typedef action::identifier      identifier;
	typedef action::integer_literal integer_literal;
	typedef action::float_literal   float_literal;
	typedef action::string_literal  string_literal;

	// basic
	typedef action::colon_type_specifier      colon_type_specifier;
	typedef action::type_specifier            type_specifier;
	typedef action::template_arg_identifier   template_arg_identifier;
	typedef action::template_param_identifier template_param_identifier;
	typedef action::template_arg_specifier    template_arg_specifier;
	typedef action::type_list_specifier       type_list_specifier;
	typedef action::storage_specifier         storage_specifier;
	typedef action::visibility_specifier      visibility_specifier;
	typedef action::annotation_specifiers     annotation_specifiers;
	typedef action::annotation_specifier      annotation_specifier;
	typedef action::nested_identifier         nested_identifier;

	//////////////////////////////////////////////////////////////////////
	/// Semantic Actions for Non-terminals

	// expression
	typedef action::primary_expression          primary_expression;
	typedef action::lambda_expression           lambda_expression;
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

	// declaration
	typedef action::declaration    declaration;
	typedef action::variable_decl  variable_decl;
	typedef action::function_decl  function_decl;
	typedef action::typedef_decl   typedef_decl;
	typedef action::class_decl     class_decl;
	typedef action::interface_decl interface_decl;
	typedef action::enum_decl      enum_decl;

	// module
	typedef action::program      program;
	typedef action::package_decl package_decl;
	typedef action::import_decl  import_decl;

	// statement
	typedef action::statement            statement;
	typedef action::compound_statement   compound_statement;
	typedef action::expression_statement expression_statement;
	typedef action::selection_statement  selection_statement;
	typedef action::iteration_statement  iteration_statement;
	typedef action::branch_statement     branch_statement;

	/**
	 * start is the entry rule for the ThorScript parser
	 */
	struct start
	{
		DEFINE_ATTRIBUTES(zillians::language::tree::ASTNode*)
		DEFINE_LOCALS()
	};
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_SEMANTICACTIONS_H_ */
