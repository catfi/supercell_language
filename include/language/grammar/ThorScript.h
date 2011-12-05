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
 * @date Jul 17, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_GRAMMAR_THORSCRIPT_H_
#define ZILLIANS_LANGUAGE_GRAMMAR_THORSCRIPT_H_

#define BOOST_SPIRIT_UNICODE

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/repository/include/qi_distinct.hpp>
#include <boost/spirit/repository/include/qi_iter_pos.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>
#include "utility/UnicodeUtil.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/context/ParserContext.h"
#include "WhiteSpace.h"

#define DISTINCT_IDENTIFIER(x)       distinct(unicode::alnum | L'_')[x]
#define DISTINCT_NO_ASSIGN_FOLLOW(x) distinct(L'=')[x]
#define EMIT_BOOL(x)                 ((x) > qi::attr(true))
#define DECL_RULE_LEXEME(x)          qi::rule<Iterator, typename SA::x::attribute_type, typename SA::x::local_type> x
#define DECL_RULE(x)                 qi::rule<Iterator, typename SA::x::attribute_type, detail::WhiteSpace<Iterator>, typename SA::x::local_type> x
#define DECL_RULE_CUSTOM_SA(x, sa)   qi::rule<Iterator, typename SA::sa::attribute_type, detail::WhiteSpace<Iterator>, typename SA::sa::local_type> x
#define INIT_RULE(x) \
		{ \
			x.name(#x); \
			if(getParserContext().dump_rule_debug) \
				debug(x); \
		}

namespace qi = boost::spirit::qi;
namespace unicode = boost::spirit::unicode;
namespace tree = zillians::language::tree;
using boost::spirit::repository::distinct;
using boost::spirit::ascii::no_case;
using boost::spirit::repository::qi::iter_pos;
using boost::spirit::omit;

namespace zillians { namespace language { namespace grammar {

/////////////////////////////////////////////////////////////////////
/// detail
/////////////////////////////////////////////////////////////////////

namespace detail {

template <typename Iterator, typename SA>
struct Identifier : qi::grammar<Iterator, typename SA::identifier::attribute_type, typename SA::identifier::local_type>
{
	Identifier() : Identifier::base_type(identifier)
	{
		location
			= omit[ iter_pos[ typename SA::location::init() ] ]
			;

		keyword_sym =
			L"void",
			L"int8", L"int16", L"int32", L"int64",
			L"float32", L"float64",
			L"true", L"false", L"null", L"self", L"this", L"super", L"...",
			L"const", L"static",
			L"typedef", L"class", L"interface", L"enum",
			L"public", L"protected", L"private",
			L"var", L"function",
			L"if", L"elif", L"else",
			L"switch", L"case", L"default",
			L"while", L"do", L"foreach", L"in", L"for",
			L"break", L"continue", L"return",
			L"new", L"as", L"instanceof",
			L"module", L"import",
			L"extends", L"implements";
		keyword = DISTINCT_IDENTIFIER(keyword_sym);

		start %= qi::lexeme[ ((unicode::alpha | L'_') > *(unicode::alnum | L'_')) - keyword ];

		identifier
			= location [ typename SA::location::cache_loc() ]
				>> start [ typename SA::identifier::init() ]
			;

		INIT_RULE(identifier);
	}

	DECL_RULE_LEXEME(location);
	qi::symbols<wchar_t const> keyword_sym;
	qi::rule<Iterator, std::wstring()> keyword;
	qi::rule<Iterator, std::wstring()> start;
	DECL_RULE_LEXEME(identifier);
};

template <typename Iterator, typename SA>
struct IntegerLiteral : qi::grammar<Iterator, typename SA::integer_literal::attribute_type, typename SA::integer_literal::local_type>
{
	IntegerLiteral() : IntegerLiteral::base_type(integer_literal)
	{
		location
			= omit[ iter_pos[ typename SA::location::init() ] ]
			;

		start
			%=	distinct(qi::lit(L'.') | L'x' | no_case[L'e'])[qi::uint_]
			|	( qi::lit(L"0x") > qi::hex )
			;

		integer_literal
			= location [ typename SA::location::cache_loc() ]
				>> start [ typename SA::integer_literal::init() ]
			;

		INIT_RULE(integer_literal);
	}

	DECL_RULE_LEXEME(location);
	qi::rule<Iterator, int64()> start;
	DECL_RULE_LEXEME(integer_literal);
};

template <typename Iterator, typename SA>
struct FloatLiteral : qi::grammar<Iterator, typename SA::float_literal::attribute_type, typename SA::float_literal::local_type>
{
	FloatLiteral() : FloatLiteral::base_type(float_literal)
	{
		location
			= omit[ iter_pos[ typename SA::location::init() ] ]
			;

		start
			%=	( builtin_float_parser
				| ( (qi::uint_ | builtin_float_parser) > no_case[L'e'] > -qi::lit(L'-') > qi::uint_ )
				) > -no_case[L'f']
			;

		float_literal
			= location [ typename SA::location::cache_loc() ]
				>> start [ typename SA::float_literal::init() ]
			;

		INIT_RULE(float_literal);
	}

	DECL_RULE_LEXEME(location);
	qi::real_parser<double, qi::strict_ureal_policies<double> > builtin_float_parser;
	qi::rule<Iterator, double()> start;
	DECL_RULE_LEXEME(float_literal);
};

template <typename Iterator, typename SA>
struct StringLiteral : qi::grammar<Iterator, typename SA::string_literal::attribute_type, typename SA::string_literal::local_type>
{
	StringLiteral() : StringLiteral::base_type(string_literal)
	{
		location
			= omit[ iter_pos[ typename SA::location::init() ] ]
			;

		unescaped_char_sym.add
			(L"\\a", L'\a')
			(L"\\b", L'\b')
			(L"\\f", L'\f')
			(L"\\n", L'\n')
			(L"\\r", L'\r')
			(L"\\t", L'\t')
			(L"\\v", L'\v')
			(L"\\\\", L'\\')
			(L"\\\'", L'\'')
			(L"\\\"", L'\"');

		start
			%= qi::lit(L'\"')
				>	*( ( ( unicode::char_ - L'\"' ) - L'\\' )
					| ( unescaped_char_sym )
					| ( L"\\x" > qi::hex )
					)
				>	L'\"'
			;

		string_literal
			= location [ typename SA::location::cache_loc() ]
				>> start [ typename SA::string_literal::init() ]
			;

		INIT_RULE(string_literal);
	}

	DECL_RULE_LEXEME(location);
	qi::symbols<wchar_t const, wchar_t const> unescaped_char_sym;
	qi::rule<Iterator, std::wstring()> start;
	DECL_RULE_LEXEME(string_literal);
};

}

/////////////////////////////////////////////////////////////////////
/// ThorScript
/////////////////////////////////////////////////////////////////////

template<typename Iterator, typename SA>
struct ThorScript : qi::grammar<Iterator, typename SA::start::attribute_type, detail::WhiteSpace<Iterator>, typename SA::start::local_type >
{
	ThorScript() : ThorScript::base_type(start)
	{
		/////////////////////////////////////////////////////////////////////
		/// Operators
		/////////////////////////////////////////////////////////////////////

		// operators & scope
		{
			// miscellaneous
			{
				ELLIPSIS  = qi::lit(L"...");
				DOT       = qi::lit(L'.');
				COLON     = qi::lit(L":");
				SEMICOLON = qi::lit(L';');
				COMMA     = qi::lit(L',');
				AT_SYMBOL = qi::lit(L'@');
				Q_MARK    = qi::lit(L'?');
			}

			// assignments
			{
				ASSIGN        = DISTINCT_NO_ASSIGN_FOLLOW(L'=');
				RSHIFT_ASSIGN = qi::lit(L">>=");
				ARITHMETIC_RSHIFT_ASSIGN = qi::lit(L">>>=");
				LSHIFT_ASSIGN = qi::lit(L"<<=");
				PLUS_ASSIGN   = qi::lit(L"+=");
				MINUS_ASSIGN  = qi::lit(L"-=");
				MUL_ASSIGN    = qi::lit(L"*=");
				DIV_ASSIGN    = qi::lit(L"/=");
				MOD_ASSIGN    = qi::lit(L"%=");
				AND_ASSIGN    = qi::lit(L"&=");
				OR_ASSIGN     = qi::lit(L"|=");
				XOR_ASSIGN    = qi::lit(L"^=");
			}

			// unary operator
			{
				INCREMENT = qi::lit(L"++");
				DECREMENT = qi::lit(L"--");
			}

			// arithmetic operators
			{
				ARITHMETIC_PLUS  = distinct(qi::lit(L'+') | L'=')[qi::lit(L'+')];
				ARITHMETIC_MINUS = distinct(qi::lit(L'-') | L'=')[qi::lit(L'-')];
				ARITHMETIC_MUL   = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L'*'));
				ARITHMETIC_DIV   = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L'/'));
				ARITHMETIC_MOD   = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L'%'));
			}

			// binary operators
			{
				BINARY_AND = distinct(qi::lit(L'&') | L'=')[qi::lit(L'&')];
				BINARY_OR  = distinct(qi::lit(L'|') | L'=')[qi::lit(L'|')];
				BINARY_XOR = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L'^'));
				BINARY_NOT = qi::lit(L'~');
			}

			// shift operators
			{
				RSHIFT = distinct(qi::lit(L'>') | L"=")[L">>"];
				ARITHMETIC_RSHIFT = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L">>>"));
				LSHIFT = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L"<<"));
			}

			// logical operators
			{
				LOGICAL_AND = qi::lit(L"&&");
				LOGICAL_OR  = qi::lit(L"||");
				LOGICAL_NOT = DISTINCT_NO_ASSIGN_FOLLOW(qi::lit(L'!'));
			}

			// comparison
			{
				COMPARE_EQ = qi::lit(L"==");
				COMPARE_NE = qi::lit(L"!=");
				COMPARE_GT = distinct(qi::lit(L'>') | L'=')[qi::lit(L'>')];
				COMPARE_LT = distinct(qi::lit(L'<') | L'=')[qi::lit(L'<')];
				COMPARE_GE = qi::lit(L">=");
				COMPARE_LE = qi::lit(L"<=");
			}

			// scope
			{
				LEFT_BRACE    = qi::lit(L'{');
				RIGHT_BRACE   = qi::lit(L'}');
				LEFT_BRACKET  = qi::lit(L'[');
				RIGHT_BRACKET = qi::lit(L']');
				LEFT_PAREN    = qi::lit(L'(');
				RIGHT_PAREN   = qi::lit(L')');
			}
		}

		/////////////////////////////////////////////////////////////////////
		/// Keywords
		/////////////////////////////////////////////////////////////////////

