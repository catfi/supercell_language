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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_MAPPERSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_MAPPERSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/context/TransformerContext.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * MapperStageVisitor is the visitation helper for MapperStage
 *
 * @see MapperStage
 */
struct MapperStageVisitor : public GenericDoubleVisitor
{
	CREATE_INVOKER(applyInvoker, apply)

	MapperStageVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(applyInvoker)
	}

	void apply(ASTNode& node)
	{
		revisit(node);
	}

	void apply(VariableDecl& node)
	{
		foreach(i, variable_decl_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(FunctionDecl& node)
	{
		foreach(i, function_decl_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(ClassDecl& node)
	{
		foreach(i, class_decl_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(ForStmt& node)
	{
		foreach(i, for_stmt_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(ForeachStmt& node)
	{
		foreach(i, foreach_stmt_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(WhileStmt& node)
	{
		foreach(i, while_stmt_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(MemberExpr& node)
	{
		foreach(i, member_expr_mappers) (*i)(node, transforms);
		revisit(node);
	}

	void apply(PrimaryExpr& node)
	{
		foreach(i, primary_expr_mappers) (*i)(node, transforms);
		revisit(node);
	}

public:
	bool hasTransforms()
	{
		return (transforms.size() > 0);
	}

	void applyTransforms()
	{
		foreach(i, transforms)
		{
			(*i)();
		}
		transforms.clear();
	}

public:
	typedef std::function<void(tree::VariableDecl&, std::vector<std::function<void()>>&)> VariableDeclMapper;
	typedef std::function<void(tree::FunctionDecl&, std::vector<std::function<void()>>&)> FunctionDeclMapper;
	typedef std::function<void(tree::ClassDecl&,    std::vector<std::function<void()>>&)> ClassDeclMapper;
	typedef std::function<void(tree::ForStmt&,      std::vector<std::function<void()>>&)> ForStmtMapper;
	typedef std::function<void(tree::ForeachStmt&,  std::vector<std::function<void()>>&)> ForeachStmtMapper;
	typedef std::function<void(tree::WhileStmt&,    std::vector<std::function<void()>>&)> WhileStmtMapper;
	typedef std::function<void(tree::CallExpr&,     std::vector<std::function<void()>>&)> CallExprMapper;
	typedef std::function<void(tree::MemberExpr&,   std::vector<std::function<void()>>&)> MemberExprMapper;
	typedef std::function<void(tree::PrimaryExpr&,  std::vector<std::function<void()>>&)> PrimaryExprMapper;

public:
	void addMapper(VariableDeclMapper mapper) { variable_decl_mappers.push_back(mapper); }
	void addMapper(FunctionDeclMapper mapper) { function_decl_mappers.push_back(mapper); }
	void addMapper(ClassDeclMapper    mapper) { class_decl_mappers.push_back(mapper);    }
	void addMapper(ForStmtMapper      mapper) { for_stmt_mappers.push_back(mapper);      }
	void addMapper(ForeachStmtMapper  mapper) { foreach_stmt_mappers.push_back(mapper);  }
	void addMapper(WhileStmtMapper    mapper) { while_stmt_mappers.push_back(mapper);    }
	void addMapper(CallExprMapper     mapper) { call_expr_mappers.push_back(mapper);     }
	void addMapper(MemberExprMapper   mapper) { member_expr_mappers.push_back(mapper);   }
	void addMapper(PrimaryExprMapper  mapper) { primary_expr_mappers.push_back(mapper);  }

public:
	std::vector<VariableDeclMapper> variable_decl_mappers;
	std::vector<FunctionDeclMapper> function_decl_mappers;
	std::vector<ClassDeclMapper>    class_decl_mappers;
	std::vector<ForStmtMapper>      for_stmt_mappers;
	std::vector<ForeachStmtMapper>  foreach_stmt_mappers;
	std::vector<WhileStmtMapper>    while_stmt_mappers;
	std::vector<CallExprMapper>     call_expr_mappers;
	std::vector<MemberExprMapper>   member_expr_mappers;
	std::vector<PrimaryExprMapper>  primary_expr_mappers;

private:
	std::vector<std::function<void()>> transforms;
};

} } } }


#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_MAPPERSTAGEVISITOR_H_ */
