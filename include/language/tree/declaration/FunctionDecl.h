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

#ifndef ZILLIANS_LANGUAGE_TREE_FUNCTIONDECL_H_
#define ZILLIANS_LANGUAGE_TREE_FUNCTIONDECL_H_

#include <boost/preprocessor.hpp>
#include "language/tree/declaration/Declaration.h"
#include "language/tree/declaration/VariableDecl.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/basic/Block.h"

namespace zillians { namespace language { namespace tree {

struct FunctionDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(FunctionDecl, (FunctionDecl)(Declaration)(ASTNode));

	explicit FunctionDecl(Identifier* name, TypeSpecifier* type, bool is_member, bool is_static, Declaration::VisibilitySpecifier::type visibility, Block* block = NULL) : name(name), type(type), is_member(is_member), is_static(is_static), visibility(visibility), block(block)
	{
		if(name) name->parent = this;
		if(type) type->parent = this;
		if(block) block->parent = this;
	}

	void appendParameter(VariableDecl* parameter_decl)
	{
		parameter_decl->parent = this;
		parameters.push_back(parameter_decl);
	}

	void appendParameter(SimpleIdentifier* name, TypeSpecifier* type = NULL, ASTNode* initializer = NULL)
	{
		VariableDecl* parameter_decl = new VariableDecl(name, type, false, false, false, Declaration::VisibilitySpecifier::DEFAULT, initializer);
		parameter_decl->parent = this;
		parameters.push_back(parameter_decl);
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Declaration)
		COMPARE_MEMBER(name)
        COMPARE_MEMBER(parameters)
        COMPARE_MEMBER(type)
		COMPARE_MEMBER(is_member)
		COMPARE_MEMBER(is_static)
		COMPARE_MEMBER(visibility)
		COMPARE_MEMBER(block)
		END_COMPARE()
    }

	Identifier* name;
	std::vector<VariableDecl*> parameters;
	TypeSpecifier* type;
	bool is_member;
	bool is_static;
	Declaration::VisibilitySpecifier::type visibility;
	Block* block;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_FUNCTIONDECL_H_ */
