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

#include <boost/filesystem.hpp>
#include "utility/Foreach.h"
#include "language/stage/stub/ThorScriptStubStage.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/tree/ASTNode.h"

#define THORSCRIPT_AST_EXTENSION ".ast"
#define CLIENT_STUB_NAME  "client_stub"
#define SERVER_STUB_NAME  "server_stub"
#define GATEWAY_STUB_NAME "gateway_stub"

using namespace zillians::language::tree;

//extern void gen_client_stub(Package* node);
//extern void gen_server_stub(Package* node);
extern void gen_gateway_stub(Package* node);

namespace zillians { namespace language { namespace stage {

ThorScriptStubStage::ThorScriptStubStage() : stub_type(UNKNOWN_STUB)
{ }

ThorScriptStubStage::~ThorScriptStubStage()
{ }

const char* ThorScriptStubStage::name()
{
	return "thor_script_stub_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptStubStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());
	option_desc_public->add_options()
		("output,o",    po::value<std::string>(), "output stub file name")
		("stub-type,s", po::value<std::string>(), "output stub file type");
	foreach(i, option_desc_public->options()) option_desc_private->add(*i);
	option_desc_private->add_options();
	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptStubStage::parseOptions(po::variables_map& vm)
{
	if(vm.count("input"))
	{
		std::vector<std::string> inputs = vm["input"].as<std::vector<std::string>>();
		foreach(i, inputs)
		{
			boost::filesystem::path file_path(*i);
			std::string extension = file_path.extension().generic_string();
			if(extension == THORSCRIPT_AST_EXTENSION)
				ast_files.push_back(*i);
			std::cout << "filename: " << *i << std::endl;
		}
	}
	if (vm.count("output"))
		output_file = vm["output"].as<std::string>();
	stub_type_t stub_type = UNKNOWN_STUB;
	if(vm.count("stub-type"))
	{
		std::string stub_type_name = vm["stub-type"].as<std::string>();
		if(stub_type_name == CLIENT_STUB_NAME)       stub_type = CLIENT_STUB;
		else if(stub_type_name == SERVER_STUB_NAME)  stub_type = SERVER_STUB;
		else if(stub_type_name == GATEWAY_STUB_NAME) stub_type = GATEWAY_STUB;
	}
	return true;
}

bool ThorScriptStubStage::execute(bool& continue_execution)
{
	foreach(i, ast_files)
    {
        tree::ASTNode* deserialized = ASTSerializationHelper::deserialize(*i);
		if(!deserialized || !tree::isa<tree::Package>(deserialized))
			continue;
		genStub(tree::cast<tree::Package>(deserialized));
    }
	return true;
}

void ThorScriptStubStage::genStub(tree::Package* node)
{
	switch(stub_type)
	{
//	case CLIENT_STUB:  gen_client_stub(node); break;
//	case SERVER_STUB:  gen_server_stub(node); break;
	case GATEWAY_STUB: gen_gateway_stub(node); break;
	}
}

} } }
