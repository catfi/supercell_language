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

# Based on:
# http://google.com/codesearch#FIWsDLeuBYE/ganesha/CMakeLists.txt&q=PATCH_COMMAND

# References:
# http://www.cmake.org/cmake/help/cmake-2-8-docs.html#module:ExternalProject
# http://www.kitware.com/products/html/BuildingExternalProjectsWithCMake2.8.html

# Download path:
# ${CMAKE_CURRENT_BINARY_DIR}/ts-doc-prefix/src/doxygen-1.7.5.1.src.tar.gz

# Untar path:
# ${CMAKE_CURRENT_BINARY_DIR}/untar_path

# Binary path:
# ${CMAKE_CURRENT_BINARY_DIR}/bin_path/bin/doxygen

# Install path:
# ${EXECUTABLE_OUTPUT_PATH}

include(ExternalProject)

ExternalProject_Add(EP_ts-doc
    #--Download step--------------
    URL
        "http://ftp.stack.nl/pub/users/dimitri/doxygen-1.7.5.1.src.tar.gz"
    #--Update/Patch step----------
    PATCH_COMMAND
        patch -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/doxy_ts.patch
    #--Configure step-------------
    SOURCE_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/untar_path
    CONFIGURE_COMMAND
        ${CMAKE_CURRENT_BINARY_DIR}/untar_path/configure --prefix ${CMAKE_CURRENT_BINARY_DIR}/bin_path
    #--Build step-----------------
    BUILD_COMMAND
        make
    BUILD_IN_SOURCE
        1
    #--Install step---------------
    INSTALL_COMMAND
        make install &&
            ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/bin_path/bin/doxygen ${EXECUTABLE_OUTPUT_PATH}/ts-doc
    #--Test step------------------
    #--Output logging-------------
    LOG_DOWNLOAD
        1
    LOG_CONFIGURE
        1
    LOG_BUILD
        1
    #--Custom targets-------------
    )

#get_target_property(ThorScriptDocGenerator ts-doc LOCATION)
#message(STATUS "Using ThorScript Doc Generator: \"${ThorScriptDocGenerator}\"")
