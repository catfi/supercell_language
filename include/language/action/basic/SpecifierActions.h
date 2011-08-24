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

struct colon_type_specifier
{
	DEFINE_ATTRIBUTES(TypeSpecifier*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
//		printf("colon_type_specifier attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = _attr(0);
	}
	END_ACTION
};

struct type_specifier
{
	DEFINE_ATTRIBUTES(TypeSpecifier*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_type)
	{
		printf("type_specifier::init_type attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("type_specifier::init_type attr(1) type = %s\n", typeid(_attr_t(1)).name());
		if(_attr(1).is_initialized())
		{
			std::vector<TypeSpecifier*> &vec = *_attr(1);
			TemplatedIdentifier *templated_identifier = new TemplatedIdentifier(TemplatedIdentifier::Usage::ACTUAL_ARGUMENT, _attr(0));
			for(std::vector<TypeSpecifier*>::iterator p; p != vec.end(); p++)
				templated_identifier->appendArgument(*p);
			_value = new TypeSpecifier(templated_identifier);
		}
		else
			_value = new TypeSpecifier(_attr(0));
	}
	END_ACTION

	BEGIN_ACTION(init_function_type)
	{
		printf("type_specifier::init_function_type attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("type_specifier::init_function_type attr(1) type = %s\n", typeid(_attr_t(1)).name());
//		_value = new TypeSpecifier(new FunctionType());
	}
	END_ACTION

	BEGIN_ACTION(init_ellipsis)
	{
//		printf("type_specifier::init_ellipsis attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

struct template_arg_specifier
{
	DEFINE_ATTRIBUTES(std::vector<TypeSpecifier*>)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("template_specifier attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = _attr(0);
	}
	END_ACTION
};

struct type_list_specifier
{
	DEFINE_ATTRIBUTES(std::vector<TypeSpecifier*>)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("type_list_specifier attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = _attr(0);
	}
	END_ACTION
};

struct storage_specifier
{
	DEFINE_ATTRIBUTES(Declaration::StorageSpecifier::type)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_static)
	{
		_value = Declaration::StorageSpecifier::STATIC;
	}
	END_ACTION

	BEGIN_ACTION(init_const)
	{
		_value = Declaration::StorageSpecifier::CONST;
	}
	END_ACTION
};

struct visibility_specifier
{
	DEFINE_ATTRIBUTES(Declaration::VisibilitySpecifier::type)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_public)
	{
		_value = Declaration::VisibilitySpecifier::PUBLIC;
	}
	END_ACTION

	BEGIN_ACTION(init_protected)
	{
		_value = Declaration::VisibilitySpecifier::PROTECTED;
	}
	END_ACTION

	BEGIN_ACTION(init_private)
	{
		_value = Declaration::VisibilitySpecifier::PRIVATE;
	}
	END_ACTION
};

struct annotation_specifiers
{
	DEFINE_ATTRIBUTES(Annotations*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("annotation_specifiers attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new Annotations();
	}
	END_ACTION

	BEGIN_ACTION(append_annotation)
	{
		printf("annotation_specifiers::append_annotation attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value->appendAnnotation(_attr(0));
	}
	END_ACTION
};

struct annotation_specifier
{
	DEFINE_ATTRIBUTES(Annotation*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("annotation_specifier attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new Annotation(_attr(0));
	}
	END_ACTION

	BEGIN_ACTION(append_keyvalue)
	{
		printf("annotation_specifier::append_keyvalue attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("annotation_specifier::append_keyvalue attr(1) type = %s\n", typeid(_attr_t(1)).name());
		_value->appendKeyValue(_attr(0), _attr(1));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_BASIC_SPECIFIERACTIONS_H_ */
