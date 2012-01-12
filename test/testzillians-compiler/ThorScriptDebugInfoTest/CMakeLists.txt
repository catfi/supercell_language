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

FIND_PROGRAM(GDB gdb)

IF (GDB)
	zillians_create_test_subject(SUBJECT thorscript-compiler-debug-test)
	zillians_add_subject_to_subject(PARENT language-compiler-critical CHILD thorscript-compiler-debug-test)

	# Define a useful macro for each debug project to use
	MACRO(thorscript_add_debuginfo_test_target)
	    # parse the argument options
	    SET(__option_catalogries "TARGET;BUNDLE;ENTRY;SCRIPT;BASELIB;DEPENDS")
	    SET(__temporary_options_variable ${ARGN})
	    split_options(__temporary_options_variable "default" __option_catalogries __options_set)
	
	    # check if the number of subject names specified is wrong
	    hashmap(GET __options_set "TARGET" __debug_target_name)
	    hashmap(GET __options_set "BUNDLE" __debug_bundle_name)
	    hashmap(GET __options_set "ENTRY" __debug_entry_name)
	    hashmap(GET __options_set "BASELIB" __debug_baselib_path)
	    hashmap(GET __options_set "SCRIPT" __debug_script_name)
	    hashmap(GET __options_set "DEPENDS" __debug_depends)

		# NOTE: Force append "runtest-" to avoid add to simple test
		SET(__debug_target_name "runtest-${__debug_target_name}")
		MESSAGE("DebugInfoTest: TARGET(${__debug_target_name}) BUNDLE(${__debug_bundle_name}) ENTRY(${__debug_entry_name}) SCRIPT(${__debug_script_name}) DEPENDS(${__debug_depends})")
	
		#
		# The ADD_CUSTOM_COMMAND associate with the ADD_CUSTOM_TARGET to make sure it will be done each time we make the target.
		# Even if there is no file modified.
		#
		ADD_CUSTOM_COMMAND(OUTPUT dummy_output 
		    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}	# Copy whole source dir XD
		    COMMAND ${CMAKE_COMMAND} -E copy_directory ${__debug_baselib_path} ${CMAKE_CURRENT_BINARY_DIR}/script/lib
			COMMENT "${__debug_target_name} copy scrips"
			)

		ADD_CUSTOM_TARGET(${__debug_target_name}
			DEPENDS dummy_output ${__debug_depends}
			COMMAND ${ThorScriptDriver} build debug
			COMMAND ${ThorScriptDriver} generate bundle
			COMMAND ${GDB} -batch -nx -x ${CMAKE_CURRENT_BINARY_DIR}/script/${__debug_script_name} -d ${CMAKE_CURRENT_BINARY_DIR}/script --args ${ThorScriptVM} -c build/bin/${__debug_bundle_name} ${__debug_entry_name}
			COMMENT "${__debug_target_name} running"
			)

	ENDMACRO()

	
	ADD_SUBDIRECTORY(arithmetic)
	ADD_SUBDIRECTORY(external_library)
	ADD_SUBDIRECTORY(package)
	ADD_SUBDIRECTORY(call_expression)
	ADD_SUBDIRECTORY(condition_jump)
	ADD_SUBDIRECTORY(variable_declare)

ENDIF()
