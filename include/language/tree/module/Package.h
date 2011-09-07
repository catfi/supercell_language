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

#ifndef ZILLIANS_LANGUAGE_TREE_PACKAGE_H_
#define ZILLIANS_LANGUAGE_TREE_PACKAGE_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"
#include "utility/Foreach.h"

namespace zillians { namespace language { namespace tree {

/**
 * Package is used to represent the hierarchical structure of a program.
 * Every ASTNode, except Package and Program, must be contained by a Package.
 */
struct Package : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Package, (Package)(ASTNode));

	explicit Package(SimpleIdentifier* _id) : id(_id)
	{
		BOOST_ASSERT(_id && "null identifier for package node is not allowed");

		id->parent = this;
	}

	Package* findPackage(const std::wstring& name)
	{
		foreach(i, children)
		{
			if((*i)->id->toString().compare(name) == 0)
				return (*i);
		}
		return NULL;
	}

	void addPackage(Package* package)
	{
		BOOST_ASSERT(package && "null child package for package node is not allowed");

		package->parent = this;
		children.push_back(package);
	}

	void addObject(ASTNode* object)
	{
		BOOST_ASSERT(object && "null child object for package node is not allowed");

		object->parent = this;
		objects.push_back(object);
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<ASTNode>(*this);
        ar & children;
        ar & objects;
    }

	SimpleIdentifier* id;
	std::vector<Package*> children;
	std::vector<ASTNode*> objects;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::Package* p, const unsigned int file_version)
{
    ar << p->id;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::Package* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

    SimpleIdentifier* id;
    ar >> id;
	::new(p) Package(id);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_PACKAGE_H_ */
