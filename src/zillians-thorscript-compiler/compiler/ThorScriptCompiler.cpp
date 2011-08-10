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

#include "core/Prerequisite.h"
#include "utility/UnicodeUtil.h"
#include "compiler/ThorScriptCompiler.h"
#include "compiler/tree/visitor/general/PrettyPrintVisitor.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>

#define STATUS_SUCCESS 0
#define STATUS_ERROR 1

using namespace zillians;
using namespace zillians::compiler;
using namespace zillians::compiler::tree;
using namespace zillians::compiler::tree::visitor;
using namespace zillians::compiler::action;

namespace qi = boost::spirit::qi;
namespace classic = boost::spirit::classic;
namespace po = boost::program_options;

// since '\t' may be printed in spaces and I don't know a way to change std::wcout, we simply replace the tab with desired number of spaces
// so that we can have correct error pointing cursor
// (note that because '\t' equals to 4 spaces reported by spirit, we have to make sure it's printed in the exact same way)
static void _expand_tabs(const std::wstring& input, std::wstring& output, int number_of_space_for_tab = 4)
{
	for(std::wstring::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if(*it == '\t')
			for(int i=0;i<number_of_space_for_tab;++i) output.push_back(L' ');
		else
			output.push_back(*it);
	}
}

void do_parse(std::string filename, bool dump_parse, bool dump_ast)
{
	std::ifstream in(filename, std::ios_base::in);

    // ignore the BOM marking the beginning of a UTF-8 file in Windows
	char c = in.peek();
    if (c == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        if (s != std::string("\xef\xbb\xbf"))
        {
            std::cerr << "Error: Unexpected characters from input file: "
                << filename << std::endl;
            return;
        }
    }

    std::string source_code_raw; // We will read the contents here.
    {
		in.unsetf(std::ios::skipws); // disable white space skipping
		std::copy(std::istream_iterator<char>(in), std::istream_iterator<char>(), std::back_inserter(source_code_raw));
    }

    // convert it from UTF8 into UCS4 as a string by using u8_to_u32_iterator
    std::wstring source_code;
	utf8_to_ucs4(source_code_raw, source_code);

    // enable correct locale so that we can print UCS4 characters
    enable_default_locale(std::wcout);

    // try to parse
	typedef classic::position_iterator2<std::wstring::iterator> pos_iterator_type;
	bool completed;
	try
	{
		pos_iterator_type begin(source_code.begin(), source_code.end(), s_to_ws(filename));
		pos_iterator_type end;

		ThorScriptTreeAction::MemberContext c;
		grammar::ThorScript<pos_iterator_type, ThorScriptTreeAction> parser(dump_parse);
		grammar::detail::WhiteSpace<pos_iterator_type> skipper;

		completed = qi::phrase_parse(
				begin, end,
				parser(&c),
				skipper);

		if(dump_ast)
		{
			std::cout << "<pretty_print>" << std::endl;
			PrettyPrintVisitor printer;
			printer.visit(*c.program);
			std::cout << "</pretty_print>" << std::endl;
		}
	}
	catch (const qi::expectation_failure<pos_iterator_type>& e)
	{
		const classic::file_position_base<std::wstring>& pos = e.first.get_position();

		std::wstring current_line;
		_expand_tabs(e.first.get_currentline(), current_line);
		std::wcerr << L"parse error at file " << pos.file << L" line " << pos.line
				<< L" column " << pos.column << std::endl
				<< L"'" << current_line << L"'" << std::endl
				<< std::setw(pos.column) << L" " << L"^- here" << std::endl;

		completed = false;
	}

	if(completed)
		std::cout << "parse completed" << std::endl;
	else
		std::cout << "parse failed" << std::endl;
}

int main(int argc, char** argv)
{
    po::options_description option_desc;
    po::positional_options_description pos_options_desc;

    option_desc.add_options()
    ("help,h",                               "this help message")
    ("dump-parse,p",                         "dump parse tree for debugging purpose")
    ("dump-ast,t",                           "dump AST pretty-print for debugging purpose")
    ("filename,f", po::value<std::string>(), "filename");
    pos_options_desc.add("filename", -1);

    try
    {
		po::variables_map args;
		po::store(po::command_line_parser(argc, argv).options(option_desc).positional(pos_options_desc).run(), args);
		po::notify(args);

		std::string filename;
		if(args.count("filename")>0)
			filename = args["filename"].as<std::string>();
		bool dump_parse = (args.count("dump-parse")>0);
		bool dump_ast = (args.count("dump-ast")>0);
		if(args.count("help"))
			return STATUS_ERROR;

		if(!filename.empty())
		{
			do_parse(filename, dump_parse, dump_ast);
			return STATUS_SUCCESS;
		}
    }
    catch(...)
    {
    	std::cout << "invalid options" << std::endl;
    }

	std::cout << "no option specified" << std::endl << std::endl;
	std::cout << "command-line options:" << std::endl << std::endl;
	std::cout << option_desc << std::endl;
	return STATUS_ERROR;
}
