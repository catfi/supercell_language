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

struct ASTGraphvizParentEdgeGenerator : public GenericDoubleVisitor
{
    CREATE_INVOKER(mInvoker, genParentEdge);

    ASTGraphvizParentEdgeGenerator(std::wostream& os) : os_(os)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(mInvoker);
    }

    ~ASTGraphvizParentEdgeGenerator()
    {
    }

    void genParentEdge(zillians::language::tree::ASTNode& node);
    void addParentEdge(ASTNode* from, ASTNode* to, const std::wstring& label = L"", const std::wstring& color = L"");

private:
    std::wostream& os_;

};

struct ASTGraphvizChildEdgeGenerator : public GenericDoubleVisitor
{
    CREATE_INVOKER(mInvoker, genChildEdge);

    ASTGraphvizChildEdgeGenerator(std::wostream& os) : os_(os)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(mInvoker);
    }

    ~ASTGraphvizChildEdgeGenerator()
    {
    }

    void genChildEdge(zillians::language::tree::ASTNode& node);
    void genChildEdge(zillians::language::tree::Annotation& node);
    void genChildEdge(zillians::language::tree::Annotations& node);
    void genChildEdge(zillians::language::tree::Internal& node);
    void genChildEdge(zillians::language::tree::Tangle& node);
    void genChildEdge(zillians::language::tree::Source& node);
    void genChildEdge(zillians::language::tree::Package& node);
    void genChildEdge(zillians::language::tree::Import& node);
    void genChildEdge(zillians::language::tree::Block& node);
    void genChildEdge(zillians::language::tree::Identifier& node);
    void genChildEdge(zillians::language::tree::SimpleIdentifier& node);
    void genChildEdge(zillians::language::tree::NestedIdentifier& node);
    void genChildEdge(zillians::language::tree::TemplateType& node);
    void genChildEdge(zillians::language::tree::TemplatedIdentifier& node);
    //void genChildEdge(zillians::language::tree::Literal& node);
    //void genChildEdge(zillians::language::tree::NumericLiteral& node);
    //void genChildEdge(zillians::language::tree::StringLiteral& node);
    //void genChildEdge(zillians::language::tree::ObjectLiteral& node);
    void genChildEdge(zillians::language::tree::TypeSpecifier& node);
    void genChildEdge(zillians::language::tree::FunctionType& node);
    void genChildEdge(zillians::language::tree::Declaration& node);
    void genChildEdge(zillians::language::tree::ClassDecl& node);
    void genChildEdge(zillians::language::tree::EnumDecl& node);
    void genChildEdge(zillians::language::tree::InterfaceDecl& node);
    void genChildEdge(zillians::language::tree::TypedefDecl& node);
    void genChildEdge(zillians::language::tree::FunctionDecl& node);
    void genChildEdge(zillians::language::tree::VariableDecl& node);
    void genChildEdge(zillians::language::tree::Statement& node);
    void genChildEdge(zillians::language::tree::DeclarativeStmt& node);
    void genChildEdge(zillians::language::tree::ExpressionStmt& node);
    //void genChildEdge(zillians::language::tree::IterativeStmt& node);
    void genChildEdge(zillians::language::tree::ForStmt& node);
    void genChildEdge(zillians::language::tree::ForeachStmt& node);
    void genChildEdge(zillians::language::tree::WhileStmt& node);
    void genChildEdge(zillians::language::tree::Selection& node);
    //void genChildEdge(zillians::language::tree::SelectionStmt& node);
    void genChildEdge(zillians::language::tree::IfElseStmt& node);
    void genChildEdge(zillians::language::tree::SwitchStmt& node);
    void genChildEdge(zillians::language::tree::BranchStmt& node);
    void genChildEdge(zillians::language::tree::Expression& node);
    void genChildEdge(zillians::language::tree::PrimaryExpr& node);
    void genChildEdge(zillians::language::tree::UnaryExpr& node);
    void genChildEdge(zillians::language::tree::BinaryExpr& node);
    void genChildEdge(zillians::language::tree::TernaryExpr& node);
    void genChildEdge(zillians::language::tree::MemberExpr& node);
    void genChildEdge(zillians::language::tree::CallExpr& node);
    void genChildEdge(zillians::language::tree::CastExpr& node);

    void addChildEdge(ASTNode* parent, ASTNode* child, const std::wstring& label = L"", const std::wstring& color = L"");

private:
    std::wostream& os_;

};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_ASTGRAPHVIZGENERATOR_H_ */
