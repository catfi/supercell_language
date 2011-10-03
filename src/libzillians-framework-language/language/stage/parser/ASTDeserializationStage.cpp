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

#include "language/stage/parser/ASTDeserializationStage.h"
#include "language/context/ParserContext.h"
#include "language/tree/ASTNodeSerialization.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace zillians { namespace language { namespace stage {

ASTDeserializationStage::ASTDeserializationStage() : enabled(false)
{ }

ASTDeserializationStage::~ASTDeserializationStage()
{ }

const char* ASTDeserializationStage::name()
{
	return "AST Deserialization Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ASTDeserializationStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("load-ast", po::value<std::string>(), "load serialized AST file");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ASTDeserializationStage::parseOptions(po::variables_map& vm)
{
	enabled = (vm.count("load-ast") > 0);
	if(enabled)
	{
		ast_file = vm["load-ast"].as<std::string>();
	}
	return true;
}

bool ASTDeserializationStage::execute(bool& continue_execution)
{
	if(!enabled)
		return true;

	// prepare the global parser context
	if(!hasParserContext())
		setParserContext(new ParserContext());

    std::ifstream ifs(ast_file);
    if(!ifs.good()) return false;

    boost::archive::text_iarchive ia(ifs);
    tree::ASTNode* from_serialize = NULL;
    ia >> from_serialize;

    getParserContext().program = tree::cast<tree::Program>(from_serialize);

	return true;
}

} } }
