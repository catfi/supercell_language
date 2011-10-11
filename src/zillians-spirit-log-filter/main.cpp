#include <iostream>
#include <sstream>
#include <iomanip>
#include <ostream>

#define TIXML_USE_TICPP
#include <ticpp/ticpp.h>

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define ENABLE_BOOST_PROPERTY_TREE

#define INDENT_CHAR  ' '
#define INDENT_WIDTH 2
#define DOT_CHAR     '-'
#define DOT_WIDTH    0
#define NEW_STYLE_TRY_SUCCESS

using boost::property_tree::ptree;

enum PARSE_STATE
{
	STATE_ANY,
	STATE_TRY,
	STATE_SUCCESS,
	STATE_FAIL,
	STATE_ATTRIB
};

static void _replace_string(std::string &s, const std::string &search_string, const std::string &replace_string)
{
    for(std::string::size_type p = 0; (p = s.find(search_string, p)) != std::string::npos; !replace_string.empty() && p++)
        s.replace(p, search_string.size(), replace_string);
}

static void _filter_whitespace(std::string &s)
{
	_replace_string(s, "\\n", "");
	_replace_string(s, " ", "");
}

static std::basic_ostream<char> &_set_indent(std::basic_ostream<char> &ss, size_t depth)
{
	return ss << std::setfill(INDENT_CHAR) << std::setw(INDENT_WIDTH*depth) << "";
}

#ifdef ENABLE_BOOST_PROPERTY_TREE

std::string parse_elem_bpp(ptree::value_type& elem, PARSE_STATE* parse_state, int depth)
{
	static std::string prev_elem_text;
	std::stringstream ss;
	std::string elem_value = elem.first;
	if(elem_value == "try")
	{
		*parse_state = STATE_TRY;
		std::string raw_elem_text = elem.second.data();
		std::string elem_text = raw_elem_text;
		_filter_whitespace(elem_text);
		if(!elem_text.empty() && elem_text != prev_elem_text)
		{
#ifdef NEW_STYLE_TRY_SUCCESS
			_set_indent(ss, depth) << raw_elem_text << std::endl;
			_set_indent(ss, depth) << '^' << std::string(DOT_WIDTH, DOT_CHAR) << " try" << std::endl;
#else
			_set_indent(ss, depth) << '<' << elem_value << '>' << raw_elem_text << "</" << elem_value << '>' << std::endl;
#endif
		}
		prev_elem_text = elem_text;
	}
	else if(elem_value == "success")
	{
		*parse_state = STATE_SUCCESS;
		std::string raw_elem_text = elem.second.data();
		std::string elem_text = raw_elem_text;
		_filter_whitespace(elem_text);
		if(!elem_text.empty() && elem_text != prev_elem_text)
		{
#ifdef NEW_STYLE_TRY_SUCCESS
			_set_indent(ss, depth) << raw_elem_text << std::endl;
			_set_indent(ss, depth) << '^' << std::string(DOT_WIDTH, DOT_CHAR) << " success" << std::endl;
#else
			_set_indent(ss, depth) << '<' << elem_value << '>' << raw_elem_text << "</" << elem_value << '>' << std::endl;
#endif
		}
		prev_elem_text = elem_text;
	}
	else if(elem_value == "fail")
	{
		*parse_state = STATE_FAIL;
	}
	else if(elem_value == "attributes")
	{
		*parse_state = STATE_ATTRIB;
	}
	else
	{
		ptree inner_node = elem.second;
		if(inner_node.empty())
			_set_indent(ss, depth) << '<' << elem_value << "/>" << std::endl; // NOTE: never occurs due to "try/success/fail/attribute"
		else
		{
			_set_indent(ss, depth) << '<' << elem_value << '>' << std::endl;
			for(ptree::iterator p = inner_node.begin(); p != inner_node.end(); p++)
			{
				PARSE_STATE parse_state = STATE_ANY;
				std::string s = parse_elem_bpp(*p, &parse_state, depth+1);
				if(parse_state == STATE_FAIL)
					return "";
				ss << s;
			}
			_set_indent(ss, depth) << "</" << elem_value << '>' << std::endl;
		}
	}
	return ss.str();
}

