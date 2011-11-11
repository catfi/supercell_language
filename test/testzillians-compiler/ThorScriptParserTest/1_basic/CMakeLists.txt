# 
# Zillians MMO
# Copyright (C) 2007-2009 Zillians.com, Inc.
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

zillians_add_complex_test(
    TARGET thorscript-parser-test-auto-basic
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptLogFilter} 3
            ${CMAKE_CURRENT_SOURCE_DIR}/0_global_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/1_global_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/2_class.t
            ${CMAKE_CURRENT_SOURCE_DIR}/3_local_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/4_member_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/5_member_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/6_package.t
            ${CMAKE_CURRENT_SOURCE_DIR}/7_expression.t
            ${CMAKE_CURRENT_SOURCE_DIR}/8_enum.t
    DEPENDS ${ThorScriptCompiler} ${ThorScriptLogFilter}
    )

zillians_add_complex_test(
    TARGET thorscript-parser-test-basic
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptLogFilter} 0
            ${CMAKE_CURRENT_SOURCE_DIR}/0_global_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/1_global_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/2_class.t
            ${CMAKE_CURRENT_SOURCE_DIR}/3_local_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/4_member_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/5_member_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/6_package.t
            ${CMAKE_CURRENT_SOURCE_DIR}/7_expression.t
            ${CMAKE_CURRENT_SOURCE_DIR}/8_enum.t
    DEPENDS ${ThorScriptCompiler} ${ThorScriptLogFilter}
    )

zillians_add_complex_test(
    TARGET thorscript-parser-test-ast-basic
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptLogFilter} 1
            ${CMAKE_CURRENT_SOURCE_DIR}/0_global_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/1_global_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/2_class.t
            ${CMAKE_CURRENT_SOURCE_DIR}/3_local_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/4_member_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/5_member_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/6_package.t
            ${CMAKE_CURRENT_SOURCE_DIR}/7_expression.t
            ${CMAKE_CURRENT_SOURCE_DIR}/8_enum.t
    DEPENDS ${ThorScriptCompiler} ${ThorScriptLogFilter}
    )

zillians_add_complex_test(
    TARGET thorscript-parser-test-xform-basic
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../test.sh ${ThorScriptCompiler} ${ThorScriptLogFilter} 2
            ${CMAKE_CURRENT_SOURCE_DIR}/0_global_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/1_global_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/2_class.t
            ${CMAKE_CURRENT_SOURCE_DIR}/3_local_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/4_member_var.t
            ${CMAKE_CURRENT_SOURCE_DIR}/5_member_func.t
            ${CMAKE_CURRENT_SOURCE_DIR}/6_package.t
            ${CMAKE_CURRENT_SOURCE_DIR}/7_expression.t
            ${CMAKE_CURRENT_SOURCE_DIR}/8_enum.t
    DEPENDS ${ThorScriptCompiler} ${ThorScriptLogFilter}
    )

zillians_add_test_to_subject(SUBJECT thorscript-parser-test-auto-all TARGET thorscript-parser-test-auto-basic)
zillians_add_test_to_subject(SUBJECT thorscript-parser-test-all TARGET thorscript-parser-test-basic)
zillians_add_test_to_subject(SUBJECT thorscript-parser-test-ast-all TARGET thorscript-parser-test-ast-basic)
