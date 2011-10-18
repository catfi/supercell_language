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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NODEINFOVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NODEINFOVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/visitor/GenericVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NodeInfoVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(infoInvoker, info)

	NodeInfoVisitor(int32 max_depth = 20) : current_depth(0), max_depth(max_depth)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(infoInvoker)
	}

	void info(ASTNode& node)
	{
		if(node.parent) tryVisit(*node.parent);
	}

	void info(Identifier& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.toString();
	}

	void info(Package& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		if(node.id->isEmpty())
			stream << "[root_package]";
		else
			stream << node.id->toString();
	}

	void info(Block& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		if(node.is_pipelined_block)
			stream << L"[pipeline_blocl]";
		else if(node.is_async_block)
			stream << L"[async_blocl]";
		else
			stream << L"[block]";
	}

	void info(TypeSpecifier& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.toString();
	}

	void info(ClassDecl& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(InterfaceDecl& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(EnumDecl& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(FunctionDecl& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(Statement& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[stmt]";
	}

	void info(BranchStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[branch_stmt]";
	}

	void info(DeclarativeStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[decl_stmt]";
	}

	void info(ExpressionStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[expr_stmt]";
	}

	void info(ForStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[for_stmt]";
	}

	void info(ForeachStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[foreach_stmt]";
	}

	void info(WhileStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[while_stmt]";
	}

	void info(IfElseStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[ifelse_stmt]";
	}

	void info(SwitchStmt& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[switch_stmt]";
	}

	void info(Expression& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[expr]";
	}

	void info(UnaryExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[unary_expr]";
	}

	void info(BinaryExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[binary_expr]";
	}

	void info(TernaryExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[ternary_expr]";
	}

	void info(CallExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[call_expr]";
	}

	void info(CastExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[cast_expr]";
	}

	void info(PrimaryExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[primary_expr]";
	}

	void info(MemberExpr& node)
	{
		if(node.parent) tryVisit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << L"[member_expr]";
	}

	void reset()
	{
		stream.str(L"");
	}

	void tryVisit(ASTNode& node)
	{
		++current_depth;
		if(current_depth < max_depth)
			visit(node);
		--current_depth;
	}

	int32 current_depth;
	int32 max_depth;
	std::wstringstream stream;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NODEINFOVISITOR_H_ */
