//[[[global
#include "language/stage/stub/ThorScriptStubStage.h"
#include <iostream>

using namespace zillians::language;
using namespace zillians::language::tree;

//[[[entry
template<>
void genStub<stage::ThorScriptStubStage::GAMENAMEGAMEMODULE_MODULE>(Tangle* node)
{
//[[[driver
//]]]
}
//]]]

//]]]

//[[[code
std::wstring GameName = L"DummyGameName";
std::wstring TranslatorUUID = L"DummyTranslatorUUID";
std::wstring ModuleUUID = L"DummyModuleUUID";

//[[[gen(replace:GAME_NAME=GameName, MODULE_UUID=ModuleUUID, TRANSLATOR_UUID=TranslatorUUID)
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
