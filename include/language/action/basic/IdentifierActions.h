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

#ifndef ZILLIANS_LANGUAGE_ACTION_BASIC_IDENTIFIERACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_BASIC_IDENTIFIERACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

namespace zillians { namespace language { namespace action {

struct identifier
{
	DEFINE_ATTRIBUTES(SimpleIdentifier*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("identifier param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new SimpleIdentifier(_param(0)));
	}
	END_ACTION
};

struct nested_identifier
{
	DEFINE_ATTRIBUTES(Identifier*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("nested_identifier param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("nested_identifier param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		if(_param(1).size() == 0)
			_result = _param(0);
		else
		{
			BIND_CACHED_LOCATION(_result = new NestedIdentifier());
			cast<NestedIdentifier>(_result)->appendIdentifier(_param(0));
			deduced_foreach_value(i, _param(1))
				cast<NestedIdentifier>(_result)->appendIdentifier(i);
		}
	}
	END_ACTION
};

struct template_arg_identifier
{
	DEFINE_ATTRIBUTES(Identifier*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("template_arg_identifier param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("template_arg_identifier param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		if(_param(1).is_initialized())
		{
			BIND_CACHED_LOCATION(_result = new TemplatedIdentifier(TemplatedIdentifier::Usage::ACTUAL_ARGUMENT, _param(0)));
			deduced_foreach_value(i, *_param(1))
				cast<TemplatedIdentifier>(_result)->append(new TypenameDecl(new SimpleIdentifier(L"_"), i, NULL));
		}
		else
			_result = _param(0);
	}
	END_ACTION
};

struct template_param_identifier
{
	DEFINE_ATTRIBUTES(Identifier*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("template_param_identifier param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("template_param_identifier param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		if(_param(1).is_initialized())
		{
			BIND_CACHED_LOCATION(_result = new TemplatedIdentifier(TemplatedIdentifier::Usage::FORMAL_PARAMETER, _param(0)));
			deduced_foreach_value(i, *(_param(1)))
			{
				SimpleIdentifier* ident            = boost::fusion::at_c<0>(i);
				TypeSpecifier*    specialized_type = NULL;
				Expression*       default_type     = NULL;
				if(boost::fusion::at_c<1>(i).is_initialized())
				{
					auto& optional_variant = *boost::fusion::at_c<1>(i);
					switch(optional_variant.which())
					{
					case 0: specialized_type = boost::get<TypeSpecifier*>(optional_variant); break;
					case 1: default_type     = boost::get<Expression*>(optional_variant); break;
					}
				}
				cast<TemplatedIdentifier>(_result)->append(new TypenameDecl(ident, specialized_type, default_type));
			}
		}
		else
		{
			_result = _param(0);
		}
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_IDENTIFIERACTIONS_H_ */
