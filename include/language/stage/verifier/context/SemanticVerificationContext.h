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
	static bool is_bound(tree::ASTNode* node)
	{
		return get(node);
	}

	static T* get(tree::ASTNode* node)
	{
		return node->get<T>();
	}

	static T* bind(tree::ASTNode* node)
	{
		T* x = get(node);
		if(!x)
			set(node, x = new T());
		return x;
	}

	static void unbind(tree::ASTNode* node)
	{
		set(node, NULL);
	}

private:
	static void set(tree::ASTNode* node, T* ctx)
	{
		node->set<T>(ctx);
	}
};

/////////////////////////////////////////////////////////////////////
/// s0

// DUPE_NAME
struct SemanticVerificationScopeContext_NameSet : public ASTNodeContext<SemanticVerificationScopeContext_NameSet>
{
	std::set<std::wstring> names;
};

// DEAD_CODE
struct SemanticVerificationBlockContext_HasVisitedReturn : public ASTNodeContext<SemanticVerificationBlockContext_HasVisitedReturn>
{ };

/////////////////////////////////////////////////////////////////////
/// s1

// MISSING_RETURN
// CONTROL_REACHES_END
struct SemanticVerificationFunctionDeclContext_ReturnCount : public ASTNodeContext<SemanticVerificationFunctionDeclContext_ReturnCount>
{
	SemanticVerificationFunctionDeclContext_ReturnCount() : count(0)
	{ }

	size_t count;
};

// UNINIT_REF
struct SemanticVerificationVariableDeclContext_HasBeenInit : public ASTNodeContext<SemanticVerificationVariableDeclContext_HasBeenInit>
{ };

// MISSING_CASE
struct SemanticVerificationEnumKeyContext_HasVisited : public ASTNodeContext<SemanticVerificationEnumKeyContext_HasVisited>
{ };

// CONTROL_REACHES_END
struct SemanticVerificationBlockContext_BranchCount : public ASTNodeContext<SemanticVerificationBlockContext_BranchCount>
{
	SemanticVerificationBlockContext_BranchCount() : count(0)
	{ }

	size_t count;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_SEMANTICVERIFICATIONCONTEXT_H_ */
