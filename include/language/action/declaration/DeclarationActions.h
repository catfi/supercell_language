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
		printf("declaration attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("declaration attr(1) type = %s\n", typeid(_attr_t(1)).name());
		_value = _attr(1);
		if(_attr(0).is_initialized())
			_value->setAnnotation(*_attr(0));
	}
	END_ACTION
};

struct const_variable_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("const_variable_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("const_variable_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		_value = _attr(1);
		if(_attr(0).is_initialized())
			dynamic_cast<VariableDecl*>(_value)->storage = Declaration::StorageSpecifier::CONST;
	}
	END_ACTION
};

struct variable_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("variable_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("variable_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		_value = _attr(0);
		ASTNode* initializer = _attr(1).is_initialized() ? *_attr(1) : NULL;
		dynamic_cast<VariableDecl*>(_value)->initializer = initializer;
	}
	END_ACTION
};

struct variable_decl_stem
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("variable_decl_stem attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("variable_decl_stem attr(1) type = %s\n", typeid(_attr_t(1)).name());
		Identifier*                            name        = _attr(0);
		TypeSpecifier*                         type        = _attr(1).is_initialized() ? *_attr(1) : NULL;
		ASTNode*                               initializer = NULL;
		Declaration::VisibilitySpecifier::type visibility  = Declaration::VisibilitySpecifier::PUBLIC;
		Declaration::StorageSpecifier::type    storage     = Declaration::StorageSpecifier::NONE;
		bool                                   is_member   = false;
		_value = new VariableDecl(
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
		printf("function_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("function_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("function_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("function_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
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
		_value = new FunctionDecl(name, type, is_member, visibility, storage, block);
		if(!!parameters)
			for(typed_parameter_list::value_t::iterator i = parameters->begin(); i != parameters->end(); i++)
				dynamic_cast<FunctionDecl*>(_value)->appendParameter((*i).first, (*i).second);
	}
	END_ACTION
};

struct typedef_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("typedef_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

struct class_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("class_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("class_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("class_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("class_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
		Identifier* name = _attr(0);
		TypeSpecifier* extends_from = _attr(1).is_initialized() ? new TypeSpecifier(*_attr(1)) : NULL;
		_value = new ClassDecl(name);
		if(!!extends_from)
			dynamic_cast<ClassDecl*>(_value)->setBase(extends_from);
		if(_attr(2).is_initialized())
			foreach(i, *_attr(2))
				dynamic_cast<ClassDecl*>(_value)->addInterface(new TypeSpecifier(*i));
		foreach(i, _attr(3))
		{
			if(isa<VariableDecl>(*i))
			{
				dynamic_cast<ClassDecl*>(_value)->addVariable(dynamic_cast<VariableDecl*>(*i));
				dynamic_cast<VariableDecl*>(*i)->is_member = true;
			}
			else if(isa<FunctionDecl>(*i))
			{
				dynamic_cast<ClassDecl*>(_value)->addFunction(dynamic_cast<FunctionDecl*>(*i));
				dynamic_cast<FunctionDecl*>(*i)->is_member = true;
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
		printf("class_member_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("class_member_decl attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("class_member_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("class_member_decl attr(3) type = %s\n", typeid(_attr_t(3)).name());
		Annotations*                           annotations = _attr(0).is_initialized() ? *_attr(0) : NULL;
		Declaration::VisibilitySpecifier::type visibility  = _attr(1).is_initialized() ? *_attr(1) : Declaration::VisibilitySpecifier::DEFAULT;
		Declaration::StorageSpecifier::type    storage     = _attr(2).is_initialized() ? *_attr(2) : Declaration::StorageSpecifier::NONE;
		_value = _attr(3);
		dynamic_cast<Declaration*>(_value)->setAnnotation(annotations);
		if(isa<VariableDecl>(_value))
		{
			dynamic_cast<VariableDecl*>(_value)->visibility = visibility;
			dynamic_cast<VariableDecl*>(_value)->storage = storage;
		}
		else if(isa<FunctionDecl>(_value))
		{
			dynamic_cast<FunctionDecl*>(_value)->visibility = visibility;
			dynamic_cast<FunctionDecl*>(_value)->storage = storage;
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
		printf("interface_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

struct member_function_decl_without_body
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("member_function_decl_without_body attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

struct enum_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("enum_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_DECLARATIONACTIONS_H_ */
