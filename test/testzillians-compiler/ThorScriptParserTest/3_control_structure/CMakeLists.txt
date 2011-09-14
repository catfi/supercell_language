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
    TARGET thorscript-parser-test-control-structure
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/test.sh ${TCC} ${SPIRIT_LOG_FILTER} 0
            ${CMAKE_CURRENT_SOURCE_DIR}/0_if_elif_else.t
            ${CMAKE_CURRENT_SOURCE_DIR}/1_switch_case_default.t
            ${CMAKE_CURRENT_SOURCE_DIR}/2_while_dowhile.t
            ${CMAKE_CURRENT_SOURCE_DIR}/3_foreach.t
            ${CMAKE_CURRENT_SOURCE_DIR}/4_for.t
    DEPENDS ${TCC} ${SPIRIT_LOG_FILTER}
    )

zillians_add_complex_test(
    TARGET thorscript-parser-test-ast-control-structure
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/test.sh ${TCC} ${SPIRIT_LOG_FILTER} 1
            ${CMAKE_CURRENT_SOURCE_DIR}/0_if_elif_else.t
            ${CMAKE_CURRENT_SOURCE_DIR}/1_switch_case_default.t
            ${CMAKE_CURRENT_SOURCE_DIR}/2_while_dowhile.t
            ${CMAKE_CURRENT_SOURCE_DIR}/3_foreach.t
            ${CMAKE_CURRENT_SOURCE_DIR}/4_for.t
    DEPENDS ${TCC} ${SPIRIT_LOG_FILTER}
    )

zillians_add_test_to_subject(SUBJECT thorscript-parser-test-all TARGET thorscript-parser-test-control-structure)
zillians_add_test_to_subject(SUBJECT thorscript-parser-test-ast-all TARGET thorscript-parser-test-ast-control-structure)
