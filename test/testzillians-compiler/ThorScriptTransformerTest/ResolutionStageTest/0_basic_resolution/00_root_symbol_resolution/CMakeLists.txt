# 
# Zillians MMO
# Copyright (C) 2007-2011 Zillians.com, Inc.
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
    TARGET thorscript-compiler-transform-test-00
    SHELL ${ThorScriptCompiler} ${compile_options_resolution} ${CMAKE_CURRENT_SOURCE_DIR}/00.t
    DEPENDS ${ThorScriptCompiler}
    )
    
zillians_add_test_to_subject(SUBJECT thorscript-transformer-resolution-test-all TARGET thorscript-compiler-transform-test-00)
