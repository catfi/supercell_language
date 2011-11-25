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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGLOBALDISPATCHGENERATORSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGLOBALDISPATCHGENERATORSTAGEVISITOR_H_

#include "core/Prerequisite.h"

#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/GenericVisitor.h"
#include "language/context/ParserContext.h"
#include "language/stage/generator/context/SynthesizedFunctionContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "utility/UnicodeUtil.h"
#include "language/stage/generator/LLVMGlobalDispatchGeneratorStage.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/LLVMContext.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct LLVMGlobalDispatchGeneratorStageVisitor : public GenericVisitor
{
    CREATE_GENERIC_INVOKER(generateInvoker)

    LLVMGlobalDispatchGeneratorStageVisitor()
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void apply(ASTNode& node)
    {
        GenericVisitor::apply(node);
    }

    void apply(Annotation& node)
    {
        if(isServerFunctionAnnotaion(node))
        {
            BOOST_ASSERT(cast<FunctionDecl>(node.parent->parent) != NULL);
            mServerFunctions.push_back(cast<FunctionDecl>(node.parent->parent));
        }
        GenericVisitor::apply(node);
    }

private:
    bool isServerFunctionAnnotaion(Annotation& node)
    {
        if(cast<FunctionDecl>(node.parent->parent) == NULL)
        {
            return false;
        }
        if(node.name->name != L"server")
        {
        	return false;
        }
        return true;
    }

public:
    std::vector<FunctionDecl*> mServerFunctions;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMGLOBALDISPATCHGENERATORSTAGEVISITOR_H_ */
