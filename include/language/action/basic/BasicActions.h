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

struct block
{
	DEFINE_ATTRIBUTES(Block*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("block attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new Block();
		_value->appendObjects(_attr(0));
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
		printf("typed_parameter_list attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value.reset(new value_t);
		BOOST_FOREACH(auto i, (_attr(0)))
		{
			SimpleIdentifier* name = boost::fusion::at_c<0>(i);
			boost::optional<TypeSpecifier*> &optional_type = boost::fusion::at_c<1>(i);
			TypeSpecifier* type = optional_type.is_initialized() ? *optional_type : NULL;
			_value->push_back(value_t::value_type(name, type));
		}
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_BASICACTIONS_H_ */
