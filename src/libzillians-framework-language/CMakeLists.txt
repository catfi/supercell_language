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

INCLUDE(UsingCog)

include_directories( ${PROJECT_COMMON_SOURCE_DIR}/include/
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/
    )

add_library(zillians-framework-language
    language/context/ConfigurationContext.cpp
    language/context/ParserContext.cpp
    language/logging/LoggerWrapper.cpp
    language/stage/generator/LLVMGlobalDispatchGeneratorStage.cpp
    language/stage/StageConductor.cpp
    language/stage/basic/TreeDebugStage.cpp
    language/stage/parser/ThorScriptParserStage.cpp
    language/stage/transformer/LiteralCompactionStage.cpp
    language/stage/transformer/ResolutionStage.cpp
    language/stage/transformer/ManglingStage.cpp
    #language/stage/transformer/TypeSynthesisStage.cpp
    #language/stage/transformer/TypeInferenceStage.cpp
    language/stage/verifier/SemanticVerificationStage0.cpp
    language/stage/verifier/SemanticVerificationStage1.cpp
	language/stage/verifier/StaticTestVerificationStage.cpp
	)

set(StringTablePath ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/language/logging)

# Generate StringTable.h
#add_custom_command(OUTPUT ${StringTablePath}/StringTable.h
#	COMMAND cog.py -d -o ${StringTablePath}/StringTable.h ${StringTablePath}/StringTable.cog
#	DEPENDS ${StringTablePath}/StringTable.cog
#	COMMENT "Generate StringTable.h by using cog.py"
#	)

SET(out_files "")
COG_GEN(
    OUTPUT out_files
    COG_FILE ${StringTablePath}/StringTable.cog
    COMMENT "cog.py -- Generating StringTable.h from StringTable.cog .."
    )

add_custom_target(generate-string-table ALL echo DEPENDS ${out_files} COMMENT "generate-string-table depends on StringTable.h")
add_dependencies(zillians-framework-language generate-string-table)

if(LLVM_FOUND)
    add_definitions( ${LLVM_CPPFLAGS} )
    
    add_library(zillians-framework-language-llvm
        language/context/GeneratorContext.cpp
        language/stage/generator/LLVMGeneratorStage.cpp
        language/stage/generator/LLVMDebugInfoGeneratorStage.cpp
        language/stage/generator/LLVMBitCodeGeneratorStage.cpp
        )
endif()

ADD_PRECOMPILED_HEADER(zillians-framework-language
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/language/_PCH.h
    )

if(LLVM_FOUND)
    target_link_libraries(zillians-framework-language
        zillians-common-core
        zillians-common-utility
        zillians-framework-language-llvm
        )
    
    target_link_libraries(zillians-framework-language-llvm
        zillians-common-core
        zillians-common-utility
		${LLVM_LDFLAGS}
        ${LLVM_ALL_LIBS}
        )
else()
    target_link_libraries(zillians-framework-language
        zillians-common-core
        zillians-common-utility
        )
endif()


