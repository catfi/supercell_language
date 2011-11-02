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

#include "language/stage/serialization/ASTDeserializationStage.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/context/ParserContext.h"
#include <boost/filesystem.hpp>

namespace zillians { namespace language { namespace stage {

ASTDeserializationStage::ASTDeserializationStage() : enabled_load(false)
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
		("load-ast", po::value<std::string>(), "load serialized AST file as root");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ASTDeserializationStage::parseOptions(po::variables_map& vm)
{
	if(vm.count("load-ast") > 0)
	{
		enabled_load = true;
		ast_file_to_load = vm["load-ast"].as<std::string>();
	}

	inputs = vm["input"].as<std::vector<std::string>>();

	return true;
}

bool ASTDeserializationStage::execute(bool& continue_execution)
{
	// prepare the global parser context
	if(!hasParserContext())
		setParserContext(new ParserContext());

	if(enabled_load)
	{
		tree::ASTNode* deserialized = ASTSerializationHelper::deserialize(ast_file_to_load);
		if(!deserialized || !tree::isa<tree::Tangle>(deserialized)) return false;

		tree::Tangle* t = tree::cast<tree::Tangle>(deserialized);
		t->markImported(true /*is_imported*/);
		if(getParserContext().tangle)
		{
			getParserContext().tangle->merge(t);
		}
		else
		{
			getParserContext().tangle = t;
		}
	}

	UNUSED_ARGUMENT(continue_execution);

	return true;
}

} } }
