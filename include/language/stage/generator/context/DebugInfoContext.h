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

#ifndef ZILLIANS_LANGUAGE_DEBUGINFOCONTEXT_H_
#define ZILLIANS_LANGUAGE_DEBUGINFOCONTEXT_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/GlobalContext.h"
#include "language/stage/generator/detail/LLVMHeaders.h"

namespace zillians { namespace language { namespace stage {

/// This is per program context
struct DebugInfoProgramContext
{
	void addProgramContext(llvm::DICompileUnit& compile_unit, llvm::DIFile& file)
	{
		compile_units.push_back(compile_unit);
		files.push_back(file);
	}

	static DebugInfoProgramContext* get(tree::ASTNode* node)
	{
		return node->get<DebugInfoProgramContext>();
	}

	static void set(tree::ASTNode* node, DebugInfoProgramContext* ctx)
	{
		node->set<DebugInfoProgramContext>(ctx);
	}

	std::vector<llvm::DICompileUnit> compile_units;
	std::vector<llvm::DIFile> files;
};

struct DebugInfoContext
{
	DebugInfoContext(llvm::DICompileUnit compile_unit, llvm::DIFile file, llvm::DIDescriptor context) :
		compile_unit(compile_unit), file(file), context(context)
	{}

	static DebugInfoContext* get(tree::ASTNode* node)
	{
		return node->get<DebugInfoContext>();
	}

	static void set(tree::ASTNode* node, DebugInfoContext* ctx)
	{
		node->set<DebugInfoContext>(ctx);
	}

	// My own descriptor context
	llvm::DIDescriptor context;

	// which compile unit it belongs to
	llvm::DICompileUnit compile_unit;

	// which file it belongs to
	llvm::DIFile file;
};

struct DebugInfoTypeContext
{
	DebugInfoTypeContext(shared_ptr<llvm::DIType> type) : type(type)
	{}

	static DebugInfoTypeContext* get(tree::ASTNode* node)
	{
		return node->get<DebugInfoTypeContext>();
	}

	static void set(tree::ASTNode* node, DebugInfoTypeContext* ctx)
	{
		node->set<DebugInfoTypeContext>(ctx);
	}

	shared_ptr<llvm::DIType> type;
};

} } }

#endif /* ZILLIANS_LANGUAGE_DEBUGINFOCONTEXT_H_ */
