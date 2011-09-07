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

#ifndef ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_
#define ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_

#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/declaration/Declaration.h"
#include "language/tree/declaration/FunctionDecl.h"
#include "language/tree/declaration/VariableDecl.h"
#include "language/tree/declaration/TypedefDecl.h"

namespace zillians { namespace language { namespace tree {

struct ClassDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ClassDecl, (ClassDecl)(Declaration)(ASTNode));

	explicit ClassDecl(Identifier* name) : name(name), base(NULL)
	{
		BOOST_ASSERT(name && "null class name identifier is not allowed");

		name->parent = this;
	}

	void addFunction(FunctionDecl* func)
	{
		BOOST_ASSERT(name && "null member function declaration is not allowed");

		func->parent = this;
		member_functions.push_back(func);
	}

	void addVariable(VariableDecl* var)
	{
		BOOST_ASSERT(name && "null member variable declaration is not allowed");

		var->parent = this;
		member_variables.push_back(var);
	}

	void setBase(TypeSpecifier* extends_from)
	{
		BOOST_ASSERT(extends_from && "null base class is not allowed");

		extends_from->parent = this;

		if(base)
			base->parent = NULL;

		base = extends_from;
	}

	void addInterface(TypeSpecifier* interface)
	{
		BOOST_ASSERT(interface && "null interface is not allowed");

		interface->parent = this;
		implements.push_back(interface);
	}

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const ClassDecl* p = cast<const ClassDecl>(&rhs);
        if (p == NULL) return false;
        // compare base class
        if (!Declaration::isEqual(*p, visited)) return false;

        // compare data member
        if (!isASTNodeMemberEqual(&ClassDecl::name            , *this, *p, visited)) return false;
        if (!isASTNodeMemberEqual(&ClassDecl::base            , *this, *p, visited)) return false;
        if (!isVectorMemberEqual (&ClassDecl::implements      , *this, *p, visited)) return false;
        if (!isVectorMemberEqual (&ClassDecl::member_functions, *this, *p, visited)) return false;
        if (!isVectorMemberEqual (&ClassDecl::member_variables, *this, *p, visited)) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Declaration>(*this) ;
        // NOTE: name is serialized in save_construct_data() function
        // see http://www.boost.org/doc/libs/1_47_0/libs/serialization/doc/serialization.html#constructors
        //ar & name;
        ar & base;
        ar & implements;
        ar & member_functions;
        ar & member_variables;
    }

	Identifier* name;
	TypeSpecifier* base;
	std::vector<TypeSpecifier*> implements;
	std::vector<FunctionDecl*> member_functions;
	std::vector<VariableDecl*> member_variables;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ClassDecl* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ClassDecl* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	Identifier* name;
	ar >> name;
	::new(p) ClassDecl(name);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_CLASSDECL_H_ */
