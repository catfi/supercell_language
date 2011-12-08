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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_IMPLICITCONVERSIONSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_IMPLICITCONVERSIONSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/context/TransformerContext.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

/**
 * ImplicitConversionStageVisitor is the visitation helper for ImplicitConversionStage
 *
 * @see ImplicitConversionStage
 */
struct ImplicitConversionStageVisitor : public GenericDoubleVisitor
{
    CREATE_INVOKER(convertInvoker, convert)

    ImplicitConversionStageVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(convertInvoker)
	}

	void convert(ASTNode& node)
	{
		revisit(node);
	}

    //void convert(BinaryExpr& node)
    //{
    //    if(node.opcode != BinaryExpr::OpCode::ASSIGN)
    //    {
    //        return;
    //    }

    //    tree::ASTNode* ltype = ResolvedType::get(node.left);
    //    tree::ASTNode* rtype = ResolvedType::get(node.right);
    //    BOOST_ASSERT(ltype != NULL);
    //    BOOST_ASSERT(rtype != NULL);
    //    tree::ASTNode* uniqLtype = ASTNodeHelper::findUniqueTypeResolution(ltype);
    //    tree::ASTNode* uniqRtype = ASTNodeHelper::findUniqueTypeResolution(rtype);

    //    if(uniqLtype != uniqRtype)
    //    {
    //        transforms.push_back([&](){
    //            CastExpr* castExpr = new tree::CastExpr(node.right, ltype);
    //            ResolvedType::set(castExpr, ltype);
    //        });
    //    }
    //}

	void convert(CallExpr& node)
	{
        // for each argument
        for(size_t i=0; i != node.parameters.size(); ++i)
        {
            FunctionDecl* func          = cast<FunctionDecl>(ResolvedSymbol::get(node.node));
            Expression*   argumentExpr  = node.parameters[i];
            VariableDecl* parameterDecl = func->parameters[i];

            if(ResolvedType::get(argumentExpr)->isEqual(*parameterDecl->type))
            {
                continue;
            }

            transforms.push_back([&node, i, argumentExpr, parameterDecl](){
                CastExpr* castExpr = new CastExpr(argumentExpr, parameterDecl->type);
                node.parameters[i] = castExpr;
                castExpr->parent = &node;
                ResolvedType::set(castExpr, parameterDecl->type);
            });
        }
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

private:
	std::vector<std::function<void()>> transforms;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_IMPLICITCONVERSIONSTAGEVISITOR_H_ */
