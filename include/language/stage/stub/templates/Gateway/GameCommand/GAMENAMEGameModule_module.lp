//[[[global
#include "language/stage/stub/ThorScriptStubStage.h"
#include <iostream>

using namespace zillians::language;
using namespace zillians::language::tree;

//[[[entry
template<>
void genStub<stage::ThorScriptStubStage::GAMENAMEGAMEMODULE_MODULE>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
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
