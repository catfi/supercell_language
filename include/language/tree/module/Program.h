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

#ifndef ZILLIANS_LANGUAGE_TREE_PROGRAM_H_
#define ZILLIANS_LANGUAGE_TREE_PROGRAM_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/module/Internal.h"
#include "language/tree/module/Package.h"
#include "language/tree/module/Import.h"

namespace zillians { namespace language { namespace tree {

struct Program : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Program, (Program)(ASTNode));

	Program() : imported_root(new Package(new SimpleIdentifier(L""))), root(new Package(new SimpleIdentifier(L""))), internal(new Internal())
	{
		imported_root->parent = this;
		root->parent = this;
		internal->parent = this;
	}

	Program(Package* root) : imported_root(new Package(new SimpleIdentifier(L""))), root(root), internal(new Internal())
	{
		BOOST_ASSERT(root && "null root for program node is not allowed");

		imported_root->parent = this;
		root->parent = this;
		internal->parent = this;
	}

	void addImport(Import* import)
	{
		import->parent = this;
		imports.push_back(import);
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(imports)
		// no comparison since we don't tree two program node different if their imported root is different
		if(0) { COMPARE_MEMBER(imported_root) }
		COMPARE_MEMBER(root)
		COMPARE_MEMBER(internal)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(imports)
		// no replace use with since we won't operate on the imported root since it's imported and has been processed
		if(0) { REPLACE_USE_WITH(imported_root) }
		REPLACE_USE_WITH(root)
		REPLACE_USE_WITH(internal)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	Program* cloned = new Program(cast<Package>(root->clone()));

    	foreach(i, imports)
    		cloned->addImport(cast<Import>((*i)->clone()));

    	return cloned;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	ar & boost::serialization::base_object<ASTNode>(*this);
    	// no serialization since it must be de-serialized from somewhere and we don't want our AST become redundant
    	if(0) ar & imported_root;
    	ar & imports;
    	ar & root;
    	ar & internal;
    }

	std::vector<Import*> imports;
	Package* imported_root;
	Package* root;
	Internal* internal;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PROGRAM_H_ */
