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
#include <iostream>
#include <fstream>

#define THORSCRIPT_AST_EXTENSION ".ast"
#define CLIENT_STUB_NAME  "client_stub"
#define SERVER_STUB_NAME  "server_stub"
#define GATEWAY_STUB_NAME "gateway_stub"

using namespace zillians::language::tree;

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
            else
            	BOOST_ASSERT(false && "file must have \".ast\" extension");
        }
    }
    else
        BOOST_ASSERT(false && "must specify input");
    if (vm.count("output"))
        output_file = vm["output"].as<std::string>();
    stub_type = UNKNOWN_STUB;
    if(vm.count("stub-type"))
    {
        std::string stub_type_name = vm["stub-type"].as<std::string>();
        if(stub_type_name == "UNKNOWN_STUB")                            stub_type = UNKNOWN_STUB;
        else if(stub_type_name == "CLIENTCOMMANDOBJECT_H")              stub_type = CLIENTCOMMANDOBJECT_H;
        else if(stub_type_name == "CLOUDCOMMANDOBJECT_H")               stub_type = CLOUDCOMMANDOBJECT_H;
        else if(stub_type_name == "GAMENAME_CLIENTCOMMANDOBJECT_H")     stub_type = GAMENAME_CLIENTCOMMANDOBJECT_H;
        else if(stub_type_name == "GAMENAME_CLOUDCOMMANDOBJECT_H")      stub_type = GAMENAME_CLOUDCOMMANDOBJECT_H;
        else if(stub_type_name == "GAMENAME_GAMECOMMANDTRANSLATOR_CPP") stub_type = GAMENAME_GAMECOMMANDTRANSLATOR_CPP;
        else if(stub_type_name == "GAMENAMEGAMEMODULE_MODULE")          stub_type = GAMENAMEGAMEMODULE_MODULE;
        else
            BOOST_ASSERT(false && "reaching unreachable code");
    }
    return true;
}

bool ThorScriptStubStage::execute(bool& continue_execution)
{
	std::wofstream file;
	std::wstreambuf *prev_rdbuf;
	if(!output_file.empty())
	{
		prev_rdbuf = std::wcout.rdbuf();
		file.open(output_file);
		std::wcout.rdbuf(file.rdbuf());
	}
    foreach(i, ast_files)
    {
        tree::ASTNode* node = ASTSerializationHelper::deserialize(*i);
        if(node && tree::isa<tree::Tangle>(node))
        {
            tree::Tangle* tangle = tree::cast<tree::Tangle>(node);
            switch(stub_type)
            {
            case CLIENTCOMMANDOBJECT_H:              genStub<CLIENTCOMMANDOBJECT_H>(tangle); break;
            case CLOUDCOMMANDOBJECT_H:               genStub<CLOUDCOMMANDOBJECT_H>(tangle); break;
            case GAMENAME_CLIENTCOMMANDOBJECT_H:     genStub<GAMENAME_CLIENTCOMMANDOBJECT_H>(tangle); break;
            case GAMENAME_CLOUDCOMMANDOBJECT_H:      genStub<GAMENAME_CLOUDCOMMANDOBJECT_H>(tangle); break;
            case GAMENAME_GAMECOMMANDTRANSLATOR_CPP: genStub<GAMENAME_GAMECOMMANDTRANSLATOR_CPP>(tangle); break;
            case GAMENAMEGAMEMODULE_MODULE:          genStub<GAMENAMEGAMEMODULE_MODULE>(tangle); break;
            default:
                UNUSED_ARGUMENT(continue_execution);
                BOOST_ASSERT(false && "reaching unreachable code");
                return false;
            }
        }
    }
	if(!output_file.empty())
	{
		std::wcout.rdbuf(prev_rdbuf);
		file.close();
	}
    UNUSED_ARGUMENT(continue_execution);
    return true;
}

} } }
