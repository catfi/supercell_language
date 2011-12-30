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

#ifndef ZILLIANS_LANGUAGE_TREE_SOURCE_H_
#define ZILLIANS_LANGUAGE_TREE_SOURCE_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/module/Internal.h"
#include "language/tree/module/Package.h"
#include "language/tree/module/Import.h"

namespace zillians { namespace language { namespace tree {

struct Source : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Source, (Source)(ASTNode));

	explicit Source(const std::string& filename, bool is_imported = false) : is_imported(is_imported), filename(filename), root(new Package(new SimpleIdentifier(L"")))
	{
		root->parent = this;
	}

	explicit Source(const std::string& filename, Package* root, bool is_imported = false) : is_imported(is_imported), filename(filename), root(root)
	{
		BOOST_ASSERT(root && "null root for Source node is not allowed");
		root->parent = this;
	}

	void addImport(Import* import)
	{
		import->parent = this;
		imports.push_back(import);
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(filename)
		COMPARE_MEMBER(imports)
		COMPARE_MEMBER(root)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(imports)
		REPLACE_USE_WITH(root)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Source* cloned = new Source(filename, cast<Package>(root->clone()));

    	foreach(i, imports)
    		cloned->addImport(cast<Import>((*i)->clone()));

    	return cloned;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    	ar & filename;
    	ar & imports;
    	ar & root;
    }

    bool is_imported;
    std::string filename;
	std::vector<Import*> imports;
	Package* root;

private:
	Source() : is_imported(false)
	{ }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_SOURCE_H_ */
