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
// ASTGraphvizParentEdgeGenerator
//////////////////////////////////////////////////////////////////////////////

void ASTGraphvizParentEdgeGenerator::genParentEdge(zillians::language::tree::ASTNode& node)
{
    addParentEdge(node.parent, &node);

    // ResolvedType
    if(ASTNode* context = zillians::language::ResolvedType::get(&node))
    {
        addParentEdge(&node, context, L"ResolvedType", L"purple");
    }

    // ResolvedSymbol
    if(ASTNode* context = zillians::language::ResolvedSymbol::get(&node))
    {
        addParentEdge(&node, context, L"ResolvedSymbol", L"orange");
    }

    // SplitReferenceContext
    if(ASTNode* context = zillians::language::SplitReferenceContext::get(&node))
    {
        addParentEdge(&node, context, L"SplitReferenceContext", L"blue");
    }

    revisit(node);
}

void ASTGraphvizParentEdgeGenerator::addParentEdge(ASTNode* from, ASTNode* to, const std::wstring& label, const std::wstring& edgeColor)
{
    // edge
    os_ << L"    n"
        << std::hex << from
        << L" -> n"
        << std::hex << to;

    // attribute
    std::wstring color = edgeColor;
    if(color == L"") color = L"lightgray";

    os_ << L" [arrowhead=\"crow\"";
    if(label != L"") os_ << L", label=\""   << label << L"\"";
    if(color != L"") os_ << L", color=\"" << color << L"\"";

    os_ << L"];" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////
// ASTGraphvizChildEdgeGenerator
//////////////////////////////////////////////////////////////////////////////

void ASTGraphvizChildEdgeGenerator::genChildEdge(ASTNode& node)
{
    UNUSED_ARGUMENT(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Annotation& node)
{
    if(node.name) addChildEdge(&node, node.name);
    foreach(i, node.attribute_list)
    {
        if(i->first) addChildEdge(&node, i->first);
        if(i->second) addChildEdge(&node, i->second);
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Annotations& node)
{
    foreach(i, node.annotation_list)	addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Block& node)
{
    foreach(i, node.objects)	addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Identifier& node)
{
    UNUSED_ARGUMENT(node);
    UNREACHABLE_CODE();
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(SimpleIdentifier& node)
{
    UNUSED_ARGUMENT(node);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(NestedIdentifier& node)
{
    foreach(i, node.identifier_list)	addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(TemplatedIdentifier& node)
{
    if(node.id) addChildEdge(&node, node.id);

    foreach(i, node.templated_type_list)
    {
        if(i->id) addChildEdge(&node, (i->id));
        if(i->specialized_type) addChildEdge(&node, (i->specialized_type));
        if(i->default_type) addChildEdge(&node, (i->default_type));
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(TypeSpecifier& node)
{
    switch(node.type)
    {
    case TypeSpecifier::ReferredType::FUNCTION_TYPE: if(node.referred.function_type) addChildEdge(&node, node.referred.function_type); break;
    case TypeSpecifier::ReferredType::UNSPECIFIED: if(node.referred.unspecified) addChildEdge(&node, node.referred.unspecified); break;
    default: break;
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(FunctionType& node)
{
    foreach(i, node.templated_parameters)	addChildEdge(&node, *i);
    foreach(i, node.argument_types)			addChildEdge(&node, *i);
    revisit(node);
}

//////////////////////////////////////////////////////////////////////
/// Module
void ASTGraphvizChildEdgeGenerator::genChildEdge(Internal& node)
{
    if(node.VoidTy)     addChildEdge(&node, node.VoidTy);
    if(node.BooleanTy)  addChildEdge(&node, node.BooleanTy);
    if(node.Int8Ty)     addChildEdge(&node, node.Int8Ty);
    if(node.Int16Ty)    addChildEdge(&node, node.Int16Ty);
    if(node.Int32Ty)    addChildEdge(&node, node.Int32Ty);
    if(node.Int64Ty)    addChildEdge(&node, node.Int64Ty);
    if(node.Float32Ty)  addChildEdge(&node, node.Float32Ty);
    if(node.Float64Ty)  addChildEdge(&node, node.Float64Ty);
    if(node.ObjectTy)   addChildEdge(&node, node.ObjectTy);
    if(node.FunctionTy) addChildEdge(&node, node.FunctionTy);
    if(node.StringTy)   addChildEdge(&node, node.StringTy);

    foreach(i, node.others) addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Tangle& node)
{
    if(node.internal) addChildEdge(&node, node.internal);

    foreach(i, node.sources)
    {
        if(i->first) addChildEdge(&node, (i->first));
        if(i->second) addChildEdge(&node, (i->second));
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Source& node)
{
    foreach(i, node.imports) addChildEdge(&node, *i);

    if(node.root) addChildEdge(&node, node.root);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Package& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.id) addChildEdge(&node, node.id);
    foreach(i, node.children)	addChildEdge(&node, *i);
    foreach(i, node.objects)	addChildEdge(&node, *i);
    if(node.annotations) addChildEdge(&node, node.annotations);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(Import& node)
{
    if(node.ns) addChildEdge(&node, node.ns);
    revisit(node);
}

//////////////////////////////////////////////////////////////////////
/// Declaration
void ASTGraphvizChildEdgeGenerator::genChildEdge(Declaration& node)
{
    UNUSED_ARGUMENT(node);
    UNREACHABLE_CODE();
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(ClassDecl& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.name) addChildEdge(&node, node.name);
    if(node.base) addChildEdge(&node, node.base);
    foreach(i, node.implements)			addChildEdge(&node, *i);
    foreach(i, node.member_functions)	addChildEdge(&node, *i);
    foreach(i, node.member_variables)	addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(EnumDecl& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.name) addChildEdge(&node, node.name);
    foreach(i, node.values)
    {
        addChildEdge(&node, *i);
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(FunctionDecl& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.name) addChildEdge(&node, node.name);
    foreach(i, node.parameters)
        addChildEdge(&node, *i);
    if(node.type) addChildEdge(&node, node.type);
    if(node.block) addChildEdge(&node, node.block);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(InterfaceDecl& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.name) addChildEdge(&node, node.name);
    foreach(i, node.member_functions)
        addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(TypedefDecl& node)
{
    if(node.type) addChildEdge(&node, node.type);
    if(node.name) addChildEdge(&node, node.name);

    if(node.annotations) addChildEdge(&node, node.annotations);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(VariableDecl& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.initializer) addChildEdge(&node, node.initializer);
    if(node.name) addChildEdge(&node, node.name);
    if(node.type) addChildEdge(&node, node.type);
    revisit(node);
}

//////////////////////////////////////////////////////////////////////
/// Statement
void ASTGraphvizChildEdgeGenerator::genChildEdge(Statement& node)
{
    UNUSED_ARGUMENT(node);
    UNREACHABLE_CODE();
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(DeclarativeStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.declaration) addChildEdge(&node, node.declaration);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(ExpressionStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.expr) addChildEdge(&node, node.expr);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(ForStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.init) addChildEdge(&node, node.init);
    if(node.cond) addChildEdge(&node, node.cond);
    if(node.block) addChildEdge(&node, node.block);
    if(node.step) addChildEdge(&node, node.step);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(ForeachStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.iterator) addChildEdge(&node, node.iterator);
    if(node.range) addChildEdge(&node, node.range);
    if(node.block) addChildEdge(&node, node.block);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(WhileStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.cond) addChildEdge(&node, node.cond);
    if(node.block) addChildEdge(&node, node.block);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(IfElseStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.if_branch.cond) addChildEdge(&node, node.if_branch.cond);
    if(node.if_branch.block) addChildEdge(&node, node.if_branch.block);
    foreach(i, node.elseif_branches)
    {
        if(i->cond) addChildEdge(&node, i->cond);
        if(i->block) addChildEdge(&node, i->block);
    }
    if(node.else_block) addChildEdge(&node, node.else_block);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(SwitchStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.node) addChildEdge(&node, node.node);
    foreach(i, node.cases)
    {
        if(i->cond) addChildEdge(&node, i->cond);
        if(i->block) addChildEdge(&node, i->block);
    }
    if(node.default_block) visit(*node.default_block);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(BranchStmt& node)
{
    if(node.annotations) addChildEdge(&node, node.annotations);

    if(node.result) addChildEdge(&node, node.result);
    revisit(node);
}

//////////////////////////////////////////////////////////////////////
/// Expression
void ASTGraphvizChildEdgeGenerator::genChildEdge(Expression& node)
{
    UNUSED_ARGUMENT(node);
    UNREACHABLE_CODE();
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(PrimaryExpr& node)
{
    switch(node.catagory)
    {
    case PrimaryExpr::Catagory::IDENTIFIER: if(node.value.identifier) addChildEdge(&node, node.value.identifier); break;
    case PrimaryExpr::Catagory::LITERAL: if(node.value.literal) addChildEdge(&node, node.value.literal); break;
    case PrimaryExpr::Catagory::LAMBDA: if(node.value.lambda) addChildEdge(&node, node.value.lambda); break;
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(UnaryExpr& node)
{
    if(node.node) addChildEdge(&node, node.node);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(BinaryExpr& node)
{
    if(node.isRighAssociative())
    {
        if(node.right) addChildEdge(&node, node.right);
        if(node.left) addChildEdge(&node, node.left);
    }
    else
    {
        if(node.left) addChildEdge(&node, node.left);
        if(node.right) addChildEdge(&node, node.right);
    }
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(TernaryExpr& node)
{
    if(node.cond) addChildEdge(&node, node.cond);
    if(node.true_node) addChildEdge(&node, node.true_node);
    if(node.false_node) addChildEdge(&node, node.false_node);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(MemberExpr& node)
{
    if(node.node) addChildEdge(&node, node.node);
    if(node.member) addChildEdge(&node, node.member);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(CallExpr& node)
{
    if(node.node) addChildEdge(&node, node.node);
    foreach(i, node.parameters)
    addChildEdge(&node, *i);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::genChildEdge(CastExpr& node)
{
    if(node.node) addChildEdge(&node, node.node);
    if(node.type) addChildEdge(&node, node.type);
    revisit(node);
}

void ASTGraphvizChildEdgeGenerator::addChildEdge(ASTNode* from, ASTNode* to, const std::wstring& label, const std::wstring& color)
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
