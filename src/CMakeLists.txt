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

#ADD_SUBDIRECTORY(libzillians-framework-vw)
#ADD_SUBDIRECTORY(libzillians-framework-vw-cal)
IF(CUDA_FOUND AND CUDPP_FOUND)
    ADD_SUBDIRECTORY(libzillians-framework-vw-cuda)
ELSE()
    MESSAGE(STATUS "CUDA or CUDPP not available, disabling build for libzillians-framework-vw-cuda")
ENDIF()
#ADD_SUBDIRECTORY(libzillians-framework-vw-tbb)
#ADD_SUBDIRECTORY(libzillians-framework-vw-naive) # Not working as of 20100511
IF( BULLET_FOUND )
	ADD_SUBDIRECTORY(libzillians-framework-vw-cpu)
ENDIF()

ADD_SUBDIRECTORY(libzillians-framework-zscript-frontend)
if( LLVM_CONFIG_VERSION VERSION_LESS "2.6" )
	ADD_SUBDIRECTORY(libzillians-framework-zscript-midend)
ENDIF()
ADD_SUBDIRECTORY(libzillians-framework-zscript-backend)
ADD_SUBDIRECTORY(zillians-compiler)
ADD_SUBDIRECTORY(zillians-compiler-grammar)
ADD_SUBDIRECTORY(zillians-ptx-parser)
ADD_SUBDIRECTORY(zillians-ptx-parser-grammar)
if( LLVM_CONFIG_VERSION VERSION_LESS "2.6" )
	ADD_SUBDIRECTORY(libptx-backend)
	ADD_SUBDIRECTORY(zillians-llc)
ENDIF()
