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

	explicit VariableDecl(Identifier* name, TypeSpecifier* type, bool is_member, bool is_static, bool is_const, Declaration::VisibilitySpecifier::type visibility, ASTNode* initializer = NULL) : name(name), type(type), is_member(is_member), is_static(is_static), is_const(is_const), visibility(visibility), initializer(initializer)
	{
		BOOST_ASSERT(name && "null variable name is not allowed");

		name->parent = this;
		if(type) type->parent = this;
		if(initializer) initializer->parent = this;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if(visited.count(this))
        {
            return true ;
        }

        const VariableDecl* p = cast<const VariableDecl>(&rhs);
        if(p == NULL)
        {
            return false;
        }

        // compare base class
        if(!Declaration::isEqualImpl(*p, visited))
        {
            return false;
        }

        // compare data member
        if(!isASTNodeMemberEqual   (&VariableDecl::name            , *this, *p, visited)) return false;
        if(!isASTNodeMemberEqual   (&VariableDecl::type            , *this, *p, visited)) return false;
        if(is_member  != p->is_member                                                   ) return false;
        if(is_static  != p->is_static                                                   ) return false;
        if(is_const   != p->is_const                                                    ) return false;
        if(visibility != p->visibility                                                  ) return false;
        if(!isASTNodeMemberEqual   (&VariableDecl::initializer     , *this, *p, visited)) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        boost::serialization::base_object<Declaration>(*this);
    }

	Identifier* name;
	TypeSpecifier* type;
	bool is_member;
	bool is_static;
	bool is_const;
	Declaration::VisibilitySpecifier::type visibility;
	ASTNode* initializer;
};

} } }

namespace boost { namespace serialization {

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::VariableDecl* p, const unsigned int file_version)
{
	ar << p->name;
    ar << p->type;
    ar << p->is_member;
    ar << p->is_static;
    ar << p->is_const;
    ar << p->visibility;
    ar << p->initializer;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::VariableDecl* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	Identifier* name;
	TypeSpecifier* type;
	bool is_member;
	bool is_static;
	bool is_const;
	Declaration::VisibilitySpecifier::type visibility;
	ASTNode* initializer;

	ar >> name;
    ar >> type;
    ar >> is_member;
    ar >> is_static;
    ar >> is_const;
    ar >> visibility;
    ar >> initializer;

	::new(p) VariableDecl(name, type, is_member, is_static, is_const, visibility, initializer);
}

} } // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_VARIABLEDECL_H_ */
