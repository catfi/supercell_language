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

#ifndef ZILLIANS_LANGUAGE_ACTION_BASIC_BASICACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_BASIC_BASICACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"
#include <vector>
#include <utility>

namespace zillians { namespace language { namespace action {

struct location
{
	DEFINE_ATTRIBUTES(void)
	DEFINE_LOCALS()

	BEGIN_ACTION(cache_loc)
	{
		CACHE_LOCATION;
	}
	END_ACTION

	BEGIN_ACTION(init) // NOTE: _result_t will not be "void" if used by other SA
	{
#ifdef DEBUG
		printf("location param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		getParserContext().debug.line   = _param(0).get_position().line;
		getParserContext().debug.column = _param(0).get_position().column;
	}
	END_ACTION
};

struct block
{
	DEFINE_ATTRIBUTES(Block*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("block param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new Block());
		_result->appendObjects(_param(0));
	}
	END_ACTION
};

struct optional_brace_block
{
	DEFINE_ATTRIBUTES(ASTNode*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("optional_brace_block param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		if(isa<Block>(_param(0)))
			_result = _param(0);
		else if(isa<Statement>(_param(0)))
		{
			BIND_CACHED_LOCATION(_result = new Block());
			cast<Block>(_result)->appendObject(_param(0));
		}
	}
	END_ACTION
};

struct variable_decl_list
{
	DEFINE_ATTRIBUTES(std::vector<VariableDecl*>)
	DEFINE_LOCALS()
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_BASICACTIONS_H_ */
