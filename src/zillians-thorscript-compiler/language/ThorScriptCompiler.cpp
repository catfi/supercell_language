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
 * @date Jul 18, 2011 sdk - Initial version created.
 */

#include "language/ThorScriptCompiler.h"
#include "language/stage/basic/TreeDebugStage.h"
#include "language/stage/parser/ThorScriptParserStage.h"
#include "language/stage/transformer/ResolutionStage.h"
#include "language/stage/generator/LLVMGeneratorStage.h"

using namespace zillians::language::stage;

namespace zillians { namespace language {

ThorScriptCompiler::ThorScriptCompiler()
{ }

ThorScriptCompiler::~ThorScriptCompiler()
{ }

void ThorScriptCompiler::initialize()
{
	shared_ptr<Stage> parser(new ThorScriptParserStage());
	shared_ptr<Stage> resolution(new ResolutionStage());
	shared_ptr<Stage> debug_tree(new TreeDebugStage());
	shared_ptr<Stage> llvm_generator(new LLVMGeneratorStage());

	appendStage(parser);
	appendStage(debug_tree);
	appendStage(resolution);
	appendStage(llvm_generator);
}

void ThorScriptCompiler::finalize()
{

}

} }
