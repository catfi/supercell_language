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

ADD_SUBDIRECTORY(zillians-buffer-generator)

ADD_SUBDIRECTORY(libzillians-framework-vw)
ADD_SUBDIRECTORY(libzillians-framework-vw-api-internal)
ADD_SUBDIRECTORY(libzillians-framework-vw-api-mt)
ADD_SUBDIRECTORY(libzillians-framework-vw-processors-logger)

IF(CUDA_FOUND AND THRUST_FOUND AND ENABLE_FEATURE_CUDA)
    #ADD_SUBDIRECTORY(libzillians-framework-vw-processors-fermi)
    #ADD_SUBDIRECTORY(libzillians-framework-vw-services-fermi)
    ADD_SUBDIRECTORY(libzillians-framework-vw-api-fermi)
ELSE()
    MESSAGE(STATUS "disabling build for libzillians-framework-vw-processors-fermi")
    MESSAGE(STATUS "disabling build for libzillians-framework-vw-services-fermi")
    MESSAGE(STATUS "disabling build for libzillians-framework-vw-api-fermi")
ENDIF()

IF(CUDA_FOUND AND THRUST_FOUND AND ENABLE_FEATURE_CUDA)
    ADD_SUBDIRECTORY(zillians-gpu-tools)
ELSE()
    MESSAGE(STATUS "disabling build for libzillians-framework-vw-api-mt")
    MESSAGE(STATUS "disabling build for zillians-gpu-tools")
ENDIF()

IF(RDMA_FOUND AND ENABLE_FEATURE_RDMA)
    ADD_SUBDIRECTORY(libzillians-framework-vw-processors-gateway)
ENDIF()

IF(THRIFT_FOUND)
    ADD_SUBDIRECTORY(libzillians-framework-vw-coordinators)
    ADD_SUBDIRECTORY(libzillians-framework-vw-processors-database)
ENDIF()

ADD_SUBDIRECTORY(libzillians-framework-vw-processors)
ADD_SUBDIRECTORY(libzillians-framework-vw-services)
    

if( LLVM_FOUND )
	ADD_SUBDIRECTORY(libzillians-framework-vw-mapper-llvm)
endif()
#ADD_SUBDIRECTORY(libzillians-framework-vw-mapper-ptx)

IF(ANTLR3_FOUND )
	if( LLVM_FOUND )
		ADD_SUBDIRECTORY(libzillians-framework-zscript-midend-llvm)
	endif()

	ADD_SUBDIRECTORY(libzillians-framework-zscript-midend)
    ADD_SUBDIRECTORY(libzillians-framework-zscript-frontend)
	ADD_SUBDIRECTORY(zillians-compiler-grammar)
	ADD_SUBDIRECTORY(zillians-compiler)
#ADD_SUBDIRECTORY(zillians-zs-parser)
	ADD_SUBDIRECTORY(libzillians-framework-zscript-backend)

	ADD_SUBDIRECTORY(zillians-llc)

	ADD_SUBDIRECTORY( zillians-compiler-collection )
	ADD_SUBDIRECTORY( zillians-zscript-compileto-stages )
	ADD_SUBDIRECTORY( zillians-zscript-compileto-tpl )
	ADD_SUBDIRECTORY( zillians-zscript-support )
ENDIF(ANTLR3_FOUND)

ADD_SUBDIRECTORY(zillians-ptx-parser)
ADD_SUBDIRECTORY(zillians-ptx-parser-grammar)

ADD_SUBDIRECTORY(libptx-backend)
ADD_SUBDIRECTORY(lib-cuda-backend)

ADD_SUBDIRECTORY(zillians-crypto)
ADD_SUBDIRECTORY(zillians-stub-gen)

ADD_SUBDIRECTORY(zillians-spirit-log-filter)

ADD_SUBDIRECTORY(libzillians-framework-language)
ADD_SUBDIRECTORY(zillians-thorscript-compiler)
ADD_SUBDIRECTORY(zillians-thorscript-vm)