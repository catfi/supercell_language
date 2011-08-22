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

include_directories(
    ${PROJECT_COMMON_SOURCE_DIR}/include/
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/
    )

add_library(zillians-framework-language-stage
    language/stage/StageConductor.cpp
    language/stage/basic/TreeDebugStage.cpp
    language/stage/parser/ThorScriptParserStage.cpp
    language/stage/transformer/TypeInferenceStage.cpp
	)

if(LLVM_FOUND)
    add_definitions( ${LLVM_CPPFLAGS} )
    
    add_library(zillians-framework-language-stage-llvm
        language/stage/generator/LLVMGeneratorStage.cpp
        )
endif()

if(LLVM_FOUND)
    target_link_libraries(zillians-framework-language-stage
        zillians-common-core
        zillians-common-utility
        zillians-framework-language
        zillians-framework-language-stage-llvm
        )
    
    target_link_libraries(zillians-framework-language-stage-llvm
        zillians-common-core
        zillians-common-utility
        zillians-framework-language
        ${LLVM_ALL_LIBS}
        )
else()
    target_link_libraries(zillians-framework-language-stage
        zillians-common-core
        zillians-common-utility
        zillians-framework-language
        )
endif()
