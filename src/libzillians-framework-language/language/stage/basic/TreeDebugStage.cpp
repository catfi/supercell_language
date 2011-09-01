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

#include "language/stage/basic/TreeDebugStage.h"
#include "language/tree/visitor/general/PrettyPrintVisitor.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

TreeDebugStage::TreeDebugStage() : dump_tree(false)
{ }

TreeDebugStage::~TreeDebugStage()
{ }

const char* TreeDebugStage::name()
{
	return "tree_debug_stage";
}

void TreeDebugStage::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{
    option_desc.add_options()
    ("dump-ast", "dump AST pretty-print for debugging purpose");
}

bool TreeDebugStage::parseOptions(po::variables_map& vm)
{
	dump_tree = (vm.count("dump-ast") > 0);

	return true;
}

bool TreeDebugStage::execute()
{
	if(dump_tree)
	{
		if(getParserContext().program)
		{
			tree::visitor::PrettyPrintVisitor printer;
			printer.visit(*getParserContext().program);
		}
		else
		{
			std::cerr << "empty program node" << std::endl;
		}
	}

	return true;
}

} } }
