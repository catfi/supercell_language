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
		printf("statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
		printf("statement attr(1) type = %s\n", typeid(_attr_t(1)).name());
		switch(_attr(1).which())
		{
		case 0:
			{
				typedef boost::fusion::vector2<boost::optional<bool>, Declaration*> fusion_vector_t;
				fusion_vector_t vec = boost::get<fusion_vector_t>(_attr(1));
				bool is_const = boost::fusion::at_c<0>(vec).is_initialized();
				Declaration* decl = boost::fusion::at_c<1>(vec);
				if(is_const)
					dynamic_cast<VariableDecl*>(decl)->storage = Declaration::StorageSpecifier::CONST;
				_value = new DeclarativeStmt(decl);
			}
			break;
		case 1:
			_value = boost::get<Statement*>(_attr(1));
			break;
		case 2:
			_value = boost::get<Block*>(_attr(1));
			break;
		}
		switch(_attr(1).which())
		{
		case 0:
		case 1:
			if(_attr(0).is_initialized())
				dynamic_cast<Statement*>(_value)->setAnnotation(*_attr(0));
		}
	}
	END_ACTION
};

struct compound_statement
{
	DEFINE_ATTRIBUTES(Block*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("compound_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
		_value = new Block();
		_value->appendObjects(_attr(0));
	}
	END_ACTION
};

struct expression_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("expression_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
		if(_attr(0).is_initialized())
			_value = new ExpressionStmt(*_attr(0));
	}
	END_ACTION
};

struct selection_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("selection_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

struct iteration_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("iteration_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

struct branch_statement
{
	DEFINE_ATTRIBUTES(Statement*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		printf("branch_statement attr(0) type = %s\n", typeid(_attr_t(0)).name());
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_STATEMENT_STATEMENTACTIONS_H_ */
