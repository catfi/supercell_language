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

INCLUDE_DIRECTORIES(
    ${PROJECT_COMMON_SOURCE_DIR}/include/
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/
)

ADD_EXECUTABLE( ThorScriptDriverTest_HappyPathTest
    DriverHappyPathTest.cpp
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/src/zillians-thorscript-driver/language/ThorScriptDriver.cpp
)

TARGET_LINK_LIBRARIES(ThorScriptDriverTest_HappyPathTest
    zillians-common-core
    zillians-common-utility
    zillians-framework-language
)

zillians_add_complex_test(
    TARGET thorscript-driver-test-happypath
    DEPENDS ThorScriptDriverTest_HappyPathTest
    SHELL ThorScriptDriverTest_HappyPathTest --project-path=${CMAKE_CURRENT_SOURCE_DIR}/ --build-path=./
)

zillians_add_test_to_subject(SUBJECT thorscript-driver-test-all TARGET thorscript-driver-test-happypath)
