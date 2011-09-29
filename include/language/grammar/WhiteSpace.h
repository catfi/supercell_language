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
 *
 * @date Sep 27, 2011, yoco - Init version.
 */

#ifndef ZILLIANS_LANGUAGE_GRAMMAR_WHITESPACE_H_
#define ZILLIANS_LANGUAGE_GRAMMAR_WHITESPACE_H_

#define BOOST_SPIRIT_UNICODE

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_as.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/home/support/char_encoding/unicode.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace qi = boost::spirit::qi;
namespace unicode = boost::spirit::unicode;

namespace zillians { namespace language { namespace grammar {

/////////////////////////////////////////////////////////////////////
/// detail
/////////////////////////////////////////////////////////////////////

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

} // detail

} } } // zillians language grammar

#endif /*ZILLIANS_LANGUAGE_GRAMMAR_WHITESPACE_H_ */

