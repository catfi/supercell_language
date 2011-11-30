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
 * @date Nov 30, 2011 yoco - Initial version created.
 */

#include <string>
#include <iostream>
#include <iomanip>
#include "language/tree/visitor/ASTGraphvizGenerator.h"
#include "language/context/TransformerContext.h"
#include "language/context/ResolverContext.h"

namespace zillians { namespace language { namespace stage { namespace visitor {

//////////////////////////////////////////////////////////////////////////////
// ASTGraphvizNodeGenerator
//////////////////////////////////////////////////////////////////////////////

void ASTGraphvizNodeGenerator::addNode(ASTNode& node,
             const std::wstring& label,
             const std::wstring& shape,
             const std::wstring& borderColor,
             const std::wstring& fillColor)
{
    std::wstring color = borderColor;
    int penwidth = 1;
    // error aware??
    if(node.parent == NULL)
    {
        color = L"red";
        penwidth = 4;
    }

    stream << L"    n" << std::hex << &node << L" [label=\"" << node.instanceName();
    if(label != L"") stream << L" : " << label ;
    stream << L"\"";

    if(shape       != L"") stream << L", shape=\""                       << shape       << "\"";
    if(color       != L"") stream << L", color=\""                       << color       << "\"";
    if(penwidth    != 1  ) stream << L", penwidth=\""                    << penwidth       << "\"";
    if(fillColor   != L"") stream << L", style=\"filled\", fillcolor=\"" << fillColor   << "\"";

    stream << L"];" << std::endl;
}

void ASTGraphvizNodeGenerator::label(ASTNode& node)
{
    addNode(node);
    revisit(node);
}

void ASTGraphvizNodeGenerator::label(Identifier& node)
{
    addNode(node, node.toString());
    revisit(node);
}

void ASTGraphvizNodeGenerator::label(Import& node)
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

void ASTGraphvizNodeGenerator::label(Package& node)
{
    stream << "    n" << std::hex << &node << " [label=\"" << node.instanceName();
    if(node.id->isEmpty())
        stream << "[root_package]";
    else
        stream << " : " << node.id->toString();
    stream << "\"];" << std::endl;
    revisit(node);
}

void ASTGraphvizNodeGenerator::label(Block& node)
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

void ASTGraphvizNodeGenerator::label(TypeSpecifier& node)
{
    addNode(node, node.toString());
    revisit(node);
}

void ASTGraphvizNodeGenerator::label(Declaration& node)
{
    addNode(node, node.name->toString());
    revisit(node);
}

//////////////////////////////////////////////////////////////////////////////
// ASTGraphvizEdgeGenerator
//////////////////////////////////////////////////////////////////////////////

void ASTGraphvizEdgeGenerator::apply(zillians::language::tree::ASTNode& node)
{
    addEdge(node.parent, &node);

    // ResolvedType
    if(ASTNode* context = zillians::language::ResolvedType::get(&node))
    {
        addEdge(&node, context, L"ResolvedType", L"purple");
    }

    // ResolvedSymbol
    if(ASTNode* context = zillians::language::ResolvedSymbol::get(&node))
    {
        addEdge(&node, context, L"ResolvedSymbol", L"orange");
    }

    // SplitReferenceContext
    if(ASTNode* context = zillians::language::SplitReferenceContext::get(&node))
    {
        addEdge(&node, context, L"SplitReferenceContext", L"blue");
    }

    revisit(node);
}

void ASTGraphvizEdgeGenerator::addEdge(ASTNode* from, ASTNode* to, const std::wstring& label, const std::wstring& color)
{
    // edge
    os_ << L"    n"
        << std::hex << from
        << L" -> n"
        << std::hex << to;

    // attribute
    os_ << L" [";
    if(label != L"") os_ << L"label=\""   << label << L"\"";
    if(color != L"") os_ << L", color=\"" << color << L"\"";

    os_ << L"];" << std::endl;
}

} } } } // namespace zillians::language::stage::visitor
