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

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/basic/Block.h"

namespace zillians { namespace language { namespace tree {

struct FunctionDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(FunctionDecl, (FunctionDecl)(Declaration)(ASTNode));

	explicit FunctionDecl(Identifier* name, TypeSpecifier* type, bool is_member, Declaration::VisibilitySpecifier::type visibility, Declaration::StorageSpecifier::type storage, Block* block = NULL) : name(name), type(type), is_member(is_member), visibility(visibility), storage(storage), block( (block == NULL) ? new Block() : block )
	{
		if(name) name->parent = this;
		if(type) type->parent = this;
		if(block) block->parent = this;
	}

	void appendParameter(SimpleIdentifier* name, TypeSpecifier* type = NULL)
	{
		name->parent = this;
		if(type) type->parent = this;
		parameters.push_back(std::make_pair(name, type));
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::base_object<Declaration>(*this);
        ar & name;
        ar & parameters;
        ar & type;
        ar & is_member;
        ar & static_cast<int&>(visibility);
        ar & static_cast<int&>(storage);
        ar & block;
    }

	Identifier* name;
	std::vector<std::pair<SimpleIdentifier*, TypeSpecifier*>> parameters;
	TypeSpecifier* type;
	bool is_member;
	Declaration::VisibilitySpecifier::type visibility;
	Declaration::StorageSpecifier::type storage;
	Block* block;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_FUNCTIONDECL_H_ */
