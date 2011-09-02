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

#ifndef ZILLIANS_LANGUAGE_DEBUGCONTEXT_H_
#define ZILLIANS_LANGUAGE_DEBUGCONTEXT_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/GlobalContext.h"

namespace zillians { namespace language {

struct DebugContext
{
	// Module info will be only stored at Program node
	struct ModuleInfo
	{
		std::vector<std::wstring> source_files;
	};

	// TranslationUnit will be stored in every FunctionDecl function
	struct TranslationUnit
	{
		int32 source_index;
	};

	// SourceInfo will be stored in every AST Identifier, Statement, Expression, and Declaration
	struct SourceInfo
	{
		uint32 line;
		uint32 column;
	};

private:
	DebugContext() { }
};

} }

#endif /* ZILLIANS_LANGUAGE_DEBUGCONTEXT_H_ */