//[[[global
#include "language/stage/stub/ThorScriptStubStage.h"
#include "utility/UnicodeUtil.h"
#include <iostream>

using namespace zillians::language;
using namespace zillians::language::tree;

//[[[entry
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
    //[[[driver
    //]]]
}
//]]]

//]]]

//[[[code
std::wstring game_name       = var_map[L"game-name"];
std::wstring translator_uuid = var_map[L"translator-uuid"];
std::wstring module_uuid     = var_map[L"module-uuid"];

//[[[gen(replace:GAME_NAME=game_name, TRANSLATOR_UUID=translator_uuid, MODULE_UUID=module_uuid)
<?xml version="1.0" encoding="UTF-8"?>

<module id="com.zillians.module.${GAME_NAME}GameModule"
    name="${GAME_NAME}GameModule" 
    classid=${MODULE_UUID}
    version="0.2">
    <require id="com.zillians.module.world-gateway" version="0.1"/>
    <component id="${GAME_NAME}_GameCommandTranslator" name="${GAME_NAME}_GameCommandTranslator" classid="${TRANSLATOR_UUID}" lazy="false"/>
    <extension point="com.zillians.module.world-gateway.network-translator" component="${GAME_NAME}_GameCommandTranslator"/>
</module>
//]]]

//]]]