#else

std::string parse_elem_ticpp(ticpp::Element* elem, PARSE_STATE* parse_state, int depth)
{
	static std::string prev_elem_text;
	std::stringstream ss;
	std::string elem_value = elem->Value();
	if(elem_value == "try")
	{
		*parse_state = STATE_TRY;
		std::string raw_elem_text = elem->GetText(false);
		std::string elem_text = raw_elem_text;
		_filter_whitespace(elem_text);
		if(!elem_text.empty() && elem_text != prev_elem_text)
		{
#ifdef NEW_STYLE_TRY_SUCCESS
			_set_indent(ss, depth) << raw_elem_text << std::endl;
			_set_indent(ss, depth) << '^' << std::string(DOT_WIDTH, DOT_CHAR) << " try" << std::endl;
#else
			_set_indent(ss, depth) << '<' << elem_value << '>' << raw_elem_text << "</" << elem_value << '>' << std::endl;
#endif
		}
		prev_elem_text = elem_text;
	}
	else if(elem_value == "success")
	{
		*parse_state = STATE_SUCCESS;
		std::string raw_elem_text = elem->GetText(false);
		std::string elem_text = raw_elem_text;
		_filter_whitespace(elem_text);
		if(!elem_text.empty() && elem_text != prev_elem_text)
		{
#ifdef NEW_STYLE_TRY_SUCCESS
			_set_indent(ss, depth) << raw_elem_text << std::endl;
			_set_indent(ss, depth) << '^' << std::string(DOT_WIDTH, DOT_CHAR) << " success" << std::endl;
#else
			_set_indent(ss, depth) << '<' << elem_value << '>' << raw_elem_text << "</" << elem_value << '>' << std::endl;
#endif
		}
		prev_elem_text = elem_text;
	}
	else if(elem_value == "fail")
	{
		*parse_state = STATE_FAIL;
	}
	else if(elem_value == "attributes")
	{
		*parse_state = STATE_ATTRIB;
	}
	else
	{
		if(elem->NoChildren())
			_set_indent(ss, depth) << '<' << elem_value << "/>" << std::endl; // NOTE: never occurs due to "try/success/fail/attribute"
		else
		{
			_set_indent(ss, depth) << '<' << elem_value << '>' << std::endl;
			ticpp::Iterator<ticpp::Element> p; // NOTE: must declare on line separate from for loop
			for(p = p.begin(elem); p != p.end(); p++)
			{
				PARSE_STATE parse_state = STATE_ANY;
				std::string s = parse_elem_ticpp(p.Get(), &parse_state, depth+1);
				if(parse_state == STATE_FAIL)
					return "";
				ss << s;
			}
			_set_indent(ss, depth) << "</" << elem_value << '>' << std::endl;
		}
	}
	return ss.str();
}

#endif

int main(int argc, char** argv)
{
	if(argc != 2)
		return 0;
	char* pFilename = argv[1];
	try
	{
		PARSE_STATE parse_state = STATE_ANY;
#ifdef ENABLE_BOOST_PROPERTY_TREE
	    ptree pt;
	    read_xml(pFilename, pt);
		std::string s = parse_elem_bpp(*pt.begin(), &parse_state, 0);
#else
		ticpp::Document doc(pFilename);
		doc.LoadFile();
		std::string s = parse_elem_ticpp(dynamic_cast<ticpp::Node*>(&doc)->FirstChildElement(), &parse_state, 0);
#endif
		_replace_string(s, "\\n", "");
		if(parse_state == STATE_FAIL)
			return -1;
		std::cout << s;
	}
	catch(ticpp::Exception& ex)
	{
		std::cout << ex.what();
		return -1;
	}

	return 0;
}
