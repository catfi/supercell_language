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
#include "language/tree/basic/Annotations.h"
#include "utility/Foreach.h"

namespace zillians { namespace language { namespace tree {

/**
 * Package is used to represent the hierarchical structure of a program.
 * Every ASTNode, except Package and Program, must be contained by a Package.
 */
struct Package : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Package, (Package)(ASTNode));

	explicit Package(SimpleIdentifier* _id) : id(_id), annotations(NULL)
	{
		BOOST_ASSERT(_id && "null identifier for package node is not allowed");

		id->parent = this;
	}

	void setAnnotations(Annotations* anns)
	{
		if(annotations) annotations->parent = NULL;
		anns->parent = this;
		annotations = anns;
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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(id)
		COMPARE_MEMBER(children)
		COMPARE_MEMBER(objects)
		COMPARE_MEMBER(annotations)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(id)
		REPLACE_USE_WITH(children)
		REPLACE_USE_WITH(objects)
		REPLACE_USE_WITH(annotations)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Package* cloned = new Package((id) ? cast<SimpleIdentifier>(id->clone()) : NULL);

    	foreach(i, children)
    		cloned->addPackage(cast<Package>((*i)->clone()));

    	foreach(i, objects)
    		cloned->addObject((*i)->clone());

    	if(annotations)
    		cloned->setAnnotations(cast<Annotations>(annotations->clone()));

    	return cloned;
    }

    bool merge(Package& rhs)
    {
    	if(!id->isEqual(*rhs.id))
    		return false;

    	foreach(i, rhs.children)
    	{
    		bool merged = false;
    		foreach(j, children)
			{
    			if((*i)->id->isEqual(*(*j)->id))
    			{
    				(*j)->merge(**i);
    				merged = true;
    			}
			}

    		if(!merged)
    		{
    			children.push_back(*i);
    		}
    	}

    	foreach(i, rhs.objects)
    	{
    		objects.push_back(*i);
    	}

    	annotations->merge(*rhs.annotations);

    	return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & id;
    	ar & children;
    	ar & objects;
    	ar & annotations;
    }

	SimpleIdentifier* id;
	std::vector<Package*> children;
	std::vector<ASTNode*> objects;
	Annotations* annotations;

protected:
	Package() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PACKAGE_H_ */
