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

#ifndef ZILLIANS_LANGUAGE_ACTION_DECLARATION_DECLARATIONACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_DECLARATION_DECLARATIONACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"
#include "language/action/basic/BasicActions.h"

namespace zillians { namespace language { namespace action {

struct global_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("global_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("global_decl param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		_result = _param(1);
		if(_param(0).is_initialized())
			_result->setAnnotation(*_param(0));
	}
	END_ACTION
};

struct variable_decl_stem
{
	DEFINE_ATTRIBUTES(VariableDecl*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("variable_decl_stem param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("variable_decl_stem param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		Identifier*                            name        = _param(0);
		TypeSpecifier*                         type        = _param(1).is_initialized() ? *_param(1) : NULL;
		Expression*                            initializer = NULL;
		Declaration::VisibilitySpecifier::type visibility  = Declaration::VisibilitySpecifier::PUBLIC;
		bool                                   is_member   = false;
		bool                                   is_static   = false;
		bool                                   is_const    = false;
		BIND_CACHED_LOCATION(_result = new VariableDecl(name, type, is_member, is_static, is_const, visibility, initializer));
	}
	END_ACTION
};

struct param_decl_with_init
{
	DEFINE_ATTRIBUTES(VariableDecl*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("param_decl_with_init param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("param_decl_with_init param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		_result = _param(0);
		if(_param(1).is_initialized())
			_result->setInitializer(*_param(1));
	}
	END_ACTION
};

struct variable_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()
};

struct const_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("const_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("const_decl param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		_result = _param(0);
		cast<VariableDecl>(_result)->is_const = true;
		cast<VariableDecl>(_result)->setInitializer(_param(1));
	}
	END_ACTION
};

struct function_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("function_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("function_decl param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("function_decl param(2) type = %s\n", typeid(_param_t(2)).name());
		printf("function_decl param(3) type = %s\n", typeid(_param_t(3)).name());
#endif
		Identifier* name = NULL;
		switch(_param(0).which())
		{
		case 0:
			name = boost::get<Identifier*>(_param(0));
			break;
		case 1:
			BIND_CACHED_LOCATION(name = new SimpleIdentifier(L"new"));
			break;
		}
		std::vector<VariableDecl*>*            parameters = _param(1).is_initialized() ? &(*_param(1)) : NULL;
		TypeSpecifier*                         type       = _param(2).is_initialized() ? *_param(2) : NULL;
		Block*                                 block      = _param(3).is_initialized() ? *_param(3) : NULL;
		Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;
		bool                                   is_member  = false;
		bool                                   is_static  = false;
		BIND_CACHED_LOCATION(_result = new FunctionDecl(name, type, is_member, is_static, visibility, block));
		if(parameters)
			deduced_foreach_value(i, *parameters)
				cast<FunctionDecl>(_result)->appendParameter(i);
	}
	END_ACTION
};

struct typedef_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("typedef_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("typedef_decl param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		BIND_CACHED_LOCATION(_result = new TypedefDecl(_param(0), _param(1)));
	}
	END_ACTION
};

struct class_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("class_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("class_decl param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("class_decl param(2) type = %s\n", typeid(_param_t(2)).name());
		printf("class_decl param(3) type = %s\n", typeid(_param_t(3)).name());
#endif
		Identifier* name = _param(0);
		Identifier* extends_from_ident = _param(1).is_initialized() ? *_param(1) : NULL;
		TypeSpecifier* extends_from = NULL;
		if(extends_from_ident)
			BIND_CACHED_LOCATION(extends_from = new TypeSpecifier(extends_from_ident));
		BIND_CACHED_LOCATION(_result = new ClassDecl(name));
		if(extends_from)
			cast<ClassDecl>(_result)->setBase(extends_from);
		if(_param(2).is_initialized())
			deduced_foreach_value(i, *_param(2))
			{
				TypeSpecifier* type = new TypeSpecifier(i); BIND_CACHED_LOCATION(type);
				cast<ClassDecl>(_result)->addInterface(type);
			}
		if(_param(3).is_initialized())
			deduced_foreach_value(i, *_param(3))
				if(isa<VariableDecl>(i))
				{
					cast<ClassDecl>(_result)->addVariable(cast<VariableDecl>(i));
					cast<VariableDecl>(i)->is_member = true;
				}
				else if(isa<FunctionDecl>(i))
				{
					cast<ClassDecl>(_result)->addFunction(cast<FunctionDecl>(i));
					cast<FunctionDecl>(i)->is_member = true;
				}
	}
	END_ACTION
};

