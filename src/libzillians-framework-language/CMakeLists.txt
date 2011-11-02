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
    ${ZILLIANS_DEP_PATH}/linux
    )

set(out_files "")
set(StringTablePath ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/language/logging)
COG_GEN(
    OUTPUT out_files
    COG_FILE ${StringTablePath}/StringTable.cog
    )
add_custom_target(generate-string-table
    DEPENDS ${out_files}
    )

add_library(zillians-framework-language-spirit
    language/stage/parser/ThorScriptParserStage.cpp
    )
target_link_libraries(zillians-framework-language-spirit
    zillians-common-utility
    zillians-common-core
    )
add_dependencies(zillians-framework-language-spirit generate-string-table)

add_library(zillians-framework-language SHARED
    language/context/ConfigurationContext.cpp
    language/context/ParserContext.cpp
    language/logging/LoggerWrapper.cpp
    language/stage/StageConductor.cpp
    language/stage/transformer/LiteralCompactionStage.cpp
    language/stage/transformer/ResolutionStage.cpp
    language/stage/transformer/ManglingStage.cpp
    language/stage/transformer/RestructureStage.cpp
    language/stage/verifier/SemanticVerificationStage0.cpp
    language/stage/verifier/SemanticVerificationStage1.cpp
	language/stage/verifier/StaticTestVerificationStage.cpp    
	language/stage/verifier/visitor/StaticTestVerificationStageVisitor.cpp
	language/stage/serialization/ASTSerializationStage.cpp
	language/stage/serialization/ASTDeserializationStage.cpp    
	language/stage/serialization/detail/ASTSerializationHelper.cpp
	language/tree/basic/FunctionType.cpp  
	language/tree/ASTNodeSerialization.cpp
	language/stage/dep/ThorScriptDepStage.cpp
    language/stage/dep/ThorScriptPackageDependencyGrammar.cpp
    language/stage/make/ThorScriptMakeStage.cpp
    language/stage/bundle/ThorScriptBundleStage.cpp
    language/stage/strip/ThorScriptStripStage.cpp
    language/stage/stub/ThorScriptStubStage.cpp
    language/stage/linker/ThorScriptLinkerStage.cpp
    language/stage/driver/ThorScriptDriverStage.cpp
	)

if(LLVM_FOUND)
    add_definitions( ${LLVM_CPPFLAGS} )
    add_library(zillians-framework-language-llvm
        language/context/GeneratorContext.cpp
        language/stage/generator/LLVMGeneratorStage.cpp
        language/stage/generator/LLVMDebugInfoGeneratorStage.cpp
        language/stage/generator/LLVMBitCodeGeneratorStage.cpp
        language/stage/generator/LLVMGlobalDispatchGeneratorStage.cpp
        )
    target_link_libraries(zillians-framework-language-llvm
		${LLVM_LDFLAGS}
        ${LLVM_ALL_LIBS}
        zillians-common-utility
        zillians-common-core
        )

    target_link_libraries(zillians-framework-language
        zillians-framework-language-spirit
        zillians-common-utility
        zillians-common-core
        zillians-common-utility-archive
        zillians-thorpp-stubs
        zillians-framework-language-llvm
        )
else()
    target_link_libraries(zillians-framework-language
        zillians-framework-language-spirit
        zillians-common-utility
        zillians-common-core
        zillians-common-utility-archive
        zillians-thorpp-stubs
        )
endif()

add_dependencies(zillians-framework-language generate-string-table)

#get_target_property(__lib_location zillians-framework-language-spirit LOCATION)
#add_custom_command(TARGET zillians-framework-language-spirit POST_BUILD
#    COMMAND /usr/bin/strip --strip-debug ${__lib_location}
#    COMMENT "Stripping boost spirit debug symbols"
#    )

#ADD_PRECOMPILED_HEADER(zillians-framework-language-spirit
#    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/language/_PreCompiledHeaders.h
#    )
