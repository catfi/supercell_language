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
 * @date Aug 10, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_ANNOTATIONS_H_
#define ZILLIANS_LANGUAGE_TREE_ANNOTATIONS_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"

namespace zillians { namespace language { namespace tree {

struct Annotation : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Annotation, (Annotation)(ASTNode));

	explicit Annotation(SimpleIdentifier* name) : name(name)
	{
		if(name) name->parent = this;
	}

	void appendKeyValue(SimpleIdentifier* key, ASTNode* value)
	{
		key->parent = this;
		value->parent = this;
		attribute_list.push_back(std::make_pair(key, value));
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(name)
		COMPARE_MEMBER(attribute_list)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(name)
		REPLACE_USE_WITH(attribute_list)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Annotation* cloned = new Annotation(name);
    	foreach(i, attribute_list)
    		cloned->appendKeyValue(
    				(i->first) ? cast<SimpleIdentifier>(i->first->clone()) : NULL,
    				(i->second) ? i->second->clone() : NULL);
    	return cloned;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & name;
    	ar & attribute_list;
    }

	SimpleIdentifier* name;
	std::vector<std::pair<SimpleIdentifier*/*key*/, ASTNode*/*value*/>> attribute_list;

protected:
	Annotation() { }
};

struct Annotations : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Annotations, (Annotations)(ASTNode));

	Annotations() { }

	void appendAnnotation(Annotation* annotation)
	{
		annotation->parent = this;
		annotation_list.push_back(annotation);
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(annotation_list)
    	END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(annotation_list)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Annotations* cloned = new Annotations();
    	foreach(i, annotation_list)
    		cloned->appendAnnotation((*i) ? cast<Annotation>((*i)->clone()) : NULL);
    	return cloned;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & annotation_list;
    }

	std::vector<Annotation*> annotation_list;
};

} } }


#endif /* ZILLIANS_LANGUAGE_TREE_ANNOTATIONS_H_ */
