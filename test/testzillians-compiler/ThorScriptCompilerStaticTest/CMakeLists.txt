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

zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-all)

get_target_property(ThorScriptCompiler ts-compile LOCATION)
SET(compile_options "--enable-static-test --verify-stage-0-and-stop")

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-AMBIGUOUS_REF
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/AMBIGUOUS_REF.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-CALL_NONFUNC
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/CALL_NONFUNC.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-DUPE_NAME
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/DUPE_NAME.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-EXCEED_PARAM_LIMIT
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/EXCEED_PARAM_LIMIT.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-HEAP_VAR_INIT_NONCONST
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/HEAP_VAR_INIT_NONCONST.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_CONV_BOOL
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/INVALID_CONV_BOOL.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_CONV
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/INVALID_CONV.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_CALL
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/INVALID_NONSTATIC_CALL.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_REF
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/INVALID_NONSTATIC_REF.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_RETURN_VALUE
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/INVALID_RETURN_VALUE.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_SUBSCRIPT
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/INVALID_SUBSCRIPT.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_BREAK_TARGET
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/MISSING_BREAK_TARGET.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_CONST_INIT
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/MISSING_CONST_INIT.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_CONTINUE_TARGET
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/MISSING_CONTINUE_TARGET.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_PARAM_INIT
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/MISSING_PARAM_INIT.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_RETURN_VALUE
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/MISSING_RETURN_VALUE.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-NO_MATCHING_FUNC_ARGNUM
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/NO_MATCHING_FUNC_ARGNUM.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-NO_MATCHING_FUNC
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/NO_MATCHING_FUNC.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-NO_MATCHING_MEMBER
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/NO_MATCHING_MEMBER.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-NOT_A_CLASS
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/NOT_A_CLASS.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNDEFINED_REF
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/UNDEFINED_REF.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNEXPECTED_VARIADIC_PARAM
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/UNEXPECTED_VARIADIC_PARAM.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNKNOWN_NAME
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/UNKNOWN_NAME.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-VIS_VIOLATION
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/VIS_VIOLATION.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-WRITE_CONST
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/WRITE_CONST.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-WRITE_RVALUE
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/WRITE_RVALUE.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-AMBIGUOUS_REF)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-CALL_NONFUNC)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-DUPE_NAME)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-EXCEED_PARAM_LIMIT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-HEAP_VAR_INIT_NONCONST)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-INVALID_CONV_BOOL)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-INVALID_CONV)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_CALL)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_REF)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-INVALID_RETURN_VALUE)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-INVALID_SUBSCRIPT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-MISSING_BREAK_TARGET)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-MISSING_CONST_INIT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-MISSING_CONTINUE_TARGET)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-MISSING_PARAM_INIT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-MISSING_RETURN_VALUE)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-NO_MATCHING_FUNC_ARGNUM)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-NO_MATCHING_FUNCS)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-NO_MATCHING_MEMBER)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-NOT_A_CLASS)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-UNDEFINED_REF)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-UNEXPECT_VARIADIC_PARAM)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-UNKNOWN_NAME)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-VIS_VIOLATION)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-WRITE_CONST)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-all TARGET thorscript-compiler-static-test-WRITE_RVALUE)
