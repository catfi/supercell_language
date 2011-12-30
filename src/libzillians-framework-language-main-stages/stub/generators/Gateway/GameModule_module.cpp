#include "language/stage/stub/ThorScriptStubStage.h"
#include "utility/UnicodeUtil.h"
#include <iostream>
using namespace zillians::language;
using namespace zillians::language::tree;
	template<>
std::string get_stub_filename<stage::ThorScriptStubStage::GATEWAY_GAMECOMMAND_GAMEMODULE_MODULE>(stage::ThorScriptStubStage::var_map_t& var_map)
{
	auto f = [&](std::wstring &s, std::wstring search_string, std::wstring replace_string){
		for(std::wstring::size_type p = 0; (p = s.find(search_string, p)) != std::wstring::npos; p++)
			s.replace(p, search_string.size(), replace_string);
	};
	std::wstring s = L"${GAME_NAME}GAMEMODULE.module";
	f(s, L"${GAME_NAME}", var_map[L"game-name"]);
	return zillians::ws_to_s(s);
}
	template<>
void print_stub<stage::ThorScriptStubStage::GATEWAY_GAMECOMMAND_GAMEMODULE_MODULE>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
	std::wcout <<
		"\n"
		"\n"
		"";
	std::wstring game_name       = var_map[L"game-name"];
	std::wstring translator_uuid = var_map[L"translator-uuid"];
	std::wstring module_uuid     = var_map[L"module-uuid"];
	std::wcout <<
		"\n"
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"\n"
		"<module id=\"com.zillians.module.";
	std::wcout << game_name;
	std::wcout <<
		"GameModule\"\n"
		"    name=\"";
	std::wcout << game_name;
	std::wcout <<
		"GameModule\" \n"
		"    classid=";
	std::wcout << module_uuid;
	std::wcout <<
		"\n"
		"    version=\"0.2\">\n"
		"    <require id=\"com.zillians.module.world-gateway\" version=\"0.1\"/>\n"
		"    <component id=\"";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator\" name=\"";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator\" classid=\"";
	std::wcout << translator_uuid;
	std::wcout <<
		"\" lazy=\"false\"/>\n"
		"    <extension point=\"com.zillians.module.world-gateway.network-translator\" component=\"";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator\"/>\n"
		"</module>\n"
		"";
	std::wcout <<
		"\n"
		"";
}
