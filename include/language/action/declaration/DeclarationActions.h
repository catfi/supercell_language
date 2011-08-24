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

namespace zillians { namespace language { namespace action {

struct declaration
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("declaration attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("declaration attr(1) type = %s\n", typeid(_attr_t(1)).name());
		if(_attr(1).which() == 0)
		{
			typedef boost::fusion::vector2<boost::optional<bool>, zillians::language::tree::Declaration*> fusion_vector_t;
			fusion_vector_t vec = boost::get<fusion_vector_t>(_attr(1));
			bool is_const = boost::fusion::at_c<0>(vec).is_initialized();
			Declaration* decl = boost::fusion::at_c<1>(vec);
			if(is_const)
				dynamic_cast<VariableDecl*>(decl)->storage = Declaration::StorageSpecifier::CONST;
			_value = decl;
		}
		else
			_value = boost::get<zillians::language::tree::Declaration*>(_attr(1));
		if(_attr(0).is_initialized())
			_value->setAnnotation(*_attr(0));
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
		printf("variable_decl attr(2) type = %s\n", typeid(_attr_t(2)).name());
		Identifier*    name        = _attr(0);
		TypeSpecifier* type        = _attr(1).is_initialized() ? *_attr(1) : NULL;
		ASTNode*       initializer = NULL;
		if(_attr(2).is_initialized())
			initializer = *_attr(2);
		Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;
		Declaration::StorageSpecifier::type    storage    = Declaration::StorageSpecifier::NONE;
		bool is_member = false;
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
//		Identifier*                            name       = NULL;//_attr(0).is_initialized() ? *_attr(0) : NULL;
//		ASTNode*                               type       = NULL;//_attr(0).is_initialized() ? *_attr(0) : NULL;
//		Block*                                 block      = NULL;//_attr(0).is_initialized() ? *_attr(0) : NULL;
//		Declaration::VisibilitySpecifier::type visibility = Declaration::VisibilitySpecifier::PUBLIC;
//		Declaration::StorageSpecifier::type    storage    = Declaration::StorageSpecifier::NONE;
//		bool is_member = false;
//		_value = new FunctionDecl(name, type, is_member, visibility, storage, block);
	}
	END_ACTION
};

struct typedef_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
	}
	END_ACTION
};

struct class_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
	}
	END_ACTION
};

struct interface_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
	}
	END_ACTION
};

struct enum_decl
{
	DEFINE_ATTRIBUTES(Declaration*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_DECLARATION_DECLARATIONACTIONS_H_ */
