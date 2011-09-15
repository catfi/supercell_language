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

INCLUDE_DIRECTORIES(
    ${PROJECT_COMMON_SOURCE_DIR}/include/
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/
    )

ADD_EXECUTABLE(ThorScriptTreeTest_SemanticVerificationVisitorTest SemanticVerificationVisitorTest.cpp)

TARGET_LINK_LIBRARIES(ThorScriptTreeTest_SemanticVerificationVisitorTest
    zillians-common-core
    )

zillians_add_complex_test(
    TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_WRONG_STATIC_TEST_ANNOTATION_FORMAT_FAIL
    SHELL ${CMAKE_CURRENT_SOURCE_DIR}/expect_fail.sh ${TCC} ${CMAKE_CURRENT_SOURCE_DIR}/data/_WRONG_STATIC_TEST_ANNOTATION_FORMAT_FAIL.t
    DEPENDS ${TCC} ${CMAKE_CURRENT_SOURCE_DIR}/expect_fail.sh
    )

zillians_add_complex_test(
    TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_WRONG_STATIC_TEST_ANNOTATION_FORMAT_PASS
    SHELL ${TCC} ${CMAKE_CURRENT_SOURCE_DIR}/data/_WRONG_STATIC_TEST_ANNOTATION_FORMAT_PASS.t
    DEPENDS ${TCC}
    )

zillians_add_complex_test(
    TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_AMBIGUOUS_REF
    SHELL ${TCC} ${CMAKE_CURRENT_SOURCE_DIR}/data/AMBIGUOUS_REF.t
    DEPENDS ${TCC}
    )

zillians_add_complex_test(
    TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest_CALL_NONFUNC
    SHELL ${TCC} ${CMAKE_CURRENT_SOURCE_DIR}/data/CALL_NONFUNC.t
    DEPENDS ${TCC}
    )

zillians_add_simple_test(TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest)
zillians_add_test_to_subject(SUBJECT thorscript-tree-test TARGET ThorScriptTreeTest_SemanticVerificationVisitorTest)
