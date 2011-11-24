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

#get_target_property(ThorScriptDocGenerator ts-doc LOCATION)

zillians_add_complex_test(
    TARGET thorscript-doc-test
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/test.sh ${ThorScriptDocGenerator}
        "Thor"
        ${CMAKE_CURRENT_SOURCE_DIR}/gold/output
    DEPENDS ${ThorScriptDocGenerator}
    )
ADD_DEPENDENCIES(runtest-thorscript-doc-test EP_ts-doc)
