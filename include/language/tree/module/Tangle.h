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

#ifndef ZILLIANS_LANGUAGE_TREE_TANGLE_H_
#define ZILLIANS_LANGUAGE_TREE_TANGLE_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/module/Source.h"

namespace zillians { namespace language { namespace tree {

namespace detail {

struct IdentifierCompare {
    bool operator()(tree::Identifier* const  __x, tree::Identifier* const __y) const
    {
    	return (__x->toString() < __y->toString());
    }
};

}

struct Tangle : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Tangle, (Tangle)(ASTNode));

	Tangle() : internal(new Internal())
	{
		internal->parent = this;
	}

	void addSource(Identifier* id, Source* src)
	{
		id->parent = this;
		src->parent = this;

		sources.insert(std::make_pair(id, src));
	}

	void markImported(bool is_imported)
	{
		foreach(i, sources)
			(i->second)->is_imported = is_imported;
	}

	void merge(Tangle& t)
	{
		foreach(i, t.sources)
		{
			addSource(i->first, i->second);
		}
		t.sources.clear();
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(internal)
		COMPARE_MEMBER(sources)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(internal)
		REPLACE_USE_WITH(sources)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Tangle* cloned = new Tangle();

    	return cloned;
    }

    template<typename Archive>
    void load(Archive& ar, const unsigned int version)
    {
        UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & internal;

    	int size; ar & size;
    	for(int i=0;i<size;++i)
    	{
    		Identifier* id = NULL;
    		Source* src = NULL;
    		ar & id; ar & src;
    		addSource(id, src);
    	}
    }

    template<typename Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & internal;

        size_t size = std::count_if(sources.begin(), sources.end(), [&](const std::pair<const Identifier*, Source*>& p) {
            if(p.second->is_imported) return false;
            else                      return true;
        });
        ar & size;
        for(std::multimap<Identifier*, Source*, detail::IdentifierCompare>::const_iterator i = sources.begin(); i != sources.end(); ++i)
        {
            if(!i->second->is_imported)
            {
                ar & i->first;
                ar & i->second;
            }
        }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER();

	Internal* internal;
	std::multimap<Identifier*, Source*, detail::IdentifierCompare> sources;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_TANGLE_H_ */
