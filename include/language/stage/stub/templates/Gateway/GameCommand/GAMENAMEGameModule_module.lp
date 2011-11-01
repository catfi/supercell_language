<?xml version="1.0" encoding="UTF-8"?>

<module id="com.zillians.module.${GAME_NAME}GameModule" 
    name="${GAME_NAME}GameModule" 
    classid=${MODULE_UUID}
    version="0.2">
    <require id="com.zillians.module.world-gateway" version="0.1"/>
    <component id="${GAME_NAME}_GameCommandTranslator" name="${GAME_NAME}_GameCommandTranslator" classid="${TRANSLATOR_UUID}" lazy="false"/>
    <extension point="com.zillians.module.world-gateway.network-translator" component="${GAME_NAME}_GameCommandTranslator"/>
</module>
