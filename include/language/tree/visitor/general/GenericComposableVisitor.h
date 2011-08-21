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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICCOMPOSABLEVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICCOMPOSABLEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include <boost/fusion/container.hpp>
#include <boost/fusion/support/detail/access.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace zillians { namespace language { namespace tree { namespace visitor {

static const bool Composable = true;
static const bool Standalone = false;

template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(FUSION_MAX_VECTOR_SIZE, typename T, boost::fusion::void_)>
struct GenericComposableVisitor : GenericDoubleVisitor, boost::fusion::vector<BOOST_PP_ENUM_PARAMS(FUSION_MAX_VECTOR_SIZE, T)>
{
	typedef boost::fusion::vector<BOOST_PP_ENUM_PARAMS(FUSION_MAX_VECTOR_SIZE, T)> base_type;

	CREATE_INVOKER(applyInvoker, apply);

	// the following code uses boost PP to create multiple version of constructor
	// GenericComposableVisitor(T0)
	// GenericComposableVisitor(T0, T1)
	// GenericComposableVisitor(T0, T1, T2, ...)
	#include "language/tree/visitor/general/detail/GenericComposableVisitorCtor.h"

	struct invoke_visit
	{
		invoke_visit(const ASTNode& node) : node(node) { }

	    template<typename VisitorImpl>
	    void operator()(VisitorImpl& visitor) const
	    {
	        visitor.visit(node);
	    }

	    const ASTNode& node;
	};

	void apply(const ASTNode& node)
	{
		boost::fusion::for_each(*this, invoke_visit(node));
		revisit(node);
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICCOMPOSABLEVISITOR_H_ */
