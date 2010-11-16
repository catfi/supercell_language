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

ADD_SUBDIRECTORY(libzillians-framework-vw)

IF(CUDA_FOUND AND CUDPP_FOUND)
    ADD_SUBDIRECTORY(libzillians-framework-vw-processors-fermi)
    ADD_SUBDIRECTORY(libzillians-framework-vw-services-fermi)
    ADD_SUBDIRECTORY(libzillians-framework-vw-api-fermi)
ELSE()
    MESSAGE(STATUS "CUDA or CUDPP not available, disabling build for libzillians-framework-vw-processors-fermi and libzillians-framework-vw-services-fermi")
ENDIF()

IF(RDMA_FOUND)
    ADD_SUBDIRECTORY(libzillians-framework-vw-processors-gateway)
ENDIF()

IF(THRIFT_FOUND)
    ADD_SUBDIRECTORY(libzillians-framework-vw-coordinators)
    ADD_SUBDIRECTORY(libzillians-framework-vw-processors-database)
ENDIF()

ADD_SUBDIRECTORY(libzillians-framework-vw-processors-mt)
ADD_SUBDIRECTORY(libzillians-framework-vw-services-mt)
ADD_SUBDIRECTORY(libzillians-framework-vw-api-mt)

#ADD_SUBDIRECTORY(libzillians-framework-vw-processors-st)
#ADD_SUBDIRECTORY(libzillians-framework-vw-services-st)
#ADD_SUBDIRECTORY(libzillians-framework-vw-api-st)

if( LLVM_FOUND )
	ADD_SUBDIRECTORY(libzillians-framework-vw-mapper-llvm)
endif()
ADD_SUBDIRECTORY(libzillians-framework-vw-mapper-ptx)

IF(ICU_FOUND)
    ADD_SUBDIRECTORY(libzillians-framework-zscript-frontend)
ENDIF()

ADD_SUBDIRECTORY(libzillians-framework-zscript-midend)

if( LLVM_FOUND )
	ADD_SUBDIRECTORY(libzillians-framework-zscript-midend-llvm)
endif()

IF(ANTLR3_FOUND )
	ADD_SUBDIRECTORY(zillians-compiler-grammar)
	ADD_SUBDIRECTORY(zillians-compiler)
ENDIF(ANTLR3_FOUND )
#ADD_SUBDIRECTORY(zillians-zs-parser)

ADD_SUBDIRECTORY(libzillians-framework-zscript-backend)

ADD_SUBDIRECTORY(zillians-ptx-parser)
ADD_SUBDIRECTORY(zillians-ptx-parser-grammar)

ADD_SUBDIRECTORY(libptx-backend)
ADD_SUBDIRECTORY(zillians-llc)

#ADD_SUBDIRECTORY(zillians-buffer-generator)
