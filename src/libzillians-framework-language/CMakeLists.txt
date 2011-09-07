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

include_directories( ${PROJECT_COMMON_SOURCE_DIR}/include/
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/
    )

set(StringTablePath ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/language/logging/logging-manager)
# Generate StringTable.h
add_custom_command(OUTPUT ${StringTablePath}/StringTable.h
	COMMAND cog.py -d -o ${StringTablePath}/StringTable.h ${StringTablePath}/StringTable.cog
	MAIN_DEPENDENCY ${StringTablePath}/StringTable.cog
	COMMENT "Generate StringTable.h by using cog.py"
	)

add_library(zillians-framework-language
    language/context/ConfigurationContext.cpp
    language/context/ParserContext.cpp
    language/logging/Logger.cpp
    language/logging/logging-manager/LoggingManager.cpp
	${StringTablePath}/StringTable.h	# Put it here, so it will trigger the add_custom_command below
    language/stage/StageConductor.cpp
    language/stage/basic/TreeDebugStage.cpp
    language/stage/parser/ThorScriptParserStage.cpp
    language/stage/transformer/ResolutionStage.cpp
    language/stage/transformer/ManglingStage.cpp
	)

if(LLVM_FOUND)
    add_definitions( ${LLVM_CPPFLAGS} )
    
    add_library(zillians-framework-language-llvm
        language/context/GeneratorContext.cpp
        language/stage/generator/LLVMGeneratorStage.cpp
        language/stage/generator/LLVMDebugInfoGeneratorStage.cpp
        language/stage/generator/LLVMBitCodeGeneratorStage.cpp
        )
endif()

if(LLVM_FOUND)
    target_link_libraries(zillians-framework-language
        zillians-common-core
        zillians-common-utility
        zillians-framework-language-llvm
        )
    
    target_link_libraries(zillians-framework-language-llvm
        zillians-common-core
        zillians-common-utility
        ${LLVM_ALL_LIBS}
        )
else()
    target_link_libraries(zillians-framework-language
        zillians-common-core
        zillians-common-utility
        )
endif()


