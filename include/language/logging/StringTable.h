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

#ifndef ZILLIANS_LANGUAGE_STRINGTABLE_H_
#define ZILLIANS_LANGUAGE_STRINGTABLE_H_

#include "core/Prerequisite.h"
#include "utility/StringUtil.h"
#include "utility/UnicodeUtil.h"
#include "language/tree/ASTNode.h"
#include "language/logging/LoggingManager.h"
#include "language/context/StaticTestContext.h"
#include <boost/parameter.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

namespace zillians { namespace language {


/**
 * StringTable is responsible to:
 *
 * 1. Tables for warning and error string
 */
struct StringTable
{

	enum log_id {
		LOG_UNUSED_VARIABLE,
		LOG_UNDEFINED_VARIABLE,
	};

	enum log_level {
		LOG_LEVEL_WARNING,
		LOG_LEVEL_ERROR,
		LOG_LEVEL_FATAL,
	};

	enum log_category {
		CATEGORY_SYMBOL,
		CATEGORY_FUNCTION,
		CATEGORY_OBJECT_TYPE,
		CATEGORY_MISC,
	};

	struct attribute_table_t {
		attribute_table_t() {}
		attribute_table_t(log_level level, log_category category, uint32 degree, log_id id) :
			level(level), category(category), degree(degree), id(id) {}
		log_level level;
		log_category category;
		uint32 degree;
		log_id id;
	};

	typedef map<string, wstring> locale_translate_map_t;

	// Each message id has its own attributes
	map<log_id, attribute_table_t> attribute_table;

	// Each log id has a translation map, which includes the locale and the specific string mapping
	map<log_id, locale_translate_map_t>  translation_table;


	StringTable()
	{
		attribute_table[LOG_UNDEFINED_VARIABLE] = attribute_table_t(LOG_LEVEL_ERROR, CATEGORY_SYMBOL, 0, LOG_UNDEFINED_VARIABLE);
		attribute_table[LOG_UNUSED_VARIABLE] = attribute_table_t(LOG_LEVEL_WARNING, CATEGORY_SYMBOL, 0, LOG_UNUSED_VARIABLE);
		translation_table[LOG_UNDEFINED_VARIABLE]["zh_TW"] = L"變數 $ID(string) 在 $FILE(string) 中第 $LINE(uint32) 行未被定義";
		translation_table[LOG_UNDEFINED_VARIABLE]["en_US"] = L"Undefined variable $ID(string) in $FILE(string) in $LINE(uint32)";
		translation_table[LOG_UNUSED_VARIABLE]["zh_TW"] = L"$ID(string) 未被使用，$ID(string) 不應該被用";
		translation_table[LOG_UNUSED_VARIABLE]["en_US"] = L"Undefined variable $ID(string) is used, $ID(string) should not be used";
	}
};

// The first parameter in the following function is always node
BOOST_PARAMETER_NAME(node)

BOOST_PARAMETER_NAME(LINE)
BOOST_PARAMETER_NAME(ID)
BOOST_PARAMETER_NAME(FILE)

class Logger
{
public:
	Logger() : mLogger(NULL) {}
	virtual ~Logger() {}

	void setLogger(LoggingManager* logger) { mLogger = logger; }
	void setLocale(locale& locale) { mLocale = locale; }
	void setStringTable(StringTable* string_table) { mStringTable = string_table; }

public:
	BOOST_PARAMETER_MEMBER_FUNCTION( (void), log_undefined_variable, tag, (required (node, (tree::ASTNode&)) (ID,(string))(FILE,(string))(LINE,(uint32)) )) {
		 map<wstring, wstring> parameters;
		 wstring str_level(L"log_level_error");
		 wstring str_id(L"log_undefined_variable");
		 wstring translated = get_translated(StringTable::LOG_UNDEFINED_VARIABLE);
		 substitute(translated, L"$ID(string)", ID);
		 insert_parameters(parameters, L"id", ID);
		 substitute(translated, L"$FILE(string)", FILE);
		 insert_parameters(parameters, L"file", FILE);
		 substitute(translated, L"$LINE(uint32)", LINE);
		 insert_parameters(parameters, L"line", LINE);
		 stage::ErrorInfoContext::set(&node, new stage::ErrorInfoContext(str_level, str_id, parameters));
		 mLogger->log(StringTable::LOG_UNDEFINED_VARIABLE, translated);
	}
	BOOST_PARAMETER_MEMBER_FUNCTION( (void), log_unused_variable, tag, (required (node, (tree::ASTNode&)) (ID,(string)) )) {
		 map<wstring, wstring> parameters;
		 wstring str_level(L"log_level_warning");
		 wstring str_id(L"log_unused_variable");
		 wstring translated = get_translated(StringTable::LOG_UNUSED_VARIABLE);
		 substitute(translated, L"$ID(string)", ID);
		 insert_parameters(parameters, L"id", ID);
		 stage::ErrorInfoContext::set(&node, new stage::ErrorInfoContext(str_level, str_id, parameters));
		 mLogger->log(StringTable::LOG_UNUSED_VARIABLE, translated);
	}

private:
	wstring get_translated(StringTable::log_id id)
	{
		string locale_name = get_lc_message();

		BOOST_ASSERT(mStringTable->translation_table.find(id) != mStringTable->translation_table.end() && "Unknown Log Id");

		StringTable::locale_translate_map_t& translate_map = mStringTable->translation_table[id];
		if (translate_map.find(locale_name) == translate_map.end())
		{
			// use default -- en_US
			locale_name = "en_US";
		}
		return translate_map[locale_name];
	}

	string get_lc_message()
	{
		string lc_all = mLocale.name();
		vector<string> locale_categories = StringUtil::tokenize(lc_all, ";", false);

		for (int i = 0; i < locale_categories.size(); i++)
		{
			// We will have something like this: LC_MESSAGES, en_US.UTF-8
			vector<string> category = StringUtil::tokenize(locale_categories[i], "=", false);
			if (category.size() != 2) continue;
			if (category[0] != "LC_MESSAGES") continue;

			// Now we have lang_encoding = "en_US.UTF-8"
			string lang_encoding = category[1];

			// Strip out ".UTF-8", we only need "en_US"
			string lang = StringUtil::tokenize(lang_encoding, ".", false)[0];
			return lang;
		}

		return "en_US";
	}

	template <typename T>
	bool substitute(wstring &s, const wstring &to_search, const T &to_replace)
	{
		wstringstream stream;
		stream << to_replace;
		return substitute(s, to_search, stream.str());
	}

	bool substitute(wstring &s, const wstring &to_search, string &to_replace)
	{
		return substitute(s, to_search, s_to_ws(to_replace));
	}

	bool substitute(wstring &s, const wstring &to_search, const wstring &to_replace)
	{
		bool replace_found = false;
	    wstring::size_type p = 0;
	    while ((p = s.find(to_search, p)) != wstring::npos)
	    {
	        s.replace(p, to_search.size(), to_replace);
	        p++;
	        replace_found = true;
	    }
	    return replace_found;
	}

	template <typename T>
	void insert_parameters(map<wstring, wstring>& parameters, const wstring& key, const T& value)
	{
		wstringstream stream;
		stream << value;
		insert_parameters(parameters, key, stream.str());
	}

	void insert_parameters(map<wstring, wstring>& parameters, const wstring& key, const string& value)
	{
		insert_parameters(parameters, key, s_to_ws(value));
	}

	void insert_parameters(map<wstring, wstring>& parameters, const wstring& key, const wstring& value)
	{
		parameters[key] = value;
	}

private:
	LoggingManager* mLogger;
	locale mLocale;
	StringTable* mStringTable;
};

}}

#endif /* ZILLIANS_LANGUAGE_STRINGTABLE_H_ */
