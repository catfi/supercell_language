/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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
/**
 * @date Aug 5, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_VARIABLEDECL_H_
#define ZILLIANS_LANGUAGE_TREE_VARIABLEDECL_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"

namespace zillians { namespace language { namespace tree {

struct VariableDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(VariableDecl, (VariableDecl)(Declaration)(ASTNode));

	explicit VariableDecl(Identifier* name, TypeSpecifier* type, bool is_member, bool is_static, bool is_const, Declaration::VisibilitySpecifier::type visibility, ASTNode* initializer = NULL) : Declaration(name), type(type), is_member(is_member), is_static(is_static), is_const(is_const), visibility(visibility), initializer(initializer)
	{
		BOOST_ASSERT(name && "null variable name is not allowed");

		if(type) type->parent = this;
		if(initializer) initializer->parent = this;
	}

	void setInitializer(ASTNode* init)
	{
		if(initializer) initializer->parent = NULL;
		initializer = init;
		if(initializer) initializer->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Declaration)
		COMPARE_MEMBER(type)
		COMPARE_MEMBER(is_member)
		COMPARE_MEMBER(is_static)
		COMPARE_MEMBER(is_const)
		COMPARE_MEMBER(visibility)
		COMPARE_MEMBER(initializer)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to)
    {
    	BEGIN_REPLACE_WITH_BASE(Declaration)
		REPLACE_USE_WITH(type)
		REPLACE_USE_WITH(initializer)
    	END_REPLACE()
    }

	TypeSpecifier* type;
	bool is_member;
	bool is_static;
	bool is_const;
	Declaration::VisibilitySpecifier::type visibility;
	ASTNode* initializer;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_VARIABLEDECL_H_ */
