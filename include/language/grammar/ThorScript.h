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

#define DISTINCT_IDENTIFIER(x)   distinct(unicode::alnum | L'_')[x]
#define DISTINCT_NONASSIGN_OP(x) distinct(L'=')[x]

namespace qi = boost::spirit::qi;
namespace unicode = boost::spirit::unicode;
namespace tree = zillians::language::tree;
using boost::spirit::repository::distinct;
using boost::spirit::ascii::no_case;
using boost::spirit::repository::qi::iter_pos;
using boost::spirit::omit;

namespace zillians { namespace language { namespace grammar {

///////////////////////////////////////////////////////////////////////////////////////////////////
// detail
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {

template <typename Iterator>
struct WhiteSpace : qi::grammar<Iterator>
{
	WhiteSpace() : WhiteSpace::base_type(start)
	{
		comment_c_style = qi::lexeme[L"/*" > *(unicode::char_ - L"*/") > L"*/"];
		comment_c_style.name("comment_in_c_style");

		comment_cpp_style = qi::lexeme[L"//" > *(unicode::char_ - qi::eol) > qi::eol];
		comment_cpp_style.name("comment_in_cpp_style");

		start
			= unicode::space    // tab/space/cr/lf
			| comment_c_style   // c-style comment "/* */"
			| comment_cpp_style // cpp-style comment "//"
			;

		start.name("WHITESPACE");
	}

	qi::rule<Iterator> start;
	qi::rule<Iterator> comment_c_style;
	qi::rule<Iterator> comment_cpp_style;
};

template <typename Iterator, typename SA>
struct Identifier : qi::grammar<Iterator, typename SA::identifier::attribute_type, typename SA::identifier::local_type>
{
	Identifier() : Identifier::base_type(start_augmented)
	{
		keyword_sym =
			L"void",
			L"int8", L"uint8", L"int16", L"uint16", L"int32", L"uint32", L"int64", L"uint64",
			L"float32", L"float64",
			L"true", L"false", L"null", L"self", L"global", L"...",
			L"const", L"static",
			L"typedef", L"class", L"interface", L"enum",
			L"public", L"protected", L"private",
			L"var", L"function",
			L"if", L"elif", L"else",
			L"switch", L"case", L"default",
			L"foreach", L"in", L"do", L"while",
			L"break", L"continue", L"return",
			L"new", L"as", L"instanceof",
			L"package", L"import",
			L"extends", L"implements";
		keyword = DISTINCT_IDENTIFIER(keyword_sym);

		start %= qi::lexeme[ ((unicode::alpha | L'_') > *(unicode::alnum | L'_')) - keyword ];

		start_augmented
			=	(start
					> omit[ iter_pos[ typename SA::identifier::init_iter_pos() ] ]
				) [ typename SA::identifier::init() ];

		start_augmented.name("IDENTIFIER");
		if(getParserContext().enable_debug_parser)
			debug(start_augmented);
	}

	qi::symbols<wchar_t const> keyword_sym;
	qi::rule<Iterator, std::wstring()> keyword, start;
	qi::rule<Iterator, typename SA::identifier::attribute_type, typename SA::identifier::local_type> start_augmented;
};

template <typename Iterator, typename SA>
struct IntegerLiteral : qi::grammar<Iterator, typename SA::integer_literal::attribute_type, typename SA::integer_literal::local_type>
{
	IntegerLiteral() : IntegerLiteral::base_type(start_augmented)
	{
		start
			%=	distinct(qi::lit(L'.') | L'x' | no_case[L'e'])[qi::uint_]
			|	qi::lit(L"0x") > qi::hex
			;

		start_augmented = start [ typename SA::integer_literal::init() ];

		start_augmented.name("INTEGER_LITERAL");
		if(getParserContext().enable_debug_parser)
			debug(start_augmented);
	}

	qi::rule<Iterator, uint64()> start;
	qi::rule<Iterator, typename SA::integer_literal::attribute_type, typename SA::integer_literal::local_type> start_augmented;
};

template <typename Iterator, typename SA>
struct FloatLiteral : qi::grammar<Iterator, typename SA::float_literal::attribute_type, typename SA::float_literal::local_type>
{
	FloatLiteral() : FloatLiteral::base_type(start_augmented)
	{
		start
			%=	( builtin_float_parser
				| (qi::uint_ | builtin_float_parser) > no_case[L'e'] > -qi::lit(L'-') > qi::uint_
				) > -no_case[L'f']
			;

		start_augmented = start [ typename SA::float_literal::init() ];

		start_augmented.name("FLOAT_LITERAL");
		if(getParserContext().enable_debug_parser)
			debug(start_augmented);
	}

	qi::real_parser<double, qi::strict_ureal_policies<double> > builtin_float_parser;
	qi::rule<Iterator, double()> start;
	qi::rule<Iterator, typename SA::float_literal::attribute_type, typename SA::float_literal::local_type> start_augmented;
};

template <typename Iterator, typename SA>
struct StringLiteral : qi::grammar<Iterator, typename SA::string_literal::attribute_type, typename SA::string_literal::local_type>
{
	StringLiteral() : StringLiteral::base_type(start_augmented)
	{
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
					| unescaped_char_sym
					| L"\\x" > qi::hex
					)
				>	L'\"'
			;

		start_augmented = start [ typename SA::string_literal::init() ];

		start_augmented.name("STRING_LITERAL");
		if(getParserContext().enable_debug_parser)
			debug(start_augmented);
	}

	qi::symbols<wchar_t const, wchar_t const> unescaped_char_sym;
	qi::rule<Iterator, std::wstring()> start;
	qi::rule<Iterator, typename SA::string_literal::attribute_type, typename SA::string_literal::local_type> start_augmented;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ThorScript
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Iterator, typename SA>
struct ThorScript : qi::grammar<Iterator, typename SA::start::attribute_type, detail::WhiteSpace<Iterator>, typename SA::start::local_type >
{
	ThorScript() : ThorScript::base_type(start)
	{
		///////////////////////////////////////////////////////////////////////////////
		// Operators
		///////////////////////////////////////////////////////////////////////////////

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
				ASSIGN        = DISTINCT_NONASSIGN_OP(L'=');
				RSHIFT_ASSIGN = qi::lit(L">>=");
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
				ARITHMETIC_MUL   = DISTINCT_NONASSIGN_OP(qi::lit(L'*'));
				ARITHMETIC_DIV   = DISTINCT_NONASSIGN_OP(qi::lit(L'/'));
				ARITHMETIC_MOD   = DISTINCT_NONASSIGN_OP(qi::lit(L'%'));
			}

			// binary operators
			{
				BINARY_AND = distinct(qi::lit(L'&') | L'=')[qi::lit(L'&')];
				BINARY_OR  = distinct(qi::lit(L'|') | L'=')[qi::lit(L'|')];
				BINARY_XOR = DISTINCT_NONASSIGN_OP(qi::lit(L'^'));
				BINARY_NOT = qi::lit(L'~');
			}

			// shift operators
			{
				RSHIFT = DISTINCT_NONASSIGN_OP(qi::lit(L">>"));
				LSHIFT = DISTINCT_NONASSIGN_OP(qi::lit(L"<<"));
			}

			// logical operators
			{
				LOGICAL_AND = qi::lit(L"&&");
				LOGICAL_OR  = qi::lit(L"||");
				LOGICAL_NOT = DISTINCT_NONASSIGN_OP(qi::lit(L'!'));
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

		///////////////////////////////////////////////////////////////////////////////
		// Keywords
		///////////////////////////////////////////////////////////////////////////////

#define DECL_TOKEN(token, str) \
	token = DISTINCT_IDENTIFIER(qi::lit(str));

		// tokens
		{
			DECL_TOKEN(_TRUE, L"true");
			DECL_TOKEN(_FALSE, L"false");
			DECL_TOKEN(_NULL, L"null");
			DECL_TOKEN(_SELF, L"self");
			DECL_TOKEN(_GLOBAL, L"global");

			DECL_TOKEN(CONST, L"const");
			DECL_TOKEN(STATIC, L"static");

			DECL_TOKEN(INT8, L"int8");
			DECL_TOKEN(UINT8, L"uint8");
			DECL_TOKEN(INT16, L"int16");
			DECL_TOKEN(UINT16, L"uint16");
			DECL_TOKEN(INT32, L"int32");
			DECL_TOKEN(UINT32, L"uint32");
			DECL_TOKEN(INT64, L"int64");
			DECL_TOKEN(UINT64, L"uint64");
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

			DECL_TOKEN(FOREACH, L"foreach");
			DECL_TOKEN(IN, L"in");
			DECL_TOKEN(DO, L"do");
			DECL_TOKEN(WHILE, L"while");

			DECL_TOKEN(RETURN, L"return");
			DECL_TOKEN(BREAK, L"break");
			DECL_TOKEN(CONTINUE, L"continue");

			DECL_TOKEN(NEW, L"new");
			DECL_TOKEN(AS, L"as");
			DECL_TOKEN(INSTANCEOF, L"instanceof");

			DECL_TOKEN(PACKAGE, L"package");
			DECL_TOKEN(IMPORT, L"import");

			DECL_TOKEN(EXTENDS, L"extends");
			DECL_TOKEN(IMPLEMENTS, L"implements");
		}

		///////////////////////////////////////////////////////////////////////////////
		// Rule Define
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// BEGIN BASIC
		//

		typed_parameter_list
			= ((IDENTIFIER > -colon_type_specifier) % COMMA) [ typename SA::typed_parameter_list::init() ]
			;

		colon_type_specifier
			= COLON > type_specifier [ typename SA::colon_type_specifier::init() ]
			;

		type_specifier
			= qi::lit(L"void")                                                                     [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::VOID>() ]
			| qi::lit(L"int8")                                                                     [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::INT8>() ]
			| qi::lit(L"uint8")                                                                    [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::UINT8>() ]
			| qi::lit(L"int16")                                                                    [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::INT16>() ]
			| qi::lit(L"uint16")                                                                   [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::UINT16>() ]
			| qi::lit(L"int32")                                                                    [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::INT32>() ]
			| qi::lit(L"uint32")                                                                   [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::UINT32>() ]
			| qi::lit(L"int64")                                                                    [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::INT64>() ]
			| qi::lit(L"uint64")                                                                   [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::UINT64>() ]
			| qi::lit(L"float32")                                                                  [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::FLOAT32>() ]
			| qi::lit(L"float64")                                                                  [ typename SA::type_specifier::template init_primitive_type<tree::TypeSpecifier::PrimitiveType::FLOAT64>() ]
			| (nested_identifier > -(COMPARE_LT >> type_list_specifier > COMPARE_GT))              [ typename SA::type_specifier::init_type() ]
			| (FUNCTION > LEFT_PAREN > -type_list_specifier > RIGHT_PAREN > -colon_type_specifier) [ typename SA::type_specifier::init_function_type() ]
			| ELLIPSIS                                                                             [ typename SA::type_specifier::init_ellipsis() ]
			;

		template_param_identifier
			= (IDENTIFIER > -(COMPARE_LT > ((IDENTIFIER | (ELLIPSIS > qi::attr(true))) % COMMA) > COMPARE_GT)) [ typename SA::template_param_identifier::init() ]
			;

		template_arg_identifier
			= (IDENTIFIER > -(COMPARE_LT >> type_list_specifier > COMPARE_GT)) [ typename SA::template_arg_identifier::init() ]
			;

		type_list_specifier
			= (type_specifier % COMMA) [ typename SA::type_list_specifier::init() ]
			;

		storage_specifier
			= STATIC [ typename SA::storage_specifier::init_static() ]
			| CONST  [ typename SA::storage_specifier::init_const() ]
			;

		visibility_specifier
			= PUBLIC    [ typename SA::visibility_specifier::init_public() ]
			| PROTECTED [ typename SA::visibility_specifier::init_protected() ]
			| PRIVATE   [ typename SA::visibility_specifier::init_private() ]
			;

		interface_visibility_specifier
			= PUBLIC    [ typename SA::visibility_specifier::init_public() ]
			| PROTECTED [ typename SA::visibility_specifier::init_protected() ]
			;

		annotation_specifiers
			= (*annotation_specifier) [ typename SA::annotation_specifiers::init() ]
			;

		annotation_specifier
			=	(AT_SYMBOL > IDENTIFIER
					>	-(LEFT_BRACE > (
						(IDENTIFIER > ASSIGN > primary_expression) % COMMA
						) > RIGHT_BRACE)
				) [ typename SA::annotation_specifier::init() ]
			;

		nested_identifier
			= (IDENTIFIER > *( DOT > IDENTIFIER)) [ typename SA::nested_identifier::init() ]
			;

		//
		// END BASIC
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// BEGIN EXPRESSION
		//
		// 0.  POSTFIX_STEP
		// 1.  UNARY_SIGN, PREFIX_STEP
		// 2.  MULTIPLICATIVE
		// 3.  ADDITIVE
		// 4.  BITWISE_SHIFT
		// 5.  RELATIONAL
		// 6.  EQUALITY
		// 7.  BITWISE_AND
		// 8.  BITWISE_XOR
		// 9.  BITWISE_OR
		// 10. LOGICAL_AND
		// 11. LOGICAL_OR
		// 12. RANGE
		// 13. TERNARY
		// 14. ASSIGNMENT, MODIFY_ASSIGN
		// 15. COMMA
		//

		primary_expression
			= template_arg_identifier                 [ typename SA::primary_expression::init() ]
			| INTEGER_LITERAL                         [ typename SA::primary_expression::init() ]
			| FLOAT_LITERAL                           [ typename SA::primary_expression::init() ]
			| STRING_LITERAL                          [ typename SA::primary_expression::init() ]
			| _TRUE                                   [ typename SA::primary_expression::template init_bool<true>() ]
			| _FALSE                                  [ typename SA::primary_expression::template init_bool<false>() ]
			| _NULL                                   [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::NULL_OBJECT>() ]
			| _SELF                                   [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::SELF_OBJECT>() ]
			| _GLOBAL                                 [ typename SA::primary_expression::template init_object_literal<tree::ObjectLiteral::LiteralType::GLOBAL_OBJECT>() ]
			| (LEFT_PAREN > expression > RIGHT_PAREN) [ typename SA::primary_expression::init_paren_expression() ]
			| (FUNCTION > LEFT_PAREN > -typed_parameter_list > RIGHT_PAREN > -colon_type_specifier
				> block) [ typename SA::primary_expression::init_lambda() ]
			;

		// postfix expression
		// associativity: left-to-right
		// rank: 0
		postfix_expression
			= primary_expression                                         [ typename SA::postfix_expression::init_primary_expression() ]
				>	*( (LEFT_BRACKET > expression > RIGHT_BRACKET)       [ typename SA::postfix_expression::append_postfix_array() ]
					| (LEFT_PAREN > -(expression % COMMA) > RIGHT_PAREN) [ typename SA::postfix_expression::append_postfix_call() ]
					| (DOT >> template_arg_identifier)                   [ typename SA::postfix_expression::append_postfix_member() ]
					| INCREMENT                                          [ typename SA::postfix_expression::template append_postfix_step<tree::UnaryExpr::OpCode::POSTFIX_INCREMENT>() ]
					| DECREMENT                                          [ typename SA::postfix_expression::template append_postfix_step<tree::UnaryExpr::OpCode::POSTFIX_DECREMENT>() ]
					)
			;

		// prefix expression
		// associativity: right-to-left
		// rank: 1
		prefix_expression
			=	(postfix_expression
				|	(
						( INCREMENT        > qi::attr(tree::UnaryExpr::OpCode::PREFIX_INCREMENT)
						| DECREMENT        > qi::attr(tree::UnaryExpr::OpCode::PREFIX_DECREMENT)
						| BINARY_NOT       > qi::attr(tree::UnaryExpr::OpCode::BINARY_NOT)
						| LOGICAL_NOT      > qi::attr(tree::UnaryExpr::OpCode::LOGICAL_NOT)
						| ARITHMETIC_MINUS > qi::attr(tree::UnaryExpr::OpCode::ARITHMETIC_NEGATE)
						| NEW              > qi::attr(tree::UnaryExpr::OpCode::NEW)
						) > prefix_expression
					)
				) [ typename SA::prefix_expression::init() ]
			;

		// multiplicative expression
		// associativity: left-to-right
		// rank: 2
		multiplicative_expression
			=	(prefix_expression
				%	( ARITHMETIC_MUL > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_MUL)
					| ARITHMETIC_DIV > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_DIV)
					| ARITHMETIC_MOD > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_MOD)
					) [ typename SA::left_to_right_binary_op_vec::append_op() ]
				) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// additive expression
		// associativity: left-to-right
		// rank: 3
		additive_expression
			=	(multiplicative_expression
				%	( ARITHMETIC_PLUS  > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_ADD)
					| ARITHMETIC_MINUS > qi::attr(tree::BinaryExpr::OpCode::ARITHMETIC_SUB)
					) [ typename SA::left_to_right_binary_op_vec::append_op() ]
				) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// shift expression
		// associativity: left-to-right
		// rank: 4
		shift_expression
			=	(additive_expression
				%	( RSHIFT > qi::attr(tree::BinaryExpr::OpCode::BINARY_RSHIFT)
					| LSHIFT > qi::attr(tree::BinaryExpr::OpCode::BINARY_LSHIFT)
					) [ typename SA::left_to_right_binary_op_vec::append_op() ]
				) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// rational expression
		// associativity: left-to-right
		// rank: 5
		relational_expression
			=	(shift_expression
				%	( COMPARE_GT > qi::attr(tree::BinaryExpr::OpCode::COMPARE_GT)
					| COMPARE_LT > qi::attr(tree::BinaryExpr::OpCode::COMPARE_LT)
					| COMPARE_GE > qi::attr(tree::BinaryExpr::OpCode::COMPARE_GE)
					| COMPARE_LE > qi::attr(tree::BinaryExpr::OpCode::COMPARE_LE)
					| INSTANCEOF > qi::attr(tree::BinaryExpr::OpCode::INSTANCEOF)
					) [ typename SA::left_to_right_binary_op_vec::append_op() ]
				) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// equality expression
		// associativity: left-to-right
		// rank: 6
		equality_expression
			=	(relational_expression
				%	( COMPARE_EQ > qi::attr(tree::BinaryExpr::OpCode::COMPARE_EQ)
					| COMPARE_NE > qi::attr(tree::BinaryExpr::OpCode::COMPARE_NE)
					) [ typename SA::left_to_right_binary_op_vec::append_op() ]
				) [ typename SA::left_to_right_binary_op_vec::init() ]
			;

