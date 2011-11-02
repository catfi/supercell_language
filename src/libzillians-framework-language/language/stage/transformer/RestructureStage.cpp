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

#include "language/stage/transformer/RestructureStage.h"
#include "language/stage/transformer/visitor/RestructureStageVisitor.h"
#include "language/tree/visitor/PrettyPrintVisitor.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

RestructureStage::RestructureStage()
{ }

RestructureStage::~RestructureStage()
{ }

const char* RestructureStage::name()
{
	return "Restructure Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> RestructureStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_private->add_options()("debug-restructure-stage", "debug restructure stage");

	return std::make_pair(option_desc_public, option_desc_private);
}

bool RestructureStage::parseOptions(po::variables_map& vm)
{
	debug = (vm.count("debug-restructure-stage") > 0);

	return true;
}

bool RestructureStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

	if(!hasParserContext())
		return false;

	ParserContext& parser_context = getParserContext();

	if(parser_context.active_source)
	{
		// restructure the entire tree in multiple passes
		while(true)
		{
			visitor::RestructureStageVisitor restruct;
			restruct.visit(*parser_context.tangle);
			if(restruct.hasTransforms())
				restruct.applyTransforms();
			else
				break;
		}

		if(debug)
		{
			tree::visitor::PrettyPrintVisitor printer;
			printer.visit(*parser_context.tangle);
		}

		return true;
	}
	else
	{
		return false;
	}
}

} } }
