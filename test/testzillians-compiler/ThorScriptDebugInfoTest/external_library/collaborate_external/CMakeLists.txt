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

ADD_CUSTOM_COMMAND(OUTPUT print.o
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/native
	COMMAND gcc -g -fPIC -c -o ${CMAKE_CURRENT_BINARY_DIR}/native/print.o ${CMAKE_CURRENT_SOURCE_DIR}/native/print.cpp
	COMMENT "Build print.o")

thorscript_add_debuginfo_test_target(
		TARGET thorscript-compiler-debug-test_collaborate_external1
		BUNDLE collaborate_external.bundle
		ENTRY  test1
		BASELIB ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
		DEPENDS print.o
		SCRIPT test1.py)

thorscript_add_debuginfo_test_target(
		TARGET thorscript-compiler-debug-test_collaborate_external2
		BUNDLE collaborate_external.bundle
		ENTRY  test2
		BASELIB ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
		DEPENDS print.o
		SCRIPT test2.py)

thorscript_add_debuginfo_test_target(
		TARGET thorscript-compiler-debug-test_collaborate_external3
		BUNDLE collaborate_external.bundle
		ENTRY  test3
		BASELIB ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
		DEPENDS print.o
		SCRIPT test3.py)

zillians_add_test_to_subject(SUBJECT thorscript-compiler-debug-test TARGET thorscript-compiler-debug-test_collaborate_external1)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-debug-test TARGET thorscript-compiler-debug-test_collaborate_external2)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-debug-test TARGET thorscript-compiler-debug-test_collaborate_external3)
