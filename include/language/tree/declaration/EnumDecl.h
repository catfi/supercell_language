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

#ifndef ZILLIANS_LANGUAGE_TREE_ENUMDECL_H_
#define ZILLIANS_LANGUAGE_TREE_ENUMDECL_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct EnumDecl : public Declaration
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(EnumDecl, (EnumDecl)(Declaration)(ASTNode));

	explicit EnumDecl(Identifier* name) : name(name)
	{
		BOOST_ASSERT(name && "null enumeration name is not allowed");
	}

	void addEnumeration(SimpleIdentifier* tag, Expression* value = NULL)
	{
		BOOST_ASSERT(tag && "null tag for enumeration is not allowed");

		tag->parent = this;
		if(value) value->parent = this;
		enumeration_list.push_back(std::make_pair(tag, value));
	}

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const EnumDecl* p = cast<const EnumDecl>(&rhs);
        if (p == NULL) return false;
        // compare base class
        if (!Declaration::isEqual(*p, visited)) return false;

        // compare data member
        if (!isASTNodeMemberEqual   (&EnumDecl::name            , *this, *p, visited)) return false;
        if (!isPairVectorMemberEqual(&EnumDecl::enumeration_list, *this, *p, visited)) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Declaration>(*this);
        //ar & name;
        ar & enumeration_list;
    }

	Identifier* name;
	std::vector<std::pair<SimpleIdentifier*, Expression*>> enumeration_list;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::EnumDecl* p, const unsigned int file_version)
{
	ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::EnumDecl* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	Identifier* name;
	ar >> name;
	::new(p) EnumDecl(name);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_ENUMDECL_H_ */
