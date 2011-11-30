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
 *
 * @date Nov 28, 2011 yoco - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_ASTGRAPHVIZGENERATOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_ASTGRAPHVIZGENERATOR_H_

#include <iostream>
#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {


struct ASTGraphvizNodeGenerator : public GenericDoubleVisitor
{
    CREATE_INVOKER(mInvoker, label);

    ASTGraphvizNodeGenerator(std::wostream& os) : stream(os)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(mInvoker);
    }

    ~ASTGraphvizNodeGenerator()
    { }

	void label(ASTNode& node);
	void label(Identifier& node);
	void label(Import& node);
	void label(Package& node);
	void label(Block& node);
	void label(TypeSpecifier& node);
	void label(Declaration& node);

    void addNode(ASTNode& node,
                 const std::wstring& label = L"",
                 const std::wstring& shape = L"",
                 const std::wstring& borderColor = L"",
                 const std::wstring& fillColor = L"");
private:
    std::wostream& stream;
} ;

struct ASTGraphvizEdgeGenerator : public GenericDoubleVisitor
{
    CREATE_INVOKER(mInvoker, apply);

    ASTGraphvizEdgeGenerator(std::wostream& os) : os_(os)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(mInvoker);
    }

    ~ASTGraphvizEdgeGenerator()
    {
    }

    void apply(zillians::language::tree::ASTNode& node);
    void addEdge(ASTNode* from, ASTNode* to, const std::wstring& label = L"", const std::wstring& color = L"");

private:
    std::wostream& os_;

};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_ASTGRAPHVIZGENERATOR_H_ */
