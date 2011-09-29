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

#define BOOST_SPIRIT_UNICODE

#include <boost/spirit/include/classic_position_iterator.hpp>

#ifndef ZILLIANS_LANGUAGE_GRAMMAR_PACKAGEDEPENCYPARSER_H_
#define ZILLIANS_LANGUAGE_GRAMMAR_PACKAGEDEPENCYPARSER_H_

namespace zillians { namespace language { namespace grammar {

/////////////////////////////////////////////////////////////////////
/// Package Dependency
/////////////////////////////////////////////////////////////////////

typedef boost::spirit::classic::position_iterator2<std::wstring::iterator> pos_iterator_type;

/// @note In order to reduce compile time, we expose only a non-template function here.
/// The implementation is in '../../../src/libzillians-framework-language/language/stage/dep/ThorScriptPackageDependencyGrammar.cpp'
bool getImportedPackages(pos_iterator_type begin, pos_iterator_type end, std::vector<std::wstring>& v);

} } }

#endif /*ZILLIANS_LANGUAGE_GRAMMAR_PACKAGEDEPENCYPARSER_H_ */
