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

TreeDebugStage::TreeDebugStage() : dump_ast(false), dump_ast_and_stop(false), dump_ast_location(false)
{ }

TreeDebugStage::~TreeDebugStage()
{ }

const char* TreeDebugStage::name()
{
	return "AST Tree Debugging Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> TreeDebugStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description("AST Tree Debugging Option"));
	shared_ptr<po::options_description> option_desc_private(new po::options_description("AST Tree Debugging Option"));

	option_desc_public->add_options();

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options()
		("dump-ast",          "dump AST pretty-print for debugging purpose")
		("dump-ast-location", "dump source code locations")
		("dump-ast-and-stop", "dump AST pretty-print for debugging purpose and stop processing");

    return std::make_pair(option_desc_public, option_desc_private);
}

bool TreeDebugStage::parseOptions(po::variables_map& vm)
{
	dump_ast          = (vm.count("dump-ast") > 0);
	dump_ast_location = (vm.count("dump-ast-location") > 0);
	dump_ast_and_stop = (vm.count("dump-ast-and-stop") > 0);

	dump_ast |= dump_ast_and_stop;
	return true;
}

bool TreeDebugStage::execute(bool& continue_execution)
{
	continue_execution = !dump_ast_and_stop;
	if(!dump_ast)
		return true;
	if(getParserContext().program)
	{
		tree::visitor::PrettyPrintVisitor printer(dump_ast_location);
		printer.visit(*getParserContext().program);
	}
	else
		std::cerr << "empty program node" << std::endl;
	return true;
}

} } }
