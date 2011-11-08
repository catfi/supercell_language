# 
# Zillians MMO
# Copyright (C) 2007-2010 Zillians.com, Inc.
# For more information see http:#www.zillians.com
#
# Zillians MMO is the library and runtime for massive multiplayer online game
# development in utility computing model, which runs as a service for every 
# developer to build their virtual world running on our GPU-assisted machines
#
# This is a close source library intended to be used solely within Zillians.com
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
# AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
#
# Contact Information: info@zillians.com
#

zillians_create_test_subject(SUBJECT thorscript-stub-test-all)

zillians_create_test_subject(SUBJECT thorscript-stub-test-gateway)
#zillians_create_test_subject(SUBJECT thorscript-stub-test-client)
#zillians_create_test_subject(SUBJECT thorscript-stub-test-server)

zillians_add_subject_to_subject(PARENT thorscript-stub-test-all CHILD thorscript-stub-test-gateway)
#zillians_add_subject_to_subject(PARENT thorscript-stub-test-all CHILD thorscript-stub-test-client)
#zillians_add_subject_to_subject(PARENT thorscript-stub-test-all CHILD thorscript-stub-test-server)

zillians_add_complex_test(
    TARGET thorscript-stub-test-gateway-CLIENTCOMMANDOBJECT_H
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptStubGenerator}
            ${CMAKE_CURRENT_SOURCE_DIR}/data/input.t
            "CLIENTCOMMANDOBJECT_H"
            ${CMAKE_CURRENT_SOURCE_DIR}/gold/DummyGameName_CLIENTCOMMANDOBJECT_H_output
            ${ZILLIANS_SCRIPT_PATH}/format_source.sh
    DEPENDS ${ThorScriptCompiler} ${ThorScriptStubGenerator}
    )
zillians_add_complex_test(
    TARGET thorscript-stub-test-gateway-CLOUDCOMMANDOBJECT_H
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptStubGenerator}
            ${CMAKE_CURRENT_SOURCE_DIR}/data/input.t
            "CLOUDCOMMANDOBJECT_H"
            ${CMAKE_CURRENT_SOURCE_DIR}/gold/DummyGameName_CLOUDCOMMANDOBJECT_H_output
            ${ZILLIANS_SCRIPT_PATH}/format_source.sh
    DEPENDS ${ThorScriptCompiler} ${ThorScriptStubGenerator}
    )
zillians_add_complex_test(
    TARGET thorscript-stub-test-gateway-GAMECOMMANDTRANSLATOR_CPP
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptStubGenerator}
            ${CMAKE_CURRENT_SOURCE_DIR}/data/input.t
            "GAMECOMMANDTRANSLATOR_CPP"
            ${CMAKE_CURRENT_SOURCE_DIR}/gold/DummyGameName_GAMECOMMANDTRANSLATOR_CPP_output
            ${ZILLIANS_SCRIPT_PATH}/format_source.sh
    DEPENDS ${ThorScriptCompiler} ${ThorScriptStubGenerator}
    )
zillians_add_complex_test(
    TARGET thorscript-stub-test-gateway-GAMEMODULE_MODULE
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptStubGenerator}
            ${CMAKE_CURRENT_SOURCE_DIR}/data/input.t
            "GAMEMODULE_MODULE"
            ${CMAKE_CURRENT_SOURCE_DIR}/gold/DummyGameNameGAMEMODULE_MODULE_output
            ${ZILLIANS_SCRIPT_PATH}/format_source.sh
    DEPENDS ${ThorScriptCompiler} ${ThorScriptStubGenerator}
    )

zillians_add_test_to_subject(SUBJECT thorscript-stub-test-gateway TARGET thorscript-stub-test-gateway-CLIENTCOMMANDOBJECT_H)
zillians_add_test_to_subject(SUBJECT thorscript-stub-test-gateway TARGET thorscript-stub-test-gateway-CLOUDCOMMANDOBJECT_H)
zillians_add_test_to_subject(SUBJECT thorscript-stub-test-gateway TARGET thorscript-stub-test-gateway-GAMECOMMANDTRANSLATOR_CPP)
zillians_add_test_to_subject(SUBJECT thorscript-stub-test-gateway TARGET thorscript-stub-test-gateway-GAMEMODULE_MODULE)
