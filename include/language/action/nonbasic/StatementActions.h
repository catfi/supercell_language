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
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("statement attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		switch(_attr(1).which())
		{
		case 0:
			{
				Declaration* decl = boost::get<Declaration*>(_attr(1));
				_value = new DeclarativeStmt(decl);
			}
			break;
		case 1:
			_value = boost::get<Statement*>(_attr(1));
			break;
		}
		if(_attr(0).is_initialized())
			cast<Statement>(_value)->setAnnotation(*_attr(0));
	}
	END_ACTION

	BEGIN_ACTION(init_block)
	{
#ifdef DEBUG
		printf("statement::init_block attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		_value = _attr(0);
	}
	END_ACTION
};

struct expression_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("expression_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		if(_attr(0).is_initialized())
			_value = new ExpressionStmt(*_attr(0));
	}
	END_ACTION
};

struct selection_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_if_statement)
	{
#ifdef DEBUG
		printf("selection_statement::init_if_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("selection_statement::init_if_statement attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("selection_statement::init_if_statement attr(2) type = %s\n", typeid(_attr_t(2)).name());
		printf("selection_statement::init_if_statement attr(3) type = %s\n", typeid(_attr_t(3)).name());
#endif
		Expression* cond = _attr(0);
		ASTNode* block = _attr(1);
		_value = new IfElseStmt(Selection(cond, block));
		deduced_foreach_value(i, _attr(2))
		{
			Expression* cond  = boost::fusion::at_c<0>(i);
			ASTNode*    block = boost::fusion::at_c<1>(i);
			cast<IfElseStmt>(_value)->addElseIfBranch(Selection(cond, block));
		}
		if(_attr(3).is_initialized())
			cast<IfElseStmt>(_value)->setElseBranch(*_attr(3));
	}
	END_ACTION

	BEGIN_ACTION(init_switch_statement)
	{
#ifdef DEBUG
		printf("selection_statement::init_switch_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("selection_statement::init_switch_statement attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		_value = new SwitchStmt(_attr(0));
		deduced_foreach_value(i, _attr(1))
			switch(i.which())
			{
			case 0:
				{
					typedef boost::fusion::vector2<Expression*, std::vector<ASTNode*>> fusion_vec_t;
					fusion_vec_t &vec = boost::get<fusion_vec_t>(i);
					Expression*            cond      = boost::fusion::at_c<0>(vec);
					std::vector<ASTNode*> &block_vec = boost::fusion::at_c<1>(vec);
					Block* block = new Block();
					block->appendObjects(block_vec);
					cast<SwitchStmt>(_value)->addCase(Selection(cond, block));
				}
				break;
			case 1:
				{
					std::vector<ASTNode*> &block_vec = boost::get<std::vector<ASTNode*>>(i);
					Block* block = new Block();
					block->appendObjects(block_vec);
					cast<SwitchStmt>(_value)->setDefaultCase(block);
				}
				break;
			}
	}
	END_ACTION
};

struct iteration_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_while_loop)
	{
#ifdef DEBUG
		printf("iteration_statement::init_while_loop attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("iteration_statement::init_while_loop attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		Expression* cond = _attr(0);
		ASTNode* block = _attr(1).is_initialized() ? *_attr(1) : NULL;
		_value = new WhileStmt(WhileStmt::Style::WHILE, cond, block);
	}
	END_ACTION

	BEGIN_ACTION(init_do_while_loop)
	{
#ifdef DEBUG
		printf("iteration_statement::init_do_while_loop attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("iteration_statement::init_do_while_loop attr(1) type = %s\n", typeid(_attr_t(1)).name());
#endif
		ASTNode* block = _attr(0);
		Expression* cond = _attr(1);
		_value = new WhileStmt(WhileStmt::Style::DO_WHILE, cond, block);
	}
	END_ACTION

	BEGIN_ACTION(init_foreach)
	{
#ifdef DEBUG
		printf("iteration_statement::init_foreach attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("iteration_statement::init_foreach attr(1) type = %s\n", typeid(_attr_t(1)).name());
		printf("iteration_statement::init_foreach attr(2) type = %s\n", typeid(_attr_t(2)).name());
#endif
		ASTNode* iterator = NULL;
		switch(_attr(0).which())
		{
		case 0:
			iterator = boost::get<Declaration*>(_attr(0));
			break;
		case 1:
			iterator = boost::get<Expression*>(_attr(0));
			break;
		}
		Expression* range = _attr(1);
		ASTNode* block = _attr(2).is_initialized() ? *_attr(2) : NULL;
		_value = new ForeachStmt(iterator, range, block);
	}
	END_ACTION
};

struct branch_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init_return)
	{
#ifdef DEBUG
		printf("branch_statement::init_return attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		_value = new BranchStmt(tree::BranchStmt::OpCode::RETURN, _attr(0));
	}
	END_ACTION

	BEGIN_TEMPLATED_ACTION(init, BranchStmt::OpCode::type Type)
	{
		_value = new BranchStmt(Type);
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_STATEMENT_STATEMENTACTIONS_H_ */