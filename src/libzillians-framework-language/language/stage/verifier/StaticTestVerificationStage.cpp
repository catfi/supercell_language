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

#include "language/stage/verifier/StaticTestVerificationStage.h"
#include "language/stage/verifier/visitor/StaticTestVerificationStageVisitor.h"
#include "language/tree/visitor/general/PrettyPrintVisitor.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

StaticTestVerificationStage::StaticTestVerificationStage() : enabled(false)
{ }

StaticTestVerificationStage::~StaticTestVerificationStage()
{ }

const char* StaticTestVerificationStage::name()
{
	return "static_test_verification_stage";
}

void StaticTestVerificationStage::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{
    option_desc.add_options()
	("enable-static-test", "enable static test for compiler test suite");
}

bool StaticTestVerificationStage::parseOptions(po::variables_map& vm)
{
	enabled = (vm.count("enable-static-test") > 0);

	return true;
}

bool StaticTestVerificationStage::execute(bool& continue_execution)
{
	if(!enabled)
		return true;

	if(!hasParserContext())
		return false;

	ParserContext& parser_context = getParserContext();

	if(parser_context.program)
	{
		visitor::StaticTestVerificationStageVisitor verifier;
		verifier.programNode = parser_context.program;
		verifier.visit(*parser_context.program);
		if(verifier.isAllMatch())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

} } }
