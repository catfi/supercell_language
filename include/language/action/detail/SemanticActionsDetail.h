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

#ifndef ZILLIANS_LANGUAGE_ACTION_DETAIL_SEMANTICACTIONSDETAIL_H_
#define ZILLIANS_LANGUAGE_ACTION_DETAIL_SEMANTICACTIONSDETAIL_H_

#include "core/Prerequisite.h"
#include "utility/Foreach.h"
#include "language/GlobalContext.h"
#include "language/context/ParserContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/tree/ASTNodeFactory.h"

#include <boost/mpl/bool.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/add_reference.hpp>

#define DEFINE_ATTRIBUTES(return_type, ...) typedef return_type (attribute_type) (__VA_ARGS__);
#define DEFINE_LOCALS(...) typedef boost::spirit::locals<__VA_ARGS__> local_type;
#define DEFINE_ACTION(name) \
		struct name \
		{ \
			template<typename Attribute, typename Context> \
			void operator()(Attribute& parser_attribute, Context& context, bool& passed) const; \
		};

#define BEGIN_ACTION(name) \
		struct name \
		{ \
			template<typename ParserAttribute, typename ParserContext> \
			void operator()(ParserAttribute& parser_attribute, ParserContext& context, bool& passed) const \
			{ \
				UNUSED_ARGUMENT(parser_attribute); \
				UNUSED_ARGUMENT(context); \
				UNUSED_ARGUMENT(passed); \
				if(!getParserContext().enable_semantic_action) \
					return;

#define BEGIN_TEMPLATED_ACTION(name, ...) \
		template<__VA_ARGS__> \
		struct name \
		{ \
			template<typename ParserAttribute, typename ParserContext> \
			void operator()(ParserAttribute& parser_attribute, ParserContext& context, bool& passed) const \
			{ \
				UNUSED_ARGUMENT(parser_attribute); \
				UNUSED_ARGUMENT(context); \
				UNUSED_ARGUMENT(passed); \
				if(!getParserContext().enable_semantic_action) \
					return;

#define END_ACTION \
			} \
		};

#define _result   boost::fusion::at_c<0>(context.attributes)
#define _result_t decltype(boost::fusion::at_c<0>(context.attributes))

#define _param(i)   detail::attribute_accessor<i, ParserAttribute>::get(parser_attribute)
#define _param_t(i) typename detail::attribute_accessor<i, ParserAttribute>::result_type

#define _aux_param(i)   boost::fusion::at_c<i+1>(context.attributes)
#define _aux_param_t(i) decltype(boost::fusion::at_c<i+1>(context.attributes))

#define _local(i)   boost::fusion::at_c<i>(context.locals)
#define _local_t(i) decltype(boost::fusion::at_c<i>(context.locals))

#if 0 // NOTE: unused
#define BIND_LOCATION(x) \
		stage::SourceInfoContext::set((x), new stage::SourceInfoContext( \
				getParserContext().debug.source_index, \
				getParserContext().debug.line, \
				getParserContext().debug.column))
#endif

#define LOCATION_TYPE shared_ptr<stage::SourceInfoContext> // _local(0)
#define CACHE_LOCATION \
		{ \
			BOOST_MPL_ASSERT(( boost::is_same<_local_t(0), LOCATION_TYPE&> )); \
			_local(0).reset(new stage::SourceInfoContext( \
					getParserContext().debug.line, \
					getParserContext().debug.column)); \
		}
#define BIND_CACHED_LOCATION(x) \
		{ \
			BOOST_MPL_ASSERT(( boost::is_same<_local_t(0), LOCATION_TYPE&> )); \
			if(_local(0)) \
				stage::SourceInfoContext::set((x), new stage::SourceInfoContext(*(_local(0).get()))); \
		}

using namespace zillians::language::tree;

namespace zillians { namespace language { namespace action { namespace detail {

template<typename T>
struct is_fusion_vector : boost::is_base_of<boost::fusion::sequence_base<T>, T>
{ };

template<typename T>
struct is_std_vector : boost::mpl::false_
{ };

template<typename _Tp, typename _Alloc>
struct is_std_vector<std::vector<_Tp, _Alloc>> : boost::mpl::true_
{ };

template<int N, typename Attribute, bool IsFusionVector>
struct attribute_accessor_impl;

template<int N, typename Attribute>
struct attribute_accessor_impl<N, Attribute, true>
{
	typedef typename boost::add_reference<
	            typename boost::mpl::at_c<typename Attribute::types, N>::type
	        >::type result_type;

	static result_type get(Attribute& attribute)
	{
		return boost::fusion::at_c<N>(attribute);
	}
};

template<int N, typename Attribute>
struct attribute_accessor_impl<N, Attribute, false>
{
	typedef typename boost::add_reference<Attribute>::type result_type;
	static result_type get(Attribute& attribute)
	{
		BOOST_MPL_ASSERT(( boost::mpl::bool_<N == 0> ));
		return attribute;
	}
};

template<int N, typename Attribute>
struct attribute_accessor
{
	typedef typename attribute_accessor_impl<N, Attribute, is_fusion_vector<Attribute>::value>::result_type result_type;
	static result_type get(Attribute& attribute)
	{
		return attribute_accessor_impl<N, Attribute, is_fusion_vector<Attribute>::value>::get(attribute);
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DETAIL_SEMANTICACTIONSDETAIL_H_ */