		// and expression
		// associativity: left-to-right
		// rank: 7
		and_expression
			= (equality_expression % BINARY_AND) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::BINARY_AND>() ]
			;

		// xor expression
		// associativity: left-to-right
		// rank: 8
		xor_expression
			= (and_expression % BINARY_XOR) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::BINARY_XOR>() ]
			;

		// or expression
		// associativity: left-to-right
		// rank: 9
		or_expression
			= (xor_expression % BINARY_OR) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::BINARY_OR>() ]
			;

		// logical and expression
		// associativity: left-to-right
		// rank: 10
		logical_and_expression
			= (or_expression % LOGICAL_AND) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::LOGICAL_AND>() ]
			;

		// logical or expression
		// associativity: left-to-right
		// rank: 11
		logical_or_expression
			= (logical_and_expression % LOGICAL_OR) [ typename SA::left_to_right_binary_op::template init<tree::BinaryExpr::OpCode::LOGICAL_OR>() ]
			;

		// range expression
		// associativity: left-to-right
		// rank: 12
		range_expression
			= (logical_or_expression > -(ELLIPSIS > logical_or_expression)) [ typename SA::range_expression::init() ]
			;

		// ternary expression
		// associativity: right-to-left
		// rank: 13
		ternary_expression
			= (range_expression > -(Q_MARK > range_expression > COLON > range_expression)) [ typename SA::ternary_expression::init() ]
			;

		// assignment_expression
		// associativity: right-to-left
		// rank: 14
		expression
			=	(ternary_expression
				%	( ASSIGN        > qi::attr(tree::BinaryExpr::OpCode::ASSIGN)
					| RSHIFT_ASSIGN > qi::attr(tree::BinaryExpr::OpCode::RSHIFT_ASSIGN)
					| LSHIFT_ASSIGN > qi::attr(tree::BinaryExpr::OpCode::LSHIFT_ASSIGN)
					| PLUS_ASSIGN   > qi::attr(tree::BinaryExpr::OpCode::ADD_ASSIGN)
					| MINUS_ASSIGN  > qi::attr(tree::BinaryExpr::OpCode::SUB_ASSIGN)
					| MUL_ASSIGN    > qi::attr(tree::BinaryExpr::OpCode::MUL_ASSIGN)
					| DIV_ASSIGN    > qi::attr(tree::BinaryExpr::OpCode::DIV_ASSIGN)
					| MOD_ASSIGN    > qi::attr(tree::BinaryExpr::OpCode::MOD_ASSIGN)
					| AND_ASSIGN    > qi::attr(tree::BinaryExpr::OpCode::AND_ASSIGN)
					| OR_ASSIGN     > qi::attr(tree::BinaryExpr::OpCode::OR_ASSIGN)
					| XOR_ASSIGN    > qi::attr(tree::BinaryExpr::OpCode::XOR_ASSIGN)
					) [ typename SA::right_to_left_binary_op_vec::append_op() ]
				) [ typename SA::right_to_left_binary_op_vec::init() ]
			;

		//
		// END EXPRESSION
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// BEGIN STATEMENT
		//

		statement
			=	(-annotation_specifiers
					>>	( const_variable_decl
						| expression_statement
						| selection_statement
						| iteration_statement
						| branch_statement
						)
				) [ typename SA::statement::init() ]
			|	block [ typename SA::statement::init_block() ]
			;

		expression_statement
			= (-expression >> SEMICOLON) [ typename SA::expression_statement::init() ]
			;

		selection_statement
				=	(IF > LEFT_PAREN > expression > RIGHT_PAREN > statement
						> *(ELIF > LEFT_PAREN > expression > RIGHT_PAREN > statement)
						> -(ELSE > statement)
					) [ typename SA::selection_statement::init_if_statement() ]
				|	(SWITCH > LEFT_PAREN > expression > RIGHT_PAREN
						> LEFT_BRACE
						>	*( CASE > expression > COLON > *statement
							| DEFAULT > COLON > *statement
							)
						> RIGHT_BRACE
					) [ typename SA::selection_statement::init_switch_statement() ]
			;

		iteration_statement
			=	(WHILE > LEFT_PAREN > expression > RIGHT_PAREN > -statement)                 [ typename SA::iteration_statement::init_while_loop() ]
			|	(DO > statement > WHILE > LEFT_PAREN > expression > RIGHT_PAREN > SEMICOLON) [ typename SA::iteration_statement::init_do_while_loop() ]
			|	(FOREACH > LEFT_PAREN > (variable_decl_stem | postfix_expression) > IN > expression > RIGHT_PAREN > -statement
				) [ typename SA::iteration_statement::init_foreach() ]
			;

		branch_statement
			=	( (RETURN > expression_statement) [ typename SA::branch_statement::init_return() ]
				| (BREAK > SEMICOLON)             [ typename SA::branch_statement::template init<tree::BranchStmt::OpCode::BREAK>() ]
				| (CONTINUE > SEMICOLON)          [ typename SA::branch_statement::template init<tree::BranchStmt::OpCode::CONTINUE>() ]
				)
			;

		block
			=	(LEFT_BRACE
					> *statement
					> RIGHT_BRACE
				) [ typename SA::block::init() ]
			;

		//
		// END STATEMENT
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// BEGIN DECLARATION
		//

		declaration
			=	(-annotation_specifiers
					>>	( const_variable_decl
						| function_decl
						| typedef_decl
						| class_decl
						| interface_decl
						| enum_decl
						)
				) [ typename SA::declaration::init() ]
			;

		const_variable_decl
			= (-(CONST > qi::attr(true)) >> variable_decl) [ typename SA::const_variable_decl::init() ]
			;

		variable_decl
			= (variable_decl_stem > -(ASSIGN > expression) > SEMICOLON) [ typename SA::variable_decl::init() ]
			;

		variable_decl_stem
			= (VAR > IDENTIFIER > -colon_type_specifier) [ typename SA::variable_decl_stem::init() ]
			;

		function_decl
			= (FUNCTION > (template_param_identifier | (NEW > qi::attr(true))) > LEFT_PAREN > -typed_parameter_list > RIGHT_PAREN > -colon_type_specifier
				> -block
				) [ typename SA::function_decl::init() ]
			;

		typedef_decl
			= (TYPEDEF > type_specifier > IDENTIFIER > SEMICOLON) [ typename SA::typedef_decl::init() ]
			;

		class_decl
			= (CLASS > template_param_identifier > -(EXTENDS > nested_identifier) > -((IMPLEMENTS > nested_identifier) % COMMA)
				> LEFT_BRACE
				> *class_member_decl
				> RIGHT_BRACE
				) [ typename SA::class_decl::init() ]
			;

		class_member_decl
			=	(-annotation_specifiers >> -visibility_specifier >> -storage_specifier
					>>	( variable_decl
						| function_decl
						)
				) [ typename SA::class_member_decl::init() ]
			;

		interface_decl
			= (INTERFACE > IDENTIFIER
				> LEFT_BRACE
				> *interface_member_function_decl
				> RIGHT_BRACE
				) [ typename SA::interface_decl::init() ]
			;

		interface_member_function_decl
			=	(-interface_visibility_specifier >> FUNCTION > IDENTIFIER > LEFT_PAREN > -typed_parameter_list > RIGHT_PAREN > colon_type_specifier > SEMICOLON
				) [ typename SA::interface_member_function_decl::init() ]
			;

		enum_decl
			= (ENUM > IDENTIFIER
				> LEFT_BRACE
				> (-annotation_specifiers > IDENTIFIER > -(ASSIGN > expression)) % COMMA
				> RIGHT_BRACE
				) [ typename SA::enum_decl::init() ]
			;

		//
		// END DECLARATION
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		// BEGIN MODULE
		//

		program
			= qi::eps                                          [ typename SA::program::init() ]
				> -( (PACKAGE > nested_identifier > SEMICOLON) [ typename SA::program::append_package() ] )
				> *( (IMPORT > nested_identifier > SEMICOLON)  [ typename SA::program::append_import() ] )
				> *( declaration                               [ typename SA::program::append_declaration() ] )
			;

		//
		// END MODULE
		///////////////////////////////////////////////////////////

		start
			= program > qi::eoi
			;

		///////////////////////////////////////////////////////////////////////////////
		// Debugging Names / Handlers
		///////////////////////////////////////////////////////////////////////////////

		// keywords
		_TRUE.name("TRUE_LITERAL"); _FALSE.name("FALSE_LITERAL"); _NULL.name("NULL_LITERAL");

		// terminals
		IDENTIFIER.name("IDENTIFIER.grammar");
		INTEGER_LITERAL.name("INTEGER_LITERAL.grammar");
		FLOAT_LITERAL.name("FLOAT_LITERAL.grammar");
		STRING_LITERAL.name("STRING_LITERAL.grammar");

		// non-terminals
		typed_parameter_list.name("typed_parameter_list"); colon_type_specifier.name("colon_type_specifier"); type_specifier.name("type_specifier"); template_param_identifier.name("template_param_identifier"); template_arg_identifier.name("template_arg_identifier"); type_list_specifier.name("type_list_specifier");
			storage_specifier.name("storage_specifier");
			visibility_specifier.name("visibility_specifier");
			annotation_specifiers.name("annotation_specifiers");
			annotation_specifier.name("annotation_specifier");
			nested_identifier.name("nested_identifier");
		expression.name("expression");
			primary_expression.name("primary_expression");
			postfix_expression.name("postfix_expression");
			prefix_expression.name("prefix_expression");
			multiplicative_expression.name("multiplicative_expression");
			additive_expression.name("additive_expression");
			shift_expression.name("shift_expression");
			relational_expression.name("relational_expression");
			equality_expression.name("equality_expression");
			and_expression.name("and_expression");
			xor_expression.name("xor_expression");
			or_expression.name("or_expression");
			logical_and_expression.name("logical_and_expression");
			logical_or_expression.name("logical_or_expression");
			ternary_expression.name("ternary_expression");
		statement.name("statement");
			block.name("block");
			expression_statement.name("expression_statement");
			selection_statement.name("selection_statement");
			iteration_statement.name("iteration_statement");
			branch_statement.name("branch_statement");
		declaration.name("declaration");
			variable_decl.name("variable_decl");
			function_decl.name("function_decl");
			typedef_decl.name("typedef_decl");
			class_decl.name("class_decl"); class_member_decl.name("class_member_decl");
			interface_decl.name("interface_decl"); interface_member_function_decl.name("interface_member_function_decl");
			enum_decl.name("enum_decl");
		program.name("program");
		start.name("start");

		if(getParserContext().enable_debug_parser)
		{
			// keywords
			debug(_TRUE); debug(_FALSE); debug(_NULL);

			// terminals
#if 0 // NOTE: grammars cannot have debug_handlers, only rules can
			debug(IDENTIFIER);
			debug(INTEGER_LITERAL);
			debug(FLOAT_LITERAL);
			debug(STRING_LITERAL);
#endif

			// non-terminals
			debug(typed_parameter_list); debug(colon_type_specifier); debug(type_specifier); debug(template_param_identifier); debug(template_arg_identifier); debug(type_list_specifier);
				debug(storage_specifier);
				debug(visibility_specifier);
				debug(annotation_specifiers);
				debug(annotation_specifier);
				debug(nested_identifier);
			debug(expression);
				debug(primary_expression);
				debug(postfix_expression);
				debug(prefix_expression);
				debug(multiplicative_expression);
				debug(additive_expression);
				debug(shift_expression);
				debug(relational_expression);
				debug(equality_expression);
				debug(and_expression);
				debug(xor_expression);
				debug(or_expression);
				debug(logical_and_expression);
				debug(logical_or_expression);
				debug(ternary_expression);
			debug(statement);
				debug(block);
				debug(expression_statement);
				debug(selection_statement);
				debug(iteration_statement);
				debug(branch_statement);
			debug(declaration);
				debug(variable_decl);
				debug(function_decl);
				debug(typedef_decl);
				debug(class_decl); debug(class_member_decl);
				debug(interface_decl); debug(interface_member_function_decl);
				debug(enum_decl);
			debug(program);
			debug(start);
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// Rule Declare
	///////////////////////////////////////////////////////////////////////////////

	// operators
	qi::rule<Iterator, detail::WhiteSpace<Iterator> >
		ELLIPSIS, DOT, COLON, SEMICOLON, COMMA, AT_SYMBOL, Q_MARK,
		ASSIGN, RSHIFT_ASSIGN, LSHIFT_ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, OR_ASSIGN, XOR_ASSIGN,
		INCREMENT, DECREMENT,
		ARITHMETIC_PLUS, ARITHMETIC_MINUS, ARITHMETIC_MUL, ARITHMETIC_DIV, ARITHMETIC_MOD,
		BINARY_AND, BINARY_OR, BINARY_XOR, BINARY_NOT,
		RSHIFT, LSHIFT,
		LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
		COMPARE_EQ, COMPARE_NE, COMPARE_GT, COMPARE_LT, COMPARE_GE, COMPARE_LE,
		LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET, LEFT_PAREN, RIGHT_PAREN;

	// keywords
	qi::rule<Iterator, detail::WhiteSpace<Iterator> >
		_TRUE, _FALSE, _NULL, _SELF, _GLOBAL,
		CONST, STATIC,
		INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, FLOAT32, FLOAT64, VOID,
		TYPEDEF, CLASS, INTERFACE, ENUM,
		PUBLIC, PROTECTED, PRIVATE,
		VAR, FUNCTION,
		IF, ELIF, ELSE,
		SWITCH, CASE, DEFAULT,
		FOREACH, IN, DO, WHILE,
		RETURN, BREAK, CONTINUE,
		NEW, AS, INSTANCEOF,
		PACKAGE, IMPORT,
		EXTENDS, IMPLEMENTS;

	// terminals
	detail::Identifier<Iterator, SA>     IDENTIFIER;
	detail::IntegerLiteral<Iterator, SA> INTEGER_LITERAL;
	detail::FloatLiteral<Iterator, SA>   FLOAT_LITERAL;
	detail::StringLiteral<Iterator, SA>  STRING_LITERAL;

	// non-terminals
//	qi::rule<Iterator, detail::WhiteSpace<Iterator> >
		///////////////////////////////////////
		// BEGIN BASIC
//		typed_parameter_list, colon_type_specifier, type_specifier, template_param_identifier, template_arg_identifier, type_list_specifier,
//			storage_specifier,
//			visibility_specifier,
//			interface_visibility_specifier,
//			annotation_specifiers,
//			annotation_specifier,
//			nested_identifier,
		// END BASIC
		///////////////////////////////////////
		///////////////////////////////////////
		// BEGIN EXPRESSION
//		expression,
//			primary_expression,
//			postfix_expression,
//			prefix_expression,
//			multiplicative_expression,
//			additive_expression,
//			shift_expression,
//			relational_expression,
//			equality_expression,
//			and_expression,
//			xor_expression,
//			or_expression,
//			logical_and_expression,
//			logical_or_expression,
//			range_expression,
//			ternary_expression,
		// END EXPRESSION
		///////////////////////////////////////
		///////////////////////////////////////
		// BEGIN STATEMENT
//		statement,
//			block,
//			expression_statement,
//			selection_statement,
//			iteration_statement,
//			branch_statement,
		// END STATEMENT
		///////////////////////////////////////
		///////////////////////////////////////
		// BEGIN DECLARATION
//		declaration,
//			variable_decl,
//			function_decl,
//			typedef_decl,
//			class_decl, class_member_decl,
//			interface_decl, interface_member_function_decl,
//			enum_decl,
		// END DECLARATION
		///////////////////////////////////////
		///////////////////////////////////////
		// BEGIN MODULE
//		program,
		// END MODULE
		///////////////////////////////////////

	// basic
	qi::rule<Iterator, typename SA::nested_identifier::attribute_type,         detail::WhiteSpace<Iterator>, typename SA::nested_identifier::local_type>         nested_identifier;
	qi::rule<Iterator, typename SA::typed_parameter_list::attribute_type,      detail::WhiteSpace<Iterator>, typename SA::typed_parameter_list::local_type>      typed_parameter_list;
	qi::rule<Iterator, typename SA::colon_type_specifier::attribute_type,      detail::WhiteSpace<Iterator>, typename SA::colon_type_specifier::local_type>      colon_type_specifier;
	qi::rule<Iterator, typename SA::type_specifier::attribute_type,            detail::WhiteSpace<Iterator>, typename SA::type_specifier::local_type>            type_specifier;
	qi::rule<Iterator, typename SA::template_param_identifier::attribute_type, detail::WhiteSpace<Iterator>, typename SA::template_param_identifier::local_type> template_param_identifier;
	qi::rule<Iterator, typename SA::template_arg_identifier::attribute_type,   detail::WhiteSpace<Iterator>, typename SA::template_arg_identifier::local_type>   template_arg_identifier;
	qi::rule<Iterator, typename SA::type_list_specifier::attribute_type,       detail::WhiteSpace<Iterator>, typename SA::type_list_specifier::local_type>       type_list_specifier;
	qi::rule<Iterator, typename SA::storage_specifier::attribute_type,         detail::WhiteSpace<Iterator>, typename SA::storage_specifier::local_type>         storage_specifier;
	qi::rule<Iterator, typename SA::visibility_specifier::attribute_type,      detail::WhiteSpace<Iterator>, typename SA::visibility_specifier::local_type>      visibility_specifier;
	qi::rule<Iterator, typename SA::visibility_specifier::attribute_type,      detail::WhiteSpace<Iterator>, typename SA::visibility_specifier::local_type>      interface_visibility_specifier;
	qi::rule<Iterator, typename SA::annotation_specifiers::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::annotation_specifiers::local_type>     annotation_specifiers;
	qi::rule<Iterator, typename SA::annotation_specifier::attribute_type,      detail::WhiteSpace<Iterator>, typename SA::annotation_specifier::local_type>      annotation_specifier;

	// expression
	qi::rule<Iterator, typename SA::right_to_left_binary_op_vec::attribute_type, detail::WhiteSpace<Iterator>, typename SA::right_to_left_binary_op_vec::local_type> expression;
	qi::rule<Iterator, typename SA::primary_expression::attribute_type,          detail::WhiteSpace<Iterator>, typename SA::primary_expression::local_type>          primary_expression;
	qi::rule<Iterator, typename SA::postfix_expression::attribute_type,          detail::WhiteSpace<Iterator>, typename SA::postfix_expression::local_type>          postfix_expression;
	qi::rule<Iterator, typename SA::prefix_expression::attribute_type,           detail::WhiteSpace<Iterator>, typename SA::prefix_expression::local_type>           prefix_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op_vec::attribute_type, detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op_vec::local_type> multiplicative_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op_vec::attribute_type, detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op_vec::local_type> additive_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op_vec::attribute_type, detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op_vec::local_type> shift_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op_vec::attribute_type, detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op_vec::local_type> relational_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op_vec::attribute_type, detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op_vec::local_type> equality_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op::local_type>     and_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op::local_type>     xor_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op::local_type>     or_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op::local_type>     logical_and_expression;
	qi::rule<Iterator, typename SA::left_to_right_binary_op::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::left_to_right_binary_op::local_type>     logical_or_expression;
	qi::rule<Iterator, typename SA::range_expression::attribute_type,            detail::WhiteSpace<Iterator>, typename SA::range_expression::local_type>            range_expression;
	qi::rule<Iterator, typename SA::ternary_expression::attribute_type,          detail::WhiteSpace<Iterator>, typename SA::ternary_expression::local_type>          ternary_expression;

	// statement
	qi::rule<Iterator, typename SA::statement::attribute_type,            detail::WhiteSpace<Iterator>, typename SA::statement::local_type>            statement;
	qi::rule<Iterator, typename SA::block::attribute_type,                detail::WhiteSpace<Iterator>, typename SA::block::local_type>                block;
	qi::rule<Iterator, typename SA::expression_statement::attribute_type, detail::WhiteSpace<Iterator>, typename SA::expression_statement::local_type> expression_statement;
	qi::rule<Iterator, typename SA::selection_statement::attribute_type,  detail::WhiteSpace<Iterator>, typename SA::selection_statement::local_type>  selection_statement;
	qi::rule<Iterator, typename SA::iteration_statement::attribute_type,  detail::WhiteSpace<Iterator>, typename SA::iteration_statement::local_type>  iteration_statement;
	qi::rule<Iterator, typename SA::branch_statement::attribute_type,     detail::WhiteSpace<Iterator>, typename SA::branch_statement::local_type>     branch_statement;

	// declaration
	qi::rule<Iterator, typename SA::declaration::attribute_type,         detail::WhiteSpace<Iterator>, typename SA::declaration::local_type>         declaration;
	qi::rule<Iterator, typename SA::const_variable_decl::attribute_type, detail::WhiteSpace<Iterator>, typename SA::const_variable_decl::local_type> const_variable_decl;
	qi::rule<Iterator, typename SA::variable_decl::attribute_type,       detail::WhiteSpace<Iterator>, typename SA::variable_decl::local_type>       variable_decl;
	qi::rule<Iterator, typename SA::variable_decl_stem::attribute_type,  detail::WhiteSpace<Iterator>, typename SA::variable_decl_stem::local_type>  variable_decl_stem;
	qi::rule<Iterator, typename SA::function_decl::attribute_type,       detail::WhiteSpace<Iterator>, typename SA::function_decl::local_type>       function_decl;
	qi::rule<Iterator, typename SA::typedef_decl::attribute_type,        detail::WhiteSpace<Iterator>, typename SA::typedef_decl::local_type>        typedef_decl;
	qi::rule<Iterator, typename SA::class_decl::attribute_type,          detail::WhiteSpace<Iterator>, typename SA::class_decl::local_type>          class_decl;
	qi::rule<Iterator, typename SA::class_member_decl::attribute_type,   detail::WhiteSpace<Iterator>, typename SA::class_member_decl::local_type>   class_member_decl;
	qi::rule<Iterator, typename SA::interface_decl::attribute_type,      detail::WhiteSpace<Iterator>, typename SA::interface_decl::local_type>      interface_decl;
	qi::rule<Iterator, typename SA::interface_member_function_decl::attribute_type, detail::WhiteSpace<Iterator>, typename SA::interface_member_function_decl::local_type> interface_member_function_decl;
	qi::rule<Iterator, typename SA::enum_decl::attribute_type,           detail::WhiteSpace<Iterator>, typename SA::enum_decl::local_type>           enum_decl;

	// module
	qi::rule<Iterator, typename SA::program::attribute_type, detail::WhiteSpace<Iterator>, typename SA::program::local_type> program;

	// start
	qi::rule<Iterator, typename SA::start::attribute_type, detail::WhiteSpace<Iterator>, typename SA::start::local_type> start;
};

} } }

#endif /* ZILLIANS_LANGUAGE_GRAMMAR_THORSCRIPT_H_ */
