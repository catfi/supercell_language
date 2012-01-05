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

#include "language/stage/transformer/LiteralCompactionStage.h"
#include "language/stage/transformer/visitor/LiteralCompactionStageVisitor.h"
#include "language/tree/visitor/PrettyPrintVisitor.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

LiteralCompactionStage::LiteralCompactionStage() : dump_graphviz(false)
{ }

LiteralCompactionStage::~LiteralCompactionStage()
{ }

const char* LiteralCompactionStage::name()
{
	return "Literal Compaction Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> LiteralCompactionStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_private->add_options()
        ("debug-literal-compaction-stage", "debug literal compaction stage")
		//("dump-graphviz", "dump AST in graphviz format")
		//("dump-graphviz-dir", po::value<std::string>(), "dump AST in graphviz format")
    ;

	return std::make_pair(option_desc_public, option_desc_private);
}

bool LiteralCompactionStage::parseOptions(po::variables_map& vm)
{
	debug = (vm.count("debug-literal-compaction-stage") > 0);
	dump_graphviz = (vm.count("dump-graphviz") > 0);
    if(vm.count("dump-graphviz-dir") > 0)
    {
        dump_graphviz_dir = vm["dump-graphviz-dir"].as<std::string>();
    }

	return true;
}

bool LiteralCompactionStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

	if(!hasParserContext())
		return false;

	ParserContext& parser_context = getParserContext();

	if(parser_context.active_source)
	{
		visitor::LiteralCompactionStageVisitor compactor;
		compactor.visit(*parser_context.active_source);

		if(debug)
		{
			tree::visitor::PrettyPrintVisitor printer;
			printer.visit(*parser_context.active_source);
		}

        if(dump_graphviz)
        {
            boost::filesystem::path p(dump_graphviz_dir);
            ASTNodeHelper::visualize(getParserContext().tangle, p / "post-literal-compaction.dot");
        }

		return true;
	}
	else
	{
		return false;
	}
}

} } }
