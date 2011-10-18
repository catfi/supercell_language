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

#include "language/stage/serialization/ASTSerializationStage.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

ASTSerializationStage::ASTSerializationStage() : enabled(false)
{ }

ASTSerializationStage::~ASTSerializationStage()
{ }

const char* ASTSerializationStage::name()
{
	return "AST Serialization Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ASTSerializationStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("emit-ast", po::value<std::string>(), "emit AST file");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ASTSerializationStage::parseOptions(po::variables_map& vm)
{
	enabled = (vm.count("emit-ast") > 0);
	if(enabled)
	{
		ast_file = vm["emit-ast"].as<std::string>();
	}

	return true;
}

bool ASTSerializationStage::execute(bool& continue_execution)
{
	if(!enabled)
		return true;

	if(!hasParserContext())
		return false;

	if(!ASTSerializationHelper::serialize(ast_file, getParserContext().program->root))
		return false;

	return true;
}

} } }