#define DECL_TOKEN(token, str) \
	token = DISTINCT_IDENTIFIER(qi::lit(str));

		// tokens
		{
			DECL_TOKEN(_TRUE, L"true");
			DECL_TOKEN(_FALSE, L"false");
			DECL_TOKEN(_NULL, L"null");
			DECL_TOKEN(SELF, L"self");
			DECL_TOKEN(THIS, L"this");
			DECL_TOKEN(SUPER, L"super");

			DECL_TOKEN(CONST, L"const");
			DECL_TOKEN(STATIC, L"static");

			DECL_TOKEN(INT8, L"int8");
			DECL_TOKEN(INT16, L"int16");
			DECL_TOKEN(INT32, L"int32");
			DECL_TOKEN(INT64, L"int64");
			DECL_TOKEN(FLOAT32, L"float32");
			DECL_TOKEN(FLOAT64, L"float64");
			DECL_TOKEN(VOID, L"void");

			DECL_TOKEN(TYPEDEF, L"typedef");
			DECL_TOKEN(CLASS, L"class");
			DECL_TOKEN(INTERFACE, L"interface");
			DECL_TOKEN(ENUM, L"enum");

			DECL_TOKEN(PUBLIC, L"public");
			DECL_TOKEN(PROTECTED, L"protected");
			DECL_TOKEN(PRIVATE, L"private");

			DECL_TOKEN(VAR, L"var");
			DECL_TOKEN(FUNCTION, L"function");

			DECL_TOKEN(IF, L"if");
			DECL_TOKEN(ELIF, L"elif");
			DECL_TOKEN(ELSE, L"else");

			DECL_TOKEN(SWITCH, L"switch");
			DECL_TOKEN(CASE, L"case");
			DECL_TOKEN(DEFAULT, L"default");

			DECL_TOKEN(WHILE, L"while");
			DECL_TOKEN(DO, L"do");
			DECL_TOKEN(FOREACH, L"foreach");
			DECL_TOKEN(IN, L"in");
			DECL_TOKEN(FOR, L"for");

			DECL_TOKEN(RETURN, L"return");
			DECL_TOKEN(BREAK, L"break");
			DECL_TOKEN(CONTINUE, L"continue");

			DECL_TOKEN(NEW, L"new");
			DECL_TOKEN(AS, L"as");
			DECL_TOKEN(INSTANCEOF, L"instanceof");

			DECL_TOKEN(MODULE, L"module");
			DECL_TOKEN(IMPORT, L"import");

			DECL_TOKEN(EXTENDS, L"extends");
			DECL_TOKEN(IMPLEMENTS, L"implements");
		}

		/////////////////////////////////////////////////////////////////////
		/// Rule Define
		/////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		/// BEGIN BASIC
		///

		location
			= omit[ iter_pos[ typename SA::location::init() ] ]
			;

		param_decl_list
			%= (variable_decl_stem % COMMA)
			;

		param_decl_with_init_list
			%= (param_decl_with_init % COMMA)
			;

		init_specifier
			%= ASSIGN > expression
			;

		type_specifier
			%= COLON > thor_type
			;

		thor_type
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	( DISTINCT_IDENTIFIER(qi::lit(L"void"))                                [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::VOID>() ]
					| qi::lit(L"int8")                                                     [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT8>() ]
					| qi::lit(L"int16")                                                    [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT16>() ]
					| qi::lit(L"int32")                                                    [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT32>() ]
					| qi::lit(L"int64")                                                    [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT64>() ]
					| qi::lit(L"float32")                                                  [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::FLOAT32>() ]
					| qi::lit(L"float64")                                                  [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::FLOAT64>() ]
					| (nested_identifier > -type_specialize_specifier)                     [ typename SA::thor_type::init_type() ]
					| (FUNCTION > LEFT_PAREN > -type_list > RIGHT_PAREN > -type_specifier) [ typename SA::thor_type::init_function_type() ]
					)
			;

		thor_param_type
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	( DISTINCT_IDENTIFIER(qi::lit(L"void"))                                [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::VOID>() ]
					| qi::lit(L"int8")                                                     [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT8>() ]
					| qi::lit(L"int16")                                                    [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT16>() ]
					| qi::lit(L"int32")                                                    [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT32>() ]
					| qi::lit(L"int64")                                                    [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::INT64>() ]
					| qi::lit(L"float32")                                                  [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::FLOAT32>() ]
					| qi::lit(L"float64")                                                  [ typename SA::thor_type::template init_primitive_type<tree::PrimitiveType::FLOAT64>() ]
					| template_param_identifier                                            [ typename SA::thor_param_type::init_type() ]
					| (FUNCTION > LEFT_PAREN > -type_list > RIGHT_PAREN > -type_specifier) [ typename SA::thor_type::init_function_type() ]
					)
			;

		template_param_identifier
			= qi::eps [ typename SA::location::cache_loc() ]
			    >>	(IDENTIFIER > -(COMPARE_LT >> ((IDENTIFIER > -((COLON > thor_param_type) | init_specifier)) % COMMA) >> COMPARE_GT)
					) [ typename SA::template_param_identifier::init() ]
			;

		template_arg_identifier
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (IDENTIFIER > -type_specialize_specifier) [ typename SA::template_arg_identifier::init() ]
			;

		type_specialize_specifier
			%= (COMPARE_LT >> type_list >> COMPARE_GT)
			;

		type_list
			%= (thor_type % COMMA)
			;

		class_member_visibility
			= PUBLIC    [ typename SA::class_member_visibility::template init<tree::Declaration::VisibilitySpecifier::PUBLIC>() ]
			| PROTECTED [ typename SA::class_member_visibility::template init<tree::Declaration::VisibilitySpecifier::PROTECTED>() ]
			| PRIVATE   [ typename SA::class_member_visibility::template init<tree::Declaration::VisibilitySpecifier::PRIVATE>() ]
			;

		interface_member_visibility
			= PUBLIC    [ typename SA::class_member_visibility::template init<tree::Declaration::VisibilitySpecifier::PUBLIC>() ]
			| PROTECTED [ typename SA::class_member_visibility::template init<tree::Declaration::VisibilitySpecifier::PROTECTED>() ]
			;

		annotation_list
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (+annotation) [ typename SA::annotation_list::init() ]
			;

		annotation
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (AT_SYMBOL > IDENTIFIER > -annotation_body) [ typename SA::annotation::init() ]
			;

		annotation_body
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(LEFT_BRACE
						> ((IDENTIFIER > ASSIGN > (primary_expression | annotation_body)) % COMMA)
						> RIGHT_BRACE
					) [ typename SA::annotation_body::init() ]
			;

		nested_identifier
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (IDENTIFIER > *(DOT > IDENTIFIER)) [ typename SA::nested_identifier::init() ]
			;

		///
		/// END BASIC
		/////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		/// BEGIN EXPRESSION
		///
		/// 0.  POSTFIX_STEP
		/// 1.  UNARY_SIGN, PREFIX_STEP
		/// 2.  MULTIPLICATIVE
		/// 3.  ADDITIVE
		/// 4.  BITWISE_SHIFT
		/// 5.  RELATIONAL
		/// 6.  EQUALITY
		/// 7.  BITWISE_AND
		/// 8.  BITWISE_XOR
		/// 9.  BITWISE_OR
		/// 10. LOGICAL_AND
		/// 11. LOGICAL_OR
		/// 12. RANGE
		/// 13. TERNARY
		/// 14. ASSIGNMENT, MODIFY_ASSIGN
		/// 15. COMMA
		///

		primary_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	( template_arg_identifier                 [ typename SA::primary_expression::init() ]
					| INTEGER_LITERAL                         [ typename SA::primary_expression::init() ]
					| FLOAT_LITERAL                           [ typename SA::primary_expression::init() ]
					| STRING_LITERAL                          [ typename SA::primary_expression::init() ]
					| _TRUE                                   [ typename SA::primary_expression::template init_bool<true>() ]
					| _FALSE                                  [ typename SA::primary_expression::template init_bool<false>() ]
					| _NULL                                   [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::NULL_OBJECT>() ]
					| SELF                                    [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::SELF_OBJECT>() ]
					| THIS                                    [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::THIS_OBJECT>() ]
					| SUPER                                   [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::SUPER_OBJECT>() ]
					| (LEFT_PAREN > expression > RIGHT_PAREN) [ typename SA::primary_expression::init_paren_expression() ]
					|	(FUNCTION > LEFT_PAREN > -param_decl_list > RIGHT_PAREN > -type_specifier > block
						) [ typename SA::primary_expression::init_lambda() ]
					)
			;

		// postfix expression
		// associativity: left-to-right
		// rank: 0
		postfix_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(primary_expression                                          [ typename SA::postfix_expression::init_primary_expression() ]
						>	*( (LEFT_BRACKET > expression > RIGHT_BRACKET)       [ typename SA::postfix_expression::append_postfix_array() ]
							| (LEFT_PAREN > -(expression % COMMA) > RIGHT_PAREN) [ typename SA::postfix_expression::append_postfix_call() ]
							| (DOT >> template_arg_identifier)                   [ typename SA::postfix_expression::append_postfix_member() ]
							| INCREMENT                                          [ typename SA::postfix_expression::template append_postfix_step<tree::UnaryExpr::OpCode::POSTFIX_INCREMENT>() ]
							| DECREMENT                                          [ typename SA::postfix_expression::template append_postfix_step<tree::UnaryExpr::OpCode::POSTFIX_DECREMENT>() ]
							)
					)
			;

		// prefix expression
		// associativity: right-to-left
		// rank: 1
		prefix_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(	( -( NEW > qi::attr(tree::UnaryExpr::OpCode::NEW) ) >> postfix_expression )
					|	(	( ( INCREMENT        > qi::attr(tree::UnaryExpr::OpCode::PREFIX_INCREMENT) )
							| ( DECREMENT        > qi::attr(tree::UnaryExpr::OpCode::PREFIX_DECREMENT) )
							| ( BINARY_NOT       > qi::attr(tree::UnaryExpr::OpCode::BINARY_NOT) )
							| ( LOGICAL_NOT      > qi::attr(tree::UnaryExpr::OpCode::LOGICAL_NOT) )
							| ( ARITHMETIC_MINUS > qi::attr(tree::UnaryExpr::OpCode::ARITHMETIC_NEGATE) )
							) > prefix_expression
						)
					) [ typename SA::prefix_expression::init() ]
			;

		// multiplicative expression
		// associativity: left-to-right
		// rank: 2
		multiplicative_expression
			= qi::eps [ typename SA::location::cache_loc() ]
			 	>>	(prefix_expression
					%	( ( ARITHMETIC_MUL > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_MUL) )
						| ( ARITHMETIC_DIV > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_DIV) )
						| ( ARITHMETIC_MOD > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_MOD) )
						) [ typename SA::left_to_right_binary_op_vec::append_op() ]
					) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// additive expression
		// associativity: left-to-right
		// rank: 3
		additive_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(multiplicative_expression
					%	( ( ARITHMETIC_PLUS  > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_ADD) )
						| ( ARITHMETIC_MINUS > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_SUB) )
						) [ typename SA::left_to_right_binary_op_vec::append_op() ]
					) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// shift expression
		// associativity: left-to-right
		// rank: 4
		shift_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(additive_expression
					%	( ( RSHIFT            > qi::attr(tree::BinaryExpr::OpCode::BINARY_RSHIFT) )
						| ( ARITHMETIC_RSHIFT > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_RSHIFT) )
						| ( LSHIFT            > qi::attr(tree::BinaryExpr::OpCode::BINARY_LSHIFT) )
						) [ typename SA::left_to_right_binary_op_vec::append_op() ]
					) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// rational expression
		// associativity: left-to-right
		// rank: 5
		relational_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(shift_expression
					%	( ( COMPARE_GT > qi::attr(tree::BinaryExpr::OpCode::COMPARE_GT) )
						| ( COMPARE_LT > qi::attr(tree::BinaryExpr::OpCode::COMPARE_LT) )
						| ( COMPARE_GE > qi::attr(tree::BinaryExpr::OpCode::COMPARE_GE) )
						| ( COMPARE_LE > qi::attr(tree::BinaryExpr::OpCode::COMPARE_LE) )
						| ( INSTANCEOF > qi::attr(tree::BinaryExpr::OpCode::INSTANCEOF) )
						) [ typename SA::left_to_right_binary_op_vec::append_op() ]
					) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// equality expression
		// associativity: left-to-right
		// rank: 6
		equality_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(relational_expression
					%	( ( COMPARE_EQ > qi::attr(tree::BinaryExpr::OpCode::COMPARE_EQ) )
						| ( COMPARE_NE > qi::attr(tree::BinaryExpr::OpCode::COMPARE_NE) )
						) [ typename SA::left_to_right_binary_op_vec::append_op() ]
					) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// and expression
		// associativity: left-to-right
		// rank: 7
		and_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(equality_expression % BINARY_AND
					) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::BINARY_AND>() ]
			;

		// xor expression
		// associativity: left-to-right
		// rank: 8
		xor_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(and_expression % BINARY_XOR
					) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::BINARY_XOR>() ]
			;

		// or expression
		// associativity: left-to-right
		// rank: 9
		or_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(xor_expression % BINARY_OR
					) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::BINARY_OR>() ]
			;

		// logical and expression
		// associativity: left-to-right
		// rank: 10
		logical_and_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(or_expression % LOGICAL_AND
					) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::LOGICAL_AND>() ]
			;

		// logical or expression
		// associativity: left-to-right
		// rank: 11
		logical_or_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(logical_and_expression % LOGICAL_OR
					) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::LOGICAL_OR>() ]
			;

		// range expression
		// associativity: left-to-right
		// rank: 12
		range_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(logical_or_expression > -(ELLIPSIS > logical_or_expression)
					) [ typename SA::range_expression::init() ]
			;

		// ternary expression
		// associativity: right-to-left
		// rank: 13
		ternary_expression
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(range_expression > -(Q_MARK > range_expression > COLON > range_expression)
					) [ typename SA::ternary_expression::init() ]
			;

		// assignment_expression
		// associativity: right-to-left
		// rank: 14
		expression
			= location [ typename SA::location::cache_loc() ]
				>>	(ternary_expression
					%	( ( ASSIGN                   > qi::attr(tree::BinaryExpr::OpCode::ASSIGN) )
						| ( RSHIFT_ASSIGN            > qi::attr(tree::BinaryExpr::OpCode::RSHIFT_ASSIGN) )
						| ( ARITHMETIC_RSHIFT_ASSIGN > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_RSHIFT_ASSIGN) )
						| ( LSHIFT_ASSIGN            > qi::attr(tree::BinaryExpr::OpCode::LSHIFT_ASSIGN) )
						| ( PLUS_ASSIGN              > qi::attr(tree::BinaryExpr::OpCode::ADD_ASSIGN) )
						| ( MINUS_ASSIGN             > qi::attr(tree::BinaryExpr::OpCode::SUB_ASSIGN) )
						| ( MUL_ASSIGN               > qi::attr(tree::BinaryExpr::OpCode::MUL_ASSIGN) )
						| ( DIV_ASSIGN               > qi::attr(tree::BinaryExpr::OpCode::DIV_ASSIGN) )
						| ( MOD_ASSIGN               > qi::attr(tree::BinaryExpr::OpCode::MOD_ASSIGN) )
						| ( AND_ASSIGN               > qi::attr(tree::BinaryExpr::OpCode::AND_ASSIGN) )
						| ( OR_ASSIGN                > qi::attr(tree::BinaryExpr::OpCode::OR_ASSIGN) )
						| ( XOR_ASSIGN               > qi::attr(tree::BinaryExpr::OpCode::XOR_ASSIGN) )
						) [ typename SA::right_to_left_binary_op_vec::append_op() ]
					) [ typename SA::right_to_left_binary_op_vec::init() ]
			;

		///
		/// END EXPRESSION
		/////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		/// BEGIN STATEMENT
		///

		statement
			=	(	(-annotation_list >> location [ typename SA::location::cache_loc() ]
						>>	( decl_statement
							| expression_statement
							| selection_statement
							| iteration_statement
							| branch_statement
							)
					) [ typename SA::statement::init() ]
				|	(location [ typename SA::location::cache_loc() ]
						>> block
					) [ typename SA::statement::init_block() ]
				)
			;

		decl_statement
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (-EMIT_BOOL(STATIC) >> (variable_decl | const_decl)) [ typename SA::decl_statement::init() ]
			;

		expression_statement
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (-expression >> SEMICOLON) [ typename SA::expression_statement::init() ]
			;

		selection_statement
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(	(IF > LEFT_PAREN > expression > RIGHT_PAREN > optional_brace_block
							> *(ELIF > LEFT_PAREN > expression > RIGHT_PAREN > optional_brace_block)
							> -(ELSE > optional_brace_block)
						) [ typename SA::selection_statement::init_if_statement() ]
					|	(SWITCH > LEFT_PAREN > expression > RIGHT_PAREN
							> LEFT_BRACE
							>	*( ( CASE > expression > COLON > braceless_block )
								 | ( DEFAULT > COLON > braceless_block )
								 )
							> RIGHT_BRACE
						) [ typename SA::selection_statement::init_switch_statement() ]
					)
			;

		iteration_statement
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(	(WHILE > LEFT_PAREN > expression > RIGHT_PAREN > -optional_brace_block
						) [ typename SA::iteration_statement::init_while_loop() ]
					|	(DO > optional_brace_block > WHILE > LEFT_PAREN > expression > RIGHT_PAREN > SEMICOLON
						) [ typename SA::iteration_statement::init_do_while_loop() ]
					|	(FOREACH > LEFT_PAREN > ((VAR > variable_decl_stem) | postfix_expression) > IN > expression > RIGHT_PAREN > -optional_brace_block
						) [ typename SA::iteration_statement::init_foreach() ]
					|	(FOR > LEFT_PAREN > (variable_decl | (expression > SEMICOLON)) > expression > SEMICOLON > expression > RIGHT_PAREN > -optional_brace_block
						) [ typename SA::iteration_statement::init_for() ]
					)
			;

		branch_statement
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	( (RETURN > -expression > SEMICOLON) [ typename SA::branch_statement::init_return() ]
					| (BREAK > SEMICOLON)                [ typename SA::branch_statement::template init<tree::BranchStmt::OpCode::BREAK>() ]
					| (CONTINUE > SEMICOLON)             [ typename SA::branch_statement::template init<tree::BranchStmt::OpCode::CONTINUE>() ]
					)
			;

		block
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(LEFT_BRACE
						> *statement
						> RIGHT_BRACE
					) [ typename SA::block::init() ]
			;

		optional_brace_block
			= qi::eps [ typename SA::location::cache_loc() ]
				>> statement [ typename SA::optional_brace_block::init() ]
			;

		braceless_block
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (*statement) [ typename SA::block::init() ]
			;

		///
		/// END STATEMENT
		/////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		/// BEGIN DECLARATION
		///

		global_decl
			=	(-annotation_list >> location //[ typename SA::location::cache_loc() ]
					>>	( variable_decl
						| const_decl
						| function_decl
						| typedef_decl
						| class_decl
						| interface_decl
						| enum_decl
						)
				) [ typename SA::global_decl::init() ]
			;

		variable_decl_stem
			= location [ typename SA::location::cache_loc() ]
				>> (IDENTIFIER > -type_specifier) [ typename SA::variable_decl_stem::init() ]
			;

		param_decl_with_init
			= (variable_decl_stem > -init_specifier) [ typename SA::param_decl_with_init::init() ]
			;

		variable_decl
			%= (VAR > param_decl_with_init > SEMICOLON)
			;

		const_decl
			= (CONST > variable_decl_stem > init_specifier > SEMICOLON) [ typename SA::const_decl::init() ]
			;

		function_decl
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(FUNCTION > (template_param_identifier | EMIT_BOOL(NEW))
						> LEFT_PAREN > -param_decl_with_init_list > RIGHT_PAREN > -type_specifier
						> (block | SEMICOLON)
					) [ typename SA::function_decl::init() ]
			;

		typedef_decl
			= qi::eps [ typename SA::location::cache_loc() ]
				>> (TYPEDEF > thor_type > IDENTIFIER > SEMICOLON) [ typename SA::typedef_decl::init() ]
			;

		class_decl
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(CLASS > template_param_identifier
						> -(EXTENDS > nested_identifier) > -((IMPLEMENTS > nested_identifier) % COMMA)
							> ((LEFT_BRACE > *class_member_decl > RIGHT_BRACE) | SEMICOLON)
					) [ typename SA::class_decl::init() ]
			;

		class_member_decl
			=	(-annotation_list >> location //[ typename SA::location::cache_loc() ]
					>> -class_member_visibility >> -EMIT_BOOL(STATIC)
					>>	( variable_decl
						| const_decl
						| function_decl
						)
				) [ typename SA::class_member_decl::init() ]
			;

		interface_decl
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(INTERFACE > IDENTIFIER
						> LEFT_BRACE
						> *interface_member_function_decl
						> RIGHT_BRACE
					) [ typename SA::interface_decl::init() ]
			;

		interface_member_function_decl
			=	(-annotation_list >> location [ typename SA::location::cache_loc() ]
			 		>> -interface_member_visibility >> FUNCTION >> IDENTIFIER
					>> LEFT_PAREN >> -param_decl_list >> RIGHT_PAREN >> type_specifier > SEMICOLON
				) [ typename SA::interface_member_function_decl::init() ]
			;

		enum_decl
			= qi::eps [ typename SA::location::cache_loc() ]
				>>	(ENUM > IDENTIFIER
						> LEFT_BRACE
						> *(IDENTIFIER > -init_specifier > SEMICOLON)
						> RIGHT_BRACE
					) [ typename SA::enum_decl::init() ]
			;

		///
		/// END DECLARATION
		/////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		/// BEGIN MODULE
		///

		program
			=	*( ( (IMPORT >> IDENTIFIER >> ASSIGN > nested_identifier > SEMICOLON) [ typename SA::program::append_import_alias() ] ) |
				   ( (IMPORT >> DOT > ASSIGN > nested_identifier > SEMICOLON) [ typename SA::program::append_import_global_alias() ] ) |
				   ( (IMPORT >> nested_identifier > SEMICOLON) [ typename SA::program::append_import() ] ) )
				> *( global_decl                              [ typename SA::program::append_global_decl() ] )
			;

		///
		/// END MODULE
		/////////////////////////////////////////////////////////////////////

		start
			= location [ typename SA::start::reset() ]
				>> (program > qi::eoi)
			;

		/////////////////////////////////////////////////////////////////////
		/// Debugging Names / Handlers
		/////////////////////////////////////////////////////////////////////

		// keywords
		_TRUE.name("TRUE_LITERAL");
		_FALSE.name("FALSE_LITERAL");
		_NULL.name("NULL_LITERAL");

		// terminals
		IDENTIFIER.name("IDENTIFIER.grammar");
		INTEGER_LITERAL.name("INTEGER_LITERAL.grammar");
		FLOAT_LITERAL.name("FLOAT_LITERAL.grammar");
		STRING_LITERAL.name("STRING_LITERAL.grammar");

		if(getParserContext().dump_rule_debug)
		{
			// keywords
			debug(_TRUE);
			debug(_FALSE);
			debug(_NULL);

			// terminals
#if 0 // NOTE: grammars cannot have debug_handlers, only rules can
			debug(IDENTIFIER);
			debug(INTEGER_LITERAL);
			debug(FLOAT_LITERAL);
			debug(STRING_LITERAL);
#endif
		}

		// basic
		INIT_RULE(location);
		INIT_RULE(param_decl_list);
		INIT_RULE(param_decl_with_init_list);
		INIT_RULE(init_specifier);
		INIT_RULE(type_specifier);
		INIT_RULE(thor_type);
		INIT_RULE(template_param_identifier);
		INIT_RULE(template_arg_identifier);
		INIT_RULE(type_specialize_specifier);
		INIT_RULE(type_list);
		INIT_RULE(class_member_visibility);
		INIT_RULE(interface_member_visibility);
		INIT_RULE(annotation_list);
		INIT_RULE(annotation);
		INIT_RULE(annotation_body);
		INIT_RULE(nested_identifier);

		// expression
		INIT_RULE(primary_expression);
		INIT_RULE(postfix_expression);
		INIT_RULE(prefix_expression);
		INIT_RULE(multiplicative_expression);
		INIT_RULE(additive_expression);
		INIT_RULE(shift_expression);
		INIT_RULE(relational_expression);
		INIT_RULE(equality_expression);
		INIT_RULE(and_expression);
		INIT_RULE(xor_expression);
		INIT_RULE(or_expression);
		INIT_RULE(logical_and_expression);
		INIT_RULE(logical_or_expression);
		INIT_RULE(range_expression);
		INIT_RULE(ternary_expression);
		INIT_RULE(expression);

		// statement
		INIT_RULE(statement);
		INIT_RULE(decl_statement);
		INIT_RULE(expression_statement);
		INIT_RULE(selection_statement);
		INIT_RULE(iteration_statement);
		INIT_RULE(branch_statement);
		INIT_RULE(block);
		INIT_RULE(optional_brace_block);
		INIT_RULE(braceless_block);

		// global_decl
		INIT_RULE(global_decl);
		INIT_RULE(variable_decl_stem);
		INIT_RULE(param_decl_with_init);
		INIT_RULE(variable_decl);
		INIT_RULE(const_decl);
		INIT_RULE(function_decl);
		INIT_RULE(typedef_decl);
		INIT_RULE(class_decl);
		INIT_RULE(class_member_decl);
		INIT_RULE(interface_decl);
		INIT_RULE(interface_member_function_decl);
		INIT_RULE(enum_decl);

		// module
		INIT_RULE(program);

		// start
		INIT_RULE(start);
	}

	/////////////////////////////////////////////////////////////////////
	/// Rule Declare
	/////////////////////////////////////////////////////////////////////

	// operators
	qi::rule<Iterator, detail::WhiteSpace<Iterator> >
		ELLIPSIS, DOT, COLON, SEMICOLON, COMMA, AT_SYMBOL, Q_MARK,
		ASSIGN, RSHIFT_ASSIGN, ARITHMETIC_RSHIFT_ASSIGN, LSHIFT_ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN,
		INCREMENT, DECREMENT,
		ARITHMETIC_PLUS, ARITHMETIC_MINUS, ARITHMETIC_MUL, ARITHMETIC_DIV, ARITHMETIC_MOD,
		BINARY_AND, BINARY_OR, BINARY_XOR, BINARY_NOT,
		RSHIFT, ARITHMETIC_RSHIFT, LSHIFT,
		LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
		COMPARE_EQ, COMPARE_NE, COMPARE_GT, COMPARE_LT, COMPARE_GE, COMPARE_LE,
		LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET, LEFT_PAREN, RIGHT_PAREN;

	// keywords
	qi::rule<Iterator, detail::WhiteSpace<Iterator> >
		_TRUE, _FALSE, _NULL, SELF, THIS, SUPER,
		CONST, STATIC,
		INT8, INT16, INT32, INT64, FLOAT32, FLOAT64, VOID,
		TYPEDEF, CLASS, INTERFACE, ENUM,
		PUBLIC, PROTECTED, PRIVATE,
		VAR, FUNCTION,
		IF, ELIF, ELSE,
		SWITCH, CASE, DEFAULT,
		WHILE, DO, FOREACH, IN, FOR,
		RETURN, BREAK, CONTINUE,
		NEW, AS, INSTANCEOF,
		MODULE, IMPORT,
		EXTENDS, IMPLEMENTS;

	// terminals
	detail::Identifier<Iterator, SA>     IDENTIFIER;
	detail::IntegerLiteral<Iterator, SA> INTEGER_LITERAL;
	detail::FloatLiteral<Iterator, SA>   FLOAT_LITERAL;
	detail::StringLiteral<Iterator, SA>  STRING_LITERAL;

	// location
	DECL_RULE_LEXEME(location);

	// basic
	DECL_RULE_CUSTOM_SA( param_decl_list, variable_decl_list);
	DECL_RULE_CUSTOM_SA( param_decl_with_init_list, variable_decl_list);
	DECL_RULE(           init_specifier);
	DECL_RULE(           type_specifier);
	DECL_RULE(           thor_type);
	DECL_RULE(           thor_param_type);
	DECL_RULE(           template_param_identifier);
	DECL_RULE(           template_arg_identifier);
	DECL_RULE_CUSTOM_SA( type_specialize_specifier, type_list);
	DECL_RULE(           type_list);
	DECL_RULE(           class_member_visibility);
	DECL_RULE_CUSTOM_SA( interface_member_visibility, class_member_visibility );
	DECL_RULE(           annotation_list);
	DECL_RULE(           annotation);
	DECL_RULE(           annotation_body);
	DECL_RULE(           nested_identifier);

	// expression
	DECL_RULE(           primary_expression);
	DECL_RULE(           postfix_expression);
	DECL_RULE(           prefix_expression);
	DECL_RULE_CUSTOM_SA( multiplicative_expression, left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( additive_expression,       left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( shift_expression,          left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( relational_expression,     left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( equality_expression,       left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( and_expression,            left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( xor_expression,            left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( or_expression,             left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( logical_and_expression,    left_to_right_binary_op_vec );
	DECL_RULE_CUSTOM_SA( logical_or_expression,     left_to_right_binary_op_vec );
	DECL_RULE(           range_expression);
	DECL_RULE(           ternary_expression);
	DECL_RULE_CUSTOM_SA( expression,                right_to_left_binary_op_vec );

	// statement
	DECL_RULE(statement);
	DECL_RULE(decl_statement);
	DECL_RULE(expression_statement);
	DECL_RULE(selection_statement);
	DECL_RULE(iteration_statement);
	DECL_RULE(branch_statement);
	DECL_RULE(block);
	DECL_RULE(optional_brace_block);
	DECL_RULE_CUSTOM_SA(braceless_block, block);

	// global_decl
	DECL_RULE(global_decl);
	DECL_RULE(variable_decl_stem);
	DECL_RULE(param_decl_with_init);
	DECL_RULE(variable_decl);
	DECL_RULE(const_decl);
	DECL_RULE(function_decl);
	DECL_RULE(typedef_decl);
	DECL_RULE(class_decl);
	DECL_RULE(class_member_decl);
	DECL_RULE(interface_decl);
	DECL_RULE(interface_member_function_decl);
	DECL_RULE(enum_decl);

	// module
	DECL_RULE(program);

	// start
	DECL_RULE(start);
};

} } }

#endif /* ZILLIANS_LANGUAGE_GRAMMAR_THORSCRIPT_H_ */