struct class_member_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("class_member_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("class_member_decl param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("class_member_decl param(2) type = %s\n", typeid(_param_t(2)).name());
		printf("class_member_decl param(3) type = %s\n", typeid(_param_t(3)).name());
#endif
		Annotations*                           annotation_list = _param(0).is_initialized() ? *_param(0) : NULL;
		Declaration::VisibilitySpecifier::type visibility      = _param(1).is_initialized() ? *_param(1) : Declaration::VisibilitySpecifier::DEFAULT;
		bool                                   is_static       = _param(2).is_initialized();
		_result = _param(3);
		_result->setAnnotation(annotation_list);
		if(isa<VariableDecl>(_result))
		{
			cast<VariableDecl>(_result)->visibility = visibility;
			cast<VariableDecl>(_result)->is_static  = is_static;
		}
		else if(isa<FunctionDecl>(_result))
		{
			cast<FunctionDecl>(_result)->visibility = visibility;
			cast<FunctionDecl>(_result)->is_static  = is_static;
		}
	}
	END_ACTION
};

struct interface_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("interface_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("interface_decl param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		BIND_CACHED_LOCATION(_result = new InterfaceDecl(_param(0)));
		deduced_foreach_value(i, _param(1))
		{
			cast<InterfaceDecl>(_result)->addFunction(cast<FunctionDecl>(i));
			cast<FunctionDecl>(i)->is_member = true;
		}
	}
	END_ACTION
};

struct interface_member_function_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("interface_member_function_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("interface_member_function_decl param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("interface_member_function_decl param(2) type = %s\n", typeid(_param_t(2)).name());
		printf("interface_member_function_decl param(3) type = %s\n", typeid(_param_t(3)).name());
		printf("interface_member_function_decl param(4) type = %s\n", typeid(_param_t(4)).name());
#endif
		Annotations*                           annotation_list = _param(0).is_initialized() ? *_param(0) : NULL;
		Declaration::VisibilitySpecifier::type visibility      = _param(1).is_initialized() ? *_param(1) : Declaration::VisibilitySpecifier::DEFAULT;
		std::vector<VariableDecl*>*            parameters      = _param(3).is_initialized() ? &(*_param(3)) : NULL;
		bool                                   is_member       = false;
		bool                                   is_static       = false;
		BIND_CACHED_LOCATION(_result = new FunctionDecl(_param(2), _param(4), is_member, is_static, visibility));
		_result->setAnnotation(annotation_list);
		if(parameters)
			deduced_foreach_value(i, *parameters)
				cast<FunctionDecl>(_result)->appendParameter(i);
	}
	END_ACTION
};

struct enum_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("enum_decl param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("enum_decl param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		BIND_CACHED_LOCATION(_result = new EnumDecl(_param(0)));
		deduced_foreach_value(i, _param(1))
		{
			SimpleIdentifier*             tag             = boost::fusion::at_c<0>(i);
			boost::optional<Expression*> &optional_result = boost::fusion::at_c<1>(i);
			Expression*                   value           = optional_result.is_initialized() ? *optional_result : NULL;
			cast<EnumDecl>(_result)->addEnumeration(tag, value);
		}
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_DECLARATIONACTIONS_H_ */
