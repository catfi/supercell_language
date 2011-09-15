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

zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-all)

SET(TCC_OPTIONS "--enable-static-test")

zillians_add_complex_test(
    TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_AMBIGUOUS_REF
    SHELL ${TCC} ${TCC_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/data/AMBIGUOUS_REF.t
    DEPENDS ${TCC}
    )

zillians_add_complex_test(
    TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_CALL_NONFUNC
    SHELL ${TCC} ${TCC_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/data/CALL_NONFUNC.t
    DEPENDS ${TCC}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_AMBIGUOUS_REF)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_CALL_NONFUNC)
