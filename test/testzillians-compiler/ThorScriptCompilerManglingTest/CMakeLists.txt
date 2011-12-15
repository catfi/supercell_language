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
    TARGET thorscript-mangling-test
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/test.sh ${ThorScriptDriver} ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/data/input.t
            "."
            "qwe"
            "_func_"
            ${CMAKE_CURRENT_SOURCE_DIR}/gold/output
    DEPENDS tsc
    )

zillians_add_complex_test(
    TARGET thorscript-mangling-test2
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/test.sh ${ThorScriptDriver} ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/data/_AAA_/input2.t
            "_AAA_"
            "qwe"
            "_func_"
            ${CMAKE_CURRENT_SOURCE_DIR}/gold/output2
    DEPENDS tsc
    )
