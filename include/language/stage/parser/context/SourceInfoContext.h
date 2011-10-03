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

#ifndef ZILLIANS_LANGUAGE_SOURCEINFOCONTEXT_H_
#define ZILLIANS_LANGUAGE_SOURCEINFOCONTEXT_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/GlobalContext.h"

namespace zillians { namespace language { namespace stage {

/// ModuleDebuggingContext will be only stored at Program node
struct ModuleSourceInfoContext
{
	int32 addSource(const std::string& filename)
	{
		source_files.push_back(filename);
		return (int32)source_files.size() - 1;
	}

	static ModuleSourceInfoContext* get(tree::ASTNode* node)
	{
		return node->get<ModuleSourceInfoContext>();
	}

	static void set(tree::ASTNode* node, ModuleSourceInfoContext* ctx)
	{
		node->set<ModuleSourceInfoContext>(ctx);
	}

	std::vector<std::string> source_files;
};

/// SourceInfoContext will be stored in every AST Identifier, Statement, Expression, and Declaration
struct SourceInfoContext
{
	SourceInfoContext(int32 i, uint32 l, uint32 c) : source_index(i), line(l), column(c)
	{ }

	SourceInfoContext(const SourceInfoContext& ref) : source_index(ref.source_index), line(ref.line), column(ref.column)
	{ }

	static SourceInfoContext* get(tree::ASTNode* node)
	{
		return node->get<SourceInfoContext>();
	}

	static void set(tree::ASTNode* node, SourceInfoContext* ctx)
	{
		node->set<SourceInfoContext>(ctx);
	}

	int32 source_index;
	uint32 line;
	uint32 column;
};

} } }

#endif /* ZILLIANS_LANGUAGE_SOURCEINFOCONTEXT_H_ */
