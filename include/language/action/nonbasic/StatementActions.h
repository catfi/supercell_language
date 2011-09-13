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

#ifndef ZILLIANS_LANGUAGE_ACTION_STATEMENT_STATEMENTACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_STATEMENT_STATEMENTACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

namespace zillians { namespace language { namespace action {

struct statement
{
	DEFINE_ATTRIBUTES(ASTNode*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("statement param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("statement param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		_result = _param(1);
		if(_param(0).is_initialized())
			cast<Statement>(_result)->setAnnotation(*_param(0));
	}
	END_ACTION

	BEGIN_ACTION(init_block)
	{
#ifdef DEBUG
		printf("statement::init_block param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		_result = _param(0);
	}
	END_ACTION
};

struct decl_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("expression_statement param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("expression_statement param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		VariableDecl* decl = dynamic_cast<VariableDecl*>(_param(1));
		decl->is_static = _param(0).is_initialized();
		BIND_CACHED_LOCATION(_result = new DeclarativeStmt(decl));
	}
	END_ACTION
};

struct expression_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("expression_statement param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		if(_param(0).is_initialized())
			BIND_CACHED_LOCATION(_result = new ExpressionStmt(*_param(0)));
	}
	END_ACTION
};

struct selection_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_if_statement)
	{
#ifdef DEBUG
		printf("selection_statement::init_if_statement param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("selection_statement::init_if_statement param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("selection_statement::init_if_statement param(2) type = %s\n", typeid(_param_t(2)).name());
		printf("selection_statement::init_if_statement param(3) type = %s\n", typeid(_param_t(3)).name());
#endif
		Expression* cond = _param(0);
		ASTNode* block = _param(1);
		BIND_CACHED_LOCATION(_result = new IfElseStmt(Selection(cond, block)));
		deduced_foreach_value(i, _param(2))
		{
			Expression* cond  = boost::fusion::at_c<0>(i);
			ASTNode*    block = boost::fusion::at_c<1>(i);
			cast<IfElseStmt>(_result)->addElseIfBranch(Selection(cond, block));
		}
		if(_param(3).is_initialized())
			cast<IfElseStmt>(_result)->setElseBranch(*_param(3));
	}
	END_ACTION

	BEGIN_ACTION(init_switch_statement)
	{
#ifdef DEBUG
		printf("selection_statement::init_switch_statement param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("selection_statement::init_switch_statement param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		BIND_CACHED_LOCATION(_result = new SwitchStmt(_param(0)));
		deduced_foreach_value(i, _param(1))
			switch(i.which())
			{
			case 0:
				{
					typedef boost::fusion::vector2<Expression*, std::vector<ASTNode*>> fusion_vec_t;
					fusion_vec_t &vec = boost::get<fusion_vec_t>(i);
					Expression*            cond      = boost::fusion::at_c<0>(vec);
					std::vector<ASTNode*> &block_vec = boost::fusion::at_c<1>(vec);
					Block* block = new Block(); BIND_CACHED_LOCATION(block);
					block->appendObjects(block_vec);
					cast<SwitchStmt>(_result)->addCase(Selection(cond, block));
				}
				break;
			case 1:
				{
					std::vector<ASTNode*> &block_vec = boost::get<std::vector<ASTNode*>>(i);
					Block* block = new Block(); BIND_CACHED_LOCATION(block);
					block->appendObjects(block_vec);
					cast<SwitchStmt>(_result)->setDefaultCase(block);
				}
				break;
			}
	}
	END_ACTION
};

struct iteration_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_while_loop)
	{
#ifdef DEBUG
		printf("iteration_statement::init_while_loop param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("iteration_statement::init_while_loop param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		Expression* cond  = _param(0);
		ASTNode*    block = _param(1).is_initialized() ? *_param(1) : NULL;
		BIND_CACHED_LOCATION(_result = new WhileStmt(WhileStmt::Style::WHILE, cond, block));
	}
	END_ACTION

	BEGIN_ACTION(init_do_while_loop)
	{
#ifdef DEBUG
		printf("iteration_statement::init_do_while_loop param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("iteration_statement::init_do_while_loop param(1) type = %s\n", typeid(_param_t(1)).name());
#endif
		ASTNode*    block = _param(0);
		Expression* cond  = _param(1);
		BIND_CACHED_LOCATION(_result = new WhileStmt(WhileStmt::Style::DO_WHILE, cond, block));
	}
	END_ACTION

	BEGIN_ACTION(init_foreach)
	{
#ifdef DEBUG
		printf("iteration_statement::init_foreach param(0) type = %s\n", typeid(_param_t(0)).name());
		printf("iteration_statement::init_foreach param(1) type = %s\n", typeid(_param_t(1)).name());
		printf("iteration_statement::init_foreach param(2) type = %s\n", typeid(_param_t(2)).name());
#endif
		ASTNode* iterator = NULL;
		switch(_param(0).which())
		{
		case 0:
			iterator = boost::get<Declaration*>(_param(0));
			break;
		case 1:
			iterator = boost::get<Expression*>(_param(0));
			break;
		}
		Expression* range = _param(1);
		ASTNode*    block = _param(2).is_initialized() ? *_param(2) : NULL;
		BIND_CACHED_LOCATION(_result = new ForeachStmt(iterator, range, block));
	}
	END_ACTION
};

struct branch_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS(LOCATION_TYPE)

	BEGIN_ACTION(init_return)
	{
#ifdef DEBUG
		printf("branch_statement::init_return param(0) type = %s\n", typeid(_param_t(0)).name());
#endif
		BIND_CACHED_LOCATION(_result = new BranchStmt(BranchStmt::OpCode::RETURN, _param(0)));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init, BranchStmt::OpCode::type Type)
	{
		BIND_CACHED_LOCATION(_result = new BranchStmt(Type));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_STATEMENT_STATEMENTACTIONS_H_ */
