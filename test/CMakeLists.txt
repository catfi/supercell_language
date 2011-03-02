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

#ADD_SUBDIRECTORY(testzillians-framework-vw-naive)
#ADD_SUBDIRECTORY(testbackend-llvm-ptx)
IF(CUDA_FOUND)
    ADD_SUBDIRECTORY(testcuda)
    ADD_SUBDIRECTORY(testzillians-framework-vw)
ELSE()
    MESSAGE(STATUS "CUDA not available, disabling build for testcuda, testzillians-framework-vw-cuda")
ENDIF()
ADD_SUBDIRECTORY(testzillians-compiler)
ADD_SUBDIRECTORY(testzillians-dep)
ADD_SUBDIRECTORY(zcc-testsuite)

zillians_add_complex_test(
    TARGET  zcc-related-all
    SHELL   make -C ${CMAKE_BINARY_DIR} runtest-zcc-llvm-all runtest-zcc-semantic-check-all runtest-zcc-s0-zcc-framework runtest-zcc-s1-syntax runtest-zcc-s2-semantic runtest-zcc-s3-exec runtest-zcc-s4-pass runtest-zcc-runtime-all
    DEPENDS 
            zcc zcc-bc zcc-flex zcc-client zcc-client-old
    )
