/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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
/**
 * @date Jul 18, 2011 sdk - Initial version created.
 */

#include "compiler/ThorScriptCompiler.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <iostream>

namespace bp = boost::program_options;

int main(int argc, char** argv)
{
    bp::options_description option_desc;
    bp::positional_options_description pos_options_desc;

    option_desc.add_options()
    ("help,h",                               "this help message")
    ("dump-parse,p",                         "dump parse tree for debugging purpose")
    ("dump-ast,t",                           "dump AST pretty-print for debugging purpose")
    ("filename,f", bp::value<std::string>(), "filename");
    pos_options_desc.add("filename", -1);

	bp::variables_map args;
    try
    {
		bp::store(bp::command_line_parser(argc, argv).options(option_desc).positional(pos_options_desc).run(), args);
		bp::notify(args);
    }
    catch(...)
    {
    	std::cout << "invalid options" << std::endl;
    }

	std::string filename;
	if(args.count("filename")>0)
		filename = args["filename"].as<std::string>();

	bool dump_parse = (args.count("dump-parse")>0);
	bool dump_ast = (args.count("dump-ast")>0);

	if(args.count("help"))
	{
		std::cout << "command-line options:" << std::endl << std::endl;
		std::cout << option_desc;
		return 0;
	}

	if(!filename.empty())
	{
		zillians::compiler::ThorScriptCompiler compiler;
		if(!compiler.parse(filename, dump_parse, dump_ast))
		{
			std::cout << "parse fail" << std::endl;
			return -1;
		}
		std::cout << "parse success" << std::endl;
		return 0;
	}

	std::cout << "no option specified" << std::endl << std::endl;
	std::cout << "command-line options:" << std::endl << std::endl;
	std::cout << option_desc << std::endl;
	return -1;
}
