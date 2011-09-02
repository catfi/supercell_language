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

struct declaration
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("declaration attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("declaration attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_result = _attr(1);
		if(_attr(0).is_initialized())
			_result->setAnnotation(*_attr(0));
	}
	END_ACTION
};

struct const_variable_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("const_variable_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("const_variable_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_result = _attr(1);
		if(_attr(0).is_initialized())
			cast<VariableDecl>(_result)->storage = Declaration::StorageSpecifier::CONST;
	}
	END_ACTION
};

struct variable_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("variable_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("variable_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_result = _attr(0);
		ASTNode* initializer = _attr(1).is_initialized() ? *_attr(1) : NULL;
		cast<VariableDecl>(_result)->initializer = initializer;
	}
	END_ACTION
};

struct variable_decl_stem
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("variable_decl_stem attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("variable_decl_stem attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		Identifier*                            name        = _attr(0);
		TypeSpecifier*                         type        = _attr(1).is_initialized() ? *_attr(1) : NULL;
		ASTNode*                               initializer = NULL;
		Declaration::VisibilitySpecifier::type visibility  = Declaration::VisibilitySpecifier::PUBLIC;
		Declaration::StorageSpecifier::type    storage     = Declaration::StorageSpecifier::NONE;
		bool                                   is_member   = false;
		_result = new VariableDecl(
				name, type, is_member, visibility, storage, initializer
				);
	}
	END_ACTION
};

struct function_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("function_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("function_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("function_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("function_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
#endif
		Identifier* name = NULL;
		switch(_attr(0).which())
		{
		case 0:
			name = boost::get<Identifier*>(_attr(0));
			break;
		case 1:
			name = new SimpleIdentifier(L"new");
			break;
		}
		typed_parameter_list::value_t*         parameters = _attr(1).is_initialized() ? (*_attr(1)).get() : NULL;
		TypeSpecifier*                         type       = _attr(2).is_initialized() ? *_attr(2) : NULL;
		Block*                                 block      = _attr(3).is_initialized() ? *_attr(3) : NULL;
		Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;
		Declaration::StorageSpecifier::type    storage    = Declaration::StorageSpecifier::NONE;
		bool                                   is_member  = false;
		_result = new FunctionDecl(name, type, is_member, visibility, storage, block);
		if(!!parameters)
			deduced_foreach_value(i, *parameters)
				cast<FunctionDecl>(_result)->appendParameter(i.first, i.second);
	}
	END_ACTION
};

struct typedef_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("typedef_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("typedef_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_result = new TypedefDecl(_attr(0), _attr(1));
	}
	END_ACTION
};

struct class_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("class_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("class_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("class_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("class_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
#endif
		Identifier* name = _attr(0);
		TypeSpecifier* extends_from = _attr(1).is_initialized() ? new TypeSpecifier(*_attr(1)) : NULL;
		_result = new ClassDecl(name);
		if(!!extends_from)
			cast<ClassDecl>(_result)->setBase(extends_from);
		if(_attr(2).is_initialized())
			deduced_foreach_value(i, *_attr(2))
				cast<ClassDecl>(_result)->addInterface(new TypeSpecifier(i));
		deduced_foreach_value(i, _attr(3))
		{
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
		printf("class_member_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("class_member_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("class_member_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("class_member_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
#endif
		Annotations*                           annotations = _attr(0).is_initialized() ? *_attr(0) : NULL;
		Declaration::VisibilitySpecifier::type visibility  = _attr(1).is_initialized() ? *_attr(1) : Declaration::VisibilitySpecifier::DEFAULT;
		Declaration::StorageSpecifier::type    storage     = _attr(2).is_initialized() ? *_attr(2) : Declaration::StorageSpecifier::NONE;
		_result = _attr(3);
		cast<Declaration>(_result)->setAnnotation(annotations);
		if(isa<VariableDecl>(_result))
		{
			cast<VariableDecl>(_result)->visibility = visibility;
			cast<VariableDecl>(_result)->storage = storage;
		}
		else if(isa<FunctionDecl>(_result))
		{
			cast<FunctionDecl>(_result)->visibility = visibility;
			cast<FunctionDecl>(_result)->storage = storage;
		}
	}
	END_ACTION
};

struct interface_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("interface_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("interface_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_result = new InterfaceDecl(_attr(0));
		deduced_foreach_value(i, _attr(1))
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
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("interface_member_function_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("interface_member_function_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("interface_member_function_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("interface_member_function_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
#endif
		Declaration::VisibilitySpecifier::type visibility = _attr(0).is_initialized() ? *_attr(0) : Declaration::VisibilitySpecifier::DEFAULT;
		typed_parameter_list::value_t*         parameters = _attr(2).is_initialized() ? (*_attr(2)).get() : NULL;
		Declaration::StorageSpecifier::type    storage    = Declaration::StorageSpecifier::NONE;
		bool                                   is_member  = false;
		_result = new FunctionDecl(_attr(1), _attr(3), is_member, visibility, storage, NULL);
		if(!!parameters)
			deduced_foreach_value(i, *parameters)
				cast<FunctionDecl>(_result)->appendParameter(i.first, i.second);
	}
	END_ACTION
};

struct enum_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("enum_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("enum_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_result = new EnumDecl(_attr(0));
		deduced_foreach_value(i, _attr(1))
		{
			boost::optional<Annotations*> &optional_annotations = boost::fusion::at_c<0>(i);
			SimpleIdentifier*              tag                  = boost::fusion::at_c<1>(i);
			boost::optional<Expression*>  &optional_result       = boost::fusion::at_c<2>(i);
			Annotations* annotations = optional_annotations.is_initialized() ? *optional_annotations : NULL;
			Expression*  value       = optional_result.is_initialized() ? *optional_result : NULL;
			cast<EnumDecl>(_result)->addEnumeration(tag, value);
		}
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_DECLARATIONACTIONS_H_ */
