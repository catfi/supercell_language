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

#ifndef ZILLIANS_LANGUAGE_ACTION_BASIC_SPECIFIERACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_BASIC_SPECIFIERACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

namespace zillians { namespace language { namespace action {

struct init_specifier
{
	DEFINE_ATTRIBUTES(Expression*)
	DEFINE_LOCALS()
};

struct type_specifier
{
	DEFINE_ATTRIBUTES(TypeSpecifier*)
	DEFINE_LOCALS()
};

struct thor_type
{
	DEFINE_ATTRIBUTES(TypeSpecifier*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_type)
	{
#ifdef DEBUG
		printf("thor_type::init_type param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("thor_type::init_type param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		Identifier* ident = NULL;
		if(_param(1).is_initialized())
		{
			BIND_CACHED_LOCATION(ident = new TemplatedIdentifier(TemplatedIdentifier::Usage::ACTUAL_ARGUMENT, _param(0)));
			deduced_foreach_value(i, *_param(1))
				cast<TemplatedIdentifier>(ident)->append(new TypenameDecl(new SimpleIdentifier(L"_"), i, NULL));
		}
		else
			ident = _param(0);
		BIND_CACHED_LOCATION(_result = new TypeSpecifier(ident));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init_primitive_type, PrimitiveType::type Type)
	{
		BIND_CACHED_LOCATION(_result = new TypeSpecifier(Type));
	}
	END_ACTION

	BEGIN_ACTION(init_function_type)
	{
#ifdef DEBUG
		printf("thor_type::init_function_type param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("thor_type::init_function_type param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		typedef std::vector<TypeSpecifier*> type_list_t;
		type_list_t*   parameters    = _param(0).is_initialized() ? &*_param(0) : NULL;
		TypeSpecifier* type          = _param(1).is_initialized() ? *_param(1) : NULL;
		FunctionType*  function_type = new FunctionType(); BIND_CACHED_LOCATION(function_type);
		if(parameters)
		{
			deduced_foreach_value(i, *parameters)
				function_type->appendParameterType(i);
		}
		function_type->setReturnType(type);
		BIND_CACHED_LOCATION(_result = new TypeSpecifier(function_type));
	}
	END_ACTION

	BEGIN_ACTION(init_ellipsis)
	{
#ifdef DEBUG
		printf("thor_type::init_ellipsis param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new TypeSpecifier(PrimitiveType::VARIADIC_ELLIPSIS));
	}
	END_ACTION
};

struct type_list
{
	DEFINE_ATTRIBUTES(std::vector<TypeSpecifier*>)
	DEFINE_LOCALS()
};

struct class_member_visibility
{
	DEFINE_ATTRIBUTES(Declaration::VisibilitySpecifier::type)
	DEFINE_LOCALS()

	BEGIN_TEMPLATED_ACTION(init, Declaration::VisibilitySpecifier::type Type)
	{
		_result = Type;
	}
	END_ACTION
};

struct annotation_list
{
	DEFINE_ATTRIBUTES(Annotations*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("annotation_list param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new Annotations());
		deduced_foreach_value(i, _param(0))
			_result->appendAnnotation(i);
	}
	END_ACTION
};

struct annotation
{
	DEFINE_ATTRIBUTES(Annotation*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("annotation::init param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("annotation::init param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		SimpleIdentifier* name = _param(0);
		if(_param(1).is_initialized())
		{
			_result = *_param(1);
			_result->name = name;
		}
		else
			BIND_CACHED_LOCATION(_result = new Annotation(name));
	}
	END_ACTION
};

struct annotation_body
{
	DEFINE_ATTRIBUTES(Annotation*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("annotation_body::init param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new Annotation(NULL));
		deduced_foreach_value(i, _param(0))
		{
			SimpleIdentifier* key = boost::fusion::at_c<0>(i);
			ASTNode* value = NULL;
			switch(boost::fusion::at_c<1>(i).which())
			{
			case 0: value = boost::get<Expression*>(boost::fusion::at_c<1>(i)); break;
			case 1: value = boost::get<Annotation*>(boost::fusion::at_c<1>(i)); break;
			}
			_result->appendKeyValue(key, value);
		}
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_SPECIFIERACTIONS_H_ */
