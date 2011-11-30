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
    ${PROJECT_FRAMEWORK_SOURCE_DIR}/include/
    )

add_executable(tsc
    main.cpp
    )
    
target_link_libraries(tsc
    zillians-framework-language-main-stages-driver
    )
    
get_target_property(ThorScriptDriver tsc LOCATION)
message(STATUS "Using ThorScript Driver: \"${ThorScriptDriver}\"")

IF(
    TARGET ts-bundle AND
    TARGET ts-compile AND
    TARGET ts-dep AND
    TARGET ts-link AND
    TARGET ts-make AND
    TARGET ts-manifest AND
    TARGET ts-strip AND
    TARGET ts-stub AND
    TARGET ts-vm
    )
    ADD_DEPENDENCIES(tsc
        ts-bundle
        ts-compile
        ts-dep
        ts-link
        ts-make
#        ts-manifest
        ts-strip
        ts-stub
#        ts-vm
        )
ENDIF()
