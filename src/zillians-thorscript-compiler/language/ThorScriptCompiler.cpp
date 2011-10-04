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
#include "language/stage/parser/ThorScriptParserStage.h"
#include "language/stage/transformer/LiteralCompactionStage.h"
#include "language/stage/transformer/RestructureStage.h"
#include "language/stage/transformer/ResolutionStage.h"
#include "language/stage/transformer/ManglingStage.h"
#include "language/stage/generator/LLVMGeneratorStage.h"
#include "language/stage/generator/LLVMDebugInfoGeneratorStage.h"
#include "language/stage/generator/LLVMBitCodeGeneratorStage.h"
#include "language/stage/generator/ASTSerializationStage.h"
#include "language/stage/verifier/SemanticVerificationStage0.h"
#include "language/stage/verifier/SemanticVerificationStage1.h"
#include "language/stage/verifier/StaticTestVerificationStage.h"

using namespace zillians::language::stage;

namespace zillians { namespace language {

ThorScriptCompiler::ThorScriptCompiler()
{
	addDefaultMode<
		boost::mpl::vector<
			ThorScriptParserStage,
			SemanticVerificationStage0,
			LiteralCompactionStage,
			RestructureStage,
			ResolutionStage,
			SemanticVerificationStage1,
			StaticTestVerificationStage,
			ManglingStage,
			LLVMGeneratorStage,
			LLVMDebugInfoGeneratorStage,
			LLVMBitCodeGeneratorStage,
			ASTSerializationStage>>();

	addMode<
		boost::mpl::vector<
			ThorScriptParserStage,
			StaticTestVerificationStage>>("mode-parse-syntax-only", "for syntax check stage");

	addMode<
		boost::mpl::vector<
			ThorScriptParserStage,
			SemanticVerificationStage0,
			StaticTestVerificationStage>>("mode-semantic-verify-0", "for semantic verification stage 0");

	addMode<
		boost::mpl::vector<
			ThorScriptParserStage,
			LiteralCompactionStage,
			RestructureStage,
			StaticTestVerificationStage>>("mode-xform-stage-only", "for transform stage");

	addMode<
		boost::mpl::vector<
			ThorScriptParserStage,
			SemanticVerificationStage0,
			LiteralCompactionStage,
			RestructureStage,
			ResolutionStage,
			SemanticVerificationStage1,
			StaticTestVerificationStage>>("mode-semantic-verify-1", "for semantic verification stage 1");
}

ThorScriptCompiler::~ThorScriptCompiler()
{ }

} }
