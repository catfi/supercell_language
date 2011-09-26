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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_TYPECONVERSIONSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_TYPECONVERSIONSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/logging/LoggerWrapper.h"
#include "language/logging/StringTable.h"
#include "language/resolver/Resolver.h"

using namespace zillians::language::tree;

namespace zillians { namespace language { namespace stage { namespace visitor {

// TODO implement resolve cache

struct TypeConversionStageVisitor : GenericDoubleVisitor
{
	CREATE_INVOKER(convertInvoker, convert)

	TypeConversionStageVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(convertInvoker)
	}

	void convert(ASTNode& node)
	{
		revisit(node);
	}

	void convert(IfElseStmt& node)
	{

	}

	void convert(ForStmt& node)
	{

	}

	void convert(ForeachStmt& node)
	{
		// TODO convert into range object
	}

	void convert(WhileStmt& node)
	{

	}

	void convert(BinaryExpr& node)
	{
		revisit(node);

		ASTNode* resolved_type_left = ResolvedType::get(node.left);
		ASTNode* resolved_type_right = ResolvedType::get(node.right);

		if(!resolved_type_left)
		{
		}

		if(!resolved_type_right)
		{
		}

		if(resolved_type_left && resolved_type_right)
		{
			if(resolved_type_left != resolved_type_right)
			{

			}
		}
	}

	void convert(CallExpr& node)
	{
		revisit(node);
	}

	void applyTransforms()
	{
		foreach(i, transforms)
		{
			(*i)();
		}
		transforms.clear();
	}

	std::vector<std::function<void()>> transforms;
};

} } } }
#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_TYPECONVERSIONSTAGEVISITOR_H_ */
