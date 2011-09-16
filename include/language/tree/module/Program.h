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
#include "language/tree/module/Package.h"
#include "language/tree/module/Import.h"

namespace zillians { namespace language { namespace tree {

struct Program : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Program, (Program)(ASTNode));

	Program() : root(new Package(new SimpleIdentifier(L"")))
	{ }

	Program(Package* root) : root(root)
	{
		BOOST_ASSERT(root && "null root for program node is not allowed");

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
		COMPARE_MEMBER(root)
		COMPARE_MEMBER(imports)
		END_COMPARE()
    }

	Package* root;
	std::vector<Import*> imports;

};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PROGRAM_H_ */
