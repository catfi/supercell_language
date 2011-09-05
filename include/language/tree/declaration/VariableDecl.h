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

	explicit VariableDecl(Identifier* name, TypeSpecifier* type, bool is_member, Declaration::VisibilitySpecifier::type visibility, Declaration::StorageSpecifier::type storage, ASTNode* initializer = NULL) : name(name), type(type), is_member(is_member), visibility(visibility), storage(storage), initializer(initializer)
	{
		BOOST_ASSERT(name && "null variable name is not allowed");

		name->parent = this;
		if(type) type->parent = this;
		if(initializer) initializer->parent = this;
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::base_object<Declaration>(*this);
        ar & name;
        ar & type;
        ar & is_member;
        ar & static_cast<int&>(visibility);
        ar & static_cast<int&>(storage);
        ar & initializer;
    }

	Identifier* name;
	TypeSpecifier* type;
	bool is_member;
	Declaration::VisibilitySpecifier::type visibility;
	Declaration::StorageSpecifier::type storage;
	ASTNode* initializer;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_VARIABLEDECL_H_ */
