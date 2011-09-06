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

#ifndef ZILLIANS_LANGUAGE_ACTION_BASIC_LITERALACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_BASIC_LITERALACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

namespace zillians { namespace language { namespace action {

struct integer_literal
{
	DEFINE_ATTRIBUTES(NumericLiteral*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_loc)
	{
		CACHE_LOCATION;
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("integer_literal param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new NumericLiteral(_param(0)));
	}
	END_ACTION
};

struct float_literal
{
	DEFINE_ATTRIBUTES(NumericLiteral*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_loc)
	{
		CACHE_LOCATION;
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("float_literal param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new NumericLiteral(_param(0)));
	}
	END_ACTION
};

struct string_literal
{
	DEFINE_ATTRIBUTES(StringLiteral*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_loc)
	{
		CACHE_LOCATION;
	}
	END_ACTION

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("string_literal param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new StringLiteral(_param(0)));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_LITERALACTIONS_H_ */
