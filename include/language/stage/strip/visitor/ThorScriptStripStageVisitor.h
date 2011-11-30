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
 */
/**
 * @date Aug 10, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_THORSCRIPTSTRIPSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_THORSCRIPTSTRIPSTAGEVISITOR_H_

#include <string>
#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct ThorScriptStripStageVisitor : public GenericDoubleVisitor
{
    CREATE_INVOKER(mStripeVisitorInvoker, apply);

	ThorScriptStripStageVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mStripeVisitorInvoker)
	}

    void apply(zillians::language::tree::ASTNode& node)
    {
        revisit(node);
    }

    void apply(zillians::language::tree::FunctionDecl& node)
    {
        if(!isFormalTemplateFunction(node))
        {
            node.block = NULL;
        }
    }

private:
    bool static isFormalTemplateFunction(zillians::language::tree::FunctionDecl& node)
    {
        return isa<TemplatedIdentifier>(node.name) &&
               cast<TemplatedIdentifier>(node.name)->type == zillians::language::tree::TemplatedIdentifier::Usage::FORMAL_PARAMETER ;
    }

};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_THORSCRIPTSTRIPSTAGEVISITOR_H_ */
