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

	BEGIN_ACTION(init)
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
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("block param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		_result = new Block();
		_result->appendObjects(_param(0));
	}
	END_ACTION
};

struct typed_parameter_list
{
	typedef std::vector<std::pair<SimpleIdentifier*, TypeSpecifier*>> value_t;
	DEFINE_ATTRIBUTES(shared_ptr<value_t>)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("typed_parameter_list param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		_result.reset(new value_t);
		deduced_foreach_value(i, _param(0))
		{
			SimpleIdentifier*                name          = boost::fusion::at_c<0>(i);
			boost::optional<TypeSpecifier*> &optional_type = boost::fusion::at_c<1>(i);
			TypeSpecifier* type = optional_type.is_initialized() ? *optional_type : NULL;
			_result->push_back(value_t::value_type(name, type));
		}
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_BASICACTIONS_H_ */
