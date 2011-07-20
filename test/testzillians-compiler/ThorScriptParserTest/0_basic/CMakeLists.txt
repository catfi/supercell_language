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
    TARGET thor-script-parser-test-basic
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/test.sh ${TCC}
        ${CMAKE_CURRENT_SOURCE_DIR}/0_class_def.t
        ${CMAKE_CURRENT_SOURCE_DIR}/1_member_var.t
        ${CMAKE_CURRENT_SOURCE_DIR}/2_member_var_init.t
        ${CMAKE_CURRENT_SOURCE_DIR}/3_member_func.t
        ${CMAKE_CURRENT_SOURCE_DIR}/4_member_func_call.t
        ${CMAKE_CURRENT_SOURCE_DIR}/5_auto_var.t
        ${CMAKE_CURRENT_SOURCE_DIR}/6_auto_var_init.t
        ${CMAKE_CURRENT_SOURCE_DIR}/7_scoped_var.t
        ${CMAKE_CURRENT_SOURCE_DIR}/8_scoped_var_init.t
        ${CMAKE_CURRENT_SOURCE_DIR}/9_package_import.t
        ${CMAKE_CURRENT_SOURCE_DIR}/10_package_def.t
        ${CMAKE_CURRENT_SOURCE_DIR}/11_basic_arithmetic.t
    DEPENDS ${TCC}
    )

ADD_DEPENDENCIES(runtest-thor-script-parser-test-all runtest-thor-script-parser-test-basic)
