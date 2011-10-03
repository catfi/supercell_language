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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_MANGLINGSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_MANGLINGSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/tree/visitor/general/NameManglingVisitor.h"
#include "language/stage/transformer/context/ManglingStageContext.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;
using zillians::language::tree::visitor::NameManglingVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * ManglingStageVisitor is the visitation helper for ManglingStage
 *
 * @see ManglingStage
 */
struct ManglingStageVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(mangleInvoker, mangle)

	ManglingStageVisitor() : next_type_id(1024), next_symbol_id(0)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mangleInvoker)
	}

	void mangle(ASTNode& node)
	{
		revisit(node);
	}

	void mangle(Identifier& node)
	{
		NameManglingContext::set(&node, new NameManglingContext(mangler.encode(node.toString())));
	}

	void mangle(ClassDecl& node)
	{
		mangler.visit(node);
		NameManglingContext::set(&node, new NameManglingContext(mangler.stream.str()));
		mangler.reset();

		TypeIdManglingContext::set(&node, new TypeIdManglingContext(next_type_id++));

		revisit(node);
	}

	void mangle(InterfaceDecl& node)
	{
		mangler.visit(node);
		NameManglingContext::set(&node, new NameManglingContext(mangler.stream.str()));
		mangler.reset();

		TypeIdManglingContext::set(&node, new TypeIdManglingContext(next_type_id++));

		revisit(node);
	}

	void mangle(EnumDecl& node)
	{
		mangler.visit(node);
		NameManglingContext::set(&node, new NameManglingContext(mangler.stream.str()));
		mangler.reset();

		TypeIdManglingContext::set(&node, new TypeIdManglingContext(next_type_id++));

		revisit(node);
	}

	void mangle(FunctionDecl& node)
	{
		mangler.visit(node);
		NameManglingContext::set(&node, new NameManglingContext(mangler.stream.str()));
		mangler.reset();

		SymbolIdManglingContext::set(&node, new SymbolIdManglingContext(next_symbol_id++));

		revisit(node);
	}

	void mangle(VariableDecl& node)
	{
		NameManglingContext::set(&node, new NameManglingContext(mangler.encode(node.name->toString())));
		mangler.reset();
	}

private:
	NameManglingVisitor mangler;
	uint64 next_type_id;
	uint64 next_symbol_id;

};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_MANGLINGSTAGEVISITOR_H_ */
