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

#include "language/stage/parser/ThorScriptParserStage.h"
#include "language/context/ParserContext.h"
#include "language/grammar/ThorScript.h"
#include "language/action/SemanticActions.h"
#include "language/tree/visitor/general/PrettyPrintVisitor.h"
#include "language/ThorScriptCompiler.h"
#include "utility/Foreach.h"
#include "utility/UnicodeUtil.h"

namespace classic = boost::spirit::classic;
namespace qi = boost::spirit::qi;

namespace zillians { namespace language { namespace stage {

namespace {

// since '\t' may be printed in spaces and I don't know a way to change std::wcout, we simply replace the tab with desired number of spaces
// so that we can have correct error pointing cursor
// (note that because '\t' equals to 4 spaces reported by spirit, we have to make sure it's printed in the exact same way)
static void expand_tabs(const std::wstring& input, std::wstring& output, int number_of_space_for_tab = 4)
{
	for(std::wstring::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if(*it == '\t')
			for(int i=0;i<number_of_space_for_tab;++i) output.push_back(L' ');
		else
			output.push_back(*it);
	}
}

}

ThorScriptParserStage::ThorScriptParserStage() : skip_parse(false), dump_parse(false)
{ }

ThorScriptParserStage::~ThorScriptParserStage()
{ }

const char* ThorScriptParserStage::name()
{
	return "thorscript_parser";
}

void ThorScriptParserStage::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{
    option_desc.add_options()
	("skip-parse",                                      "skip parsing stage and use provided parser context")
    ("dump-parse",										"dump parse tree for debugging purpose")
    ("input,i", po::value<std::vector<std::string>>(),	"thorscript files");

    positional_desc.add("input", -1);
}

bool ThorScriptParserStage::parseOptions(po::variables_map& vm)
{
	skip_parse = (vm.count("skip-parse") > 0);
	dump_parse = (vm.count("dump-parse") > 0);

	if(vm.count("input") > 0)
	{
		inputs = vm["input"].as<std::vector<std::string>>();
		return true;
	}
	else
	{
		return false;
	}
}

bool ThorScriptParserStage::execute()
{
	if(skip_parse)
		return true;

	setParserContext(new ParserContext());

	if(inputs.size() > 0)
	{
		foreach(i, inputs)
		{
			if(!parse(*i)) return false;
		}

		return true;
	}
	else
	{
		return true;
	}
}

bool ThorScriptParserStage::parse(std::string filename)
{
	std::ifstream in(filename, std::ios_base::in);

    // ignore the BOM marking the beginning of a UTF-8 file in Windows
    if(in.peek() == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        if (s != std::string("\xef\xbb\xbf"))
        {
            std::cerr << "parser error: unexpected characters from input file: " << filename << std::endl;
            return false;
        }
    }

    std::string source_code_raw;
	in.unsetf(std::ios::skipws); // disable white space skipping
	std::copy(std::istream_iterator<char>(in), std::istream_iterator<char>(), std::back_inserter(source_code_raw));

    // convert it from UTF8 into UCS4 as a string by using u8_to_u32_iterator
    std::wstring source_code;
	utf8_to_ucs4(source_code_raw, source_code);

    // enable correct locale so that we can print UCS4 characters
    enable_default_locale(std::wcout);

    getParserContext().enable_debug_parser = dump_parse;
    getParserContext().enable_semantic_action = true;

    // try to parse
	typedef classic::position_iterator2<std::wstring::iterator> pos_iterator_type;
	try
	{
		pos_iterator_type begin(source_code.begin(), source_code.end(), s_to_ws(filename));
		pos_iterator_type end;

		grammar::ThorScript<pos_iterator_type, action::ThorScriptTreeAction> parser;
		grammar::detail::WhiteSpace<pos_iterator_type> skipper;

		if(!qi::phrase_parse(
				begin, end,
				parser,
				skipper))
		{
			return false;
		}
	}
	catch (const qi::expectation_failure<pos_iterator_type>& e)
	{
		const classic::file_position_base<std::wstring>& pos = e.first.get_position();

		// TODO output error using Logger
		std::wstring current_line;
		expand_tabs(e.first.get_currentline(), current_line);
		std::wcerr << L"parse error at file " << pos.file << L" line " << pos.line
				<< L" column " << pos.column << std::endl
				<< L"'" << current_line << L"'" << std::endl
				<< std::setw(pos.column) << L" " << L"^- here" << std::endl;

		return false;
	}

	return true;
}

} } }

