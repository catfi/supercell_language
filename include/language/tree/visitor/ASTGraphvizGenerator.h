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

	void label(ASTNode& node)
	{
        stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(Identifier& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		stream << " : " << node.toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(Import& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		if(node.alias)
		{
			if(node.alias->isEmpty())
			{
				stream << "[alias: . => " << node.ns->toString() << "]";
			}
			else
			{
				stream << "[alias: " << node.alias->toString() << " => " << node.ns->toString() << "]";
			}
		}
		else
		{
			stream << "[import: " << node.ns->toString() << "]";
		}
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(Package& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		if(node.id->isEmpty())
			stream << "[root_package]";
		else
			stream << " : " << node.id->toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(Block& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		if(node.is_pipelined_block)
			stream << L"[pipeline_blocl]";
		else if(node.is_async_block)
			stream << L"[async_blocl]";
		else
			stream << L"[block]";
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(TypeSpecifier& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		stream << " : " << node.toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(ClassDecl& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		stream << " : " << node.name->toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(InterfaceDecl& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		stream << " : " << node.name->toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(EnumDecl& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		stream << " : " << node.name->toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(FunctionDecl& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
		stream << " : " << node.name->toString();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(Statement& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(BranchStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(DeclarativeStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(ExpressionStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(ForStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(ForeachStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(WhileStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(IfElseStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(SwitchStmt& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(Expression& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(UnaryExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(BinaryExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(TernaryExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(CallExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(CastExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(PrimaryExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
        revisit(node);
	}

	void label(MemberExpr& node)
	{
		stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
        stream << "\"];" << std::endl;
	}

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

    void apply(zillians::language::tree::ASTNode& node)
    {
        addEdge(node.parent, &node);

        // ResolvedType
        if(ASTNode* context = ResolvedType::get(&node))
        {
            addEdge(&node, context, L"ResolvedType", L"purple");
        }

        // ResolvedSymbol
        if(ASTNode* context = ResolvedSymbol::get(&node))
        {
            addEdge(&node, context, L"ResolvedSymbol", L"orange");
        }

        // SplitReferenceContext
        if(ASTNode* context = SplitReferenceContext::get(&node))
        {
            addEdge(&node, context, L"SplitReferenceContext", L"blue");
        }

        revisit(node);
    }

    void addEdge(ASTNode* from, ASTNode* to, const std::wstring& label = L"", const std::wstring& color = L"black")
    {
        os_ << L"    n"
            << std::hex << from
            << L" -> n"
            << std::hex << to
            << L" [label=\"" << label << L"\", color=\"" << color << L"\"];"
            << std::endl;
    }

public:

private:
    std::wostream& os_;

};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_ASTGRAPHVIZGENERATOR_H_ */
