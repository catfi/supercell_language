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

include_directories(
    ${PROJECT_COMMON_SOURCE_DIR}/include/
    ${PROJECT_LANGUAGE_SOURCE_DIR}/include/
    ${ZILLIANS_DEP_PATH}/linux
    )

add_definitions( ${LLVM_CPPFLAGS} )

add_library(zillians-language-main-stages-vm
    language/stage/vm/ThorScriptVMStage.cpp
	language/stage/vm/ThorScriptVMMode.cpp
    language/ThorScriptVM.cpp
    )
    
target_link_libraries(zillians-language-main-stages-vm
	${LLVM_LDFLAGS}
    ${LLVM_ALL_LIBS}
    zillians-language-general
    )
    
add_dependencies(zillians-language-main-stages zillians-language-main-stages-vm)