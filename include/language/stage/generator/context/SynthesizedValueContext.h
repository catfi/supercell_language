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

#ifndef ZILLIANS_LANGUAGE_STAGE_SYNTHESIZEDVALUECONTEXT_H_
#define ZILLIANS_LANGUAGE_STAGE_SYNTHESIZEDVALUECONTEXT_H_

#include "core/Prerequisite.h"
#include "core/Containers.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/GlobalContext.h"
#include "language/stage/generator/detail/LLVMHeaders.h"

namespace zillians { namespace language { namespace stage {

struct SynthesizedValueContext
{
	SynthesizedValueContext(llvm::Value* v) : v(v)
	{ }

	static SynthesizedValueContext* get(tree::ASTNode* node)
	{
		return node->get<SynthesizedValueContext>();
	}

	static void set(tree::ASTNode* node, SynthesizedValueContext* ctx)
	{
		node->set<SynthesizedValueContext>(ctx);
	}

	llvm::Value* v;
};

struct SynthesizedBlockContext
{
	SynthesizedBlockContext(llvm::BasicBlock* bb) : bb(bb)
	{ }

	static SynthesizedBlockContext* get(tree::ASTNode* node)
	{
		return node->get<SynthesizedBlockContext>();
	}

	static void set(tree::ASTNode* node, SynthesizedBlockContext* ctx)
	{
		node->set<SynthesizedBlockContext>(ctx);
	}

	llvm::BasicBlock* bb;
};

struct IntermediateValueContext
{
	IntermediateValueContext()
	{ }

	static IntermediateValueContext* get(tree::ASTNode* node)
	{
		IntermediateValueContext* ctx = node->get<IntermediateValueContext>();
		if(!ctx)
		{
			ctx = new IntermediateValueContext();
			node->set<IntermediateValueContext>(ctx);
		}
		return ctx;
	}

	static void add(tree::ASTNode* node, llvm::Value* val)
	{
		get(node)->values.insert(val);
	}

	static void remove(tree::ASTNode* node, llvm::Value* val)
	{
		get(node)->values.erase(val);
	}

	unordered_set<llvm::Value*> values;
};


#define ADD_INTERMEDIATE_LLVM_VALUE(x, val) \
	IntermediateValueContext::add(x, val)

#define REMOVE_INTERMEDIATE_LLVM_VALUE(x, val) \
	IntermediateValueContext::remove(x, val)

#define GET_INTERMEDIATE_LLVM_VALUES(x) \
	IntermediateValueContext::get(x)->values

#define GET_SYNTHESIZED_LLVM_VALUE(x) \
	((SynthesizedValueContext::get(x)) ? SynthesizedValueContext::get(x)->v : NULL)

#define SET_SYNTHESIZED_LLVM_VALUE(x, val)  \
	{ \
		if(SynthesizedValueContext::get(x)) \
			SynthesizedValueContext::get((x))->v = val; \
		else \
			SynthesizedValueContext::set(x, new SynthesizedValueContext(val)); \
	}

#define GET_SYNTHESIZED_LLVM_BLOCK(x) \
	((SynthesizedBlockContext::get(x)) ? SynthesizedBlockContext::get(x)->bb : NULL)

#define SET_SYNTHESIZED_LLVM_BLOCK(x, val)  \
	{ \
		if(SynthesizedBlockContext::get(x)) \
			SynthesizedBlockContext::get((x))->bb = val; \
		else \
			SynthesizedBlockContext::set(x, new SynthesizedBlockContext(val)); \
	}

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_SYNTHESIZEDVALUECONTEXT_H_ */
