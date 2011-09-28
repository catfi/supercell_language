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

#ifndef ZILLIANS_LANGUAGE_STAGE_SEMANTICVERIFICATIONCONTEXT_H_
#define ZILLIANS_LANGUAGE_STAGE_SEMANTICVERIFICATIONCONTEXT_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/GlobalContext.h"
#include <string>

namespace zillians { namespace language { namespace stage {

template<class T>
struct ASTNodeContext
{
	static T* get(tree::ASTNode* node)
	{
		return node->get<T>();
	}

	static void set(tree::ASTNode* node, T* ctx)
	{
		node->set<T>(ctx);
	}

	static T* get_instance(tree::ASTNode* node)
	{
		T* x = get(node);
		if(!x)
			set(node, x = new T());
		return x;
	}
};

struct SemanticVerificationScopeContext_NameSet : public ASTNodeContext<SemanticVerificationScopeContext_NameSet>
{
	std::set<std::wstring> names;
};

struct SemanticVerificationBlockContext_HasVisitedReturn : public ASTNodeContext<SemanticVerificationBlockContext_HasVisitedReturn>
{ };

struct SemanticVerificationVariableDeclContext_HasBeenInit : public ASTNodeContext<SemanticVerificationVariableDeclContext_HasBeenInit>
{ };

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_SEMANTICVERIFICATIONCONTEXT_H_ */
