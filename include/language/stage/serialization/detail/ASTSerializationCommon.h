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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_ASTSERIALIZATIONCOMMON_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_ASTSERIALIZATIONCOMMON_H_

#include "core/Prerequisite.h"
#include "language/context/ResolverContext.h"
#include "language/context/TransformerContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "core/ContextHubSerialization.h"
#include "language/tree/ASTNodeSerialization.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace zillians { namespace language { namespace stage { namespace visitor {

// this defines all context object types needed to be de-serialized
typedef ContextHubSerialization<
			boost::mpl::vector<
				ResolvedType,
				ResolvedSymbol,
				ResolvedPackage,
				SplitReferenceContext,
				SourceInfoContext,
				NameManglingContext,
				TypeIdManglingContext,
				SymbolIdManglingContext,
				InstantiatedFrom
				> > FullSerializer;

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_ASTSERIALIZATIONCOMMON_H_ */
