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

#ifndef BOOST_PP_IS_ITERATING
#if !defined(ZILLIANS_COMPILER_TREE_VISITOR_GENERICCOMPOSABLEVISITORCTOR_H_)
#define ZILLIANS_COMPILER_TREE_VISITOR_GENERICCOMPOSABLEVISITORCTOR_H_

#include <boost/preprocessor/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#define BOOST_PP_FILENAME_1 \
    "language/tree/visitor/general/detail/GenericComposableVisitorCtor.h"
#define BOOST_PP_ITERATION_LIMITS (1, FUSION_MAX_VECTOR_SIZE)
#include BOOST_PP_ITERATE()

#endif
#else // defined(BOOST_PP_IS_ITERATING)

#define N BOOST_PP_ITERATION()

#if N == 1
    explicit
#endif
    GenericComposableVisitor(BOOST_PP_ENUM_BINARY_PARAMS(
        N, typename boost::fusion::detail::call_param<T, >::type _))
        : base_type(BOOST_PP_ENUM_PARAMS(N, _)) { REGISTER_ALL_VISITABLE_ASTNODE(applyInvoker) }

#undef N

#endif // defined(BOOST_PP_IS_ITERATING)
