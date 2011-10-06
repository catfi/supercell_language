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
zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-parse)
zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-s0)
zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-xform)
zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-s1)
zillians_create_test_subject(SUBJECT thorscript-compiler-static-test-future)

zillians_add_subject_to_subject(PARENT thorscript-compiler-static-test-all CHILD thorscript-compiler-static-test-parse)
zillians_add_subject_to_subject(PARENT thorscript-compiler-static-test-all CHILD thorscript-compiler-static-test-s0)
zillians_add_subject_to_subject(PARENT thorscript-compiler-static-test-all CHILD thorscript-compiler-static-test-xform)
zillians_add_subject_to_subject(PARENT thorscript-compiler-static-test-all CHILD thorscript-compiler-static-test-s1)
zillians_add_subject_to_subject(PARENT thorscript-compiler-static-test-all CHILD thorscript-compiler-static-test-future)

get_target_property(ThorScriptCompiler ts-compile LOCATION)
SET(compile_options "--mode-xform-stage-only")
SET(compile_options_s0 "--mode-semantic-verify-0")
SET(compile_options_s1 "--mode-semantic-verify-1")

#################################################
# error
#################################################

#####################################################################
# 0_name
#####################################################################

# ambiguous
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-AMBIGUOUS_REF
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/0_name/ambiguous/AMBIGUOUS_REF.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-DUPE_NAME
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/0_name/ambiguous/DUPE_NAME.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-AMBIGUOUS_REF)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0    TARGET thorscript-compiler-static-test-DUPE_NAME)

# unknown
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INCOMPLETE_FUNC
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/0_name/unknown/INCOMPLETE_FUNC.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNKNOWN_NAME
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/0_name/unknown/UNKNOWN_NAME.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0    TARGET thorscript-compiler-static-test-INCOMPLETE_FUNC)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-UNKNOWN_NAME)

#####################################################################
# 1_type
#####################################################################

# cast / expression
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_CONV
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/cast/expression/INVALID_CONV.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-NO_MATCHING_MEMBER_FUNC
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/cast/expression/NO_MATCHING_MEMBER_FUNC.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-WRITE_RVALUE
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/cast/expression/WRITE_RVALUE.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-INVALID_CONV)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-NO_MATCHING_MEMBER_FUNC)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0    TARGET thorscript-compiler-static-test-WRITE_RVALUE)

# cast / statement
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_CONV_BOOL
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/cast/statement/INVALID_CONV_BOOL.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-INVALID_CONV_BOOL)

# function
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-CALL_NONFUNC
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/function/CALL_NONFUNC.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISMATCH_FUNC_ARGNUM_TOO_FEW
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/function/MISMATCH_FUNC_ARGNUM_TOO_FEW.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISMATCH_FUNC_ARGNUM_TOO_MANY
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/function/MISMATCH_FUNC_ARGNUM_TOO_MANY.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISMATCH_FUNC_ARGTYPE
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/function/MISMATCH_FUNC_ARGTYPE.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_RETURN_VALUE
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/function/MISSING_RETURN_VALUE.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNEXPECTED_RETURN_VALUE
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/function/UNEXPECTED_RETURN_VALUE.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-CALL_NONFUNC)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-MISMATCH_FUNC_ARGNUM_TOO_FEW)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-MISMATCH_FUNC_ARGNUM_TOO_MANY)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-MISMATCH_FUNC_ARGTYPE)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1    TARGET thorscript-compiler-static-test-MISSING_RETURN_VALUE)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1    TARGET thorscript-compiler-static-test-UNEXPECTED_RETURN_VALUE)

# primary_expression
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MEMBER_OF_NONCLASS
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/primary_expression/MEMBER_OF_NONCLASS.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-NO_SUCH_MEMBER
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/primary_expression/NO_SUCH_MEMBER.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-SUBSCRIPT_OF_NONARRAY
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/1_type/primary_expression/SUBSCRIPT_OF_NONARRAY.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-MEMBER_OF_NONCLASS)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-NO_SUCH_MEMBER)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-xform TARGET thorscript-compiler-static-test-SUBSCRIPT_OF_NONARRAY)

#####################################################################
# 2_modifier
#####################################################################

# storage / const
#zillians_add_complex_test(
#    TARGET thorscript-compiler-static-test-MISSING_CONST_INIT
#    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/storage/const/MISSING_CONST_INIT.t
#    DEPENDS ${ThorScriptCompiler}
#    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-WRITE_CONST
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/storage/const/WRITE_CONST.t
    DEPENDS ${ThorScriptCompiler}
    )

# storage / static
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_CALL
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/storage/static/INVALID_NONSTATIC_CALL.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_REF
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/storage/static/INVALID_NONSTATIC_REF.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_STATIC_INIT
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/storage/static/MISSING_STATIC_INIT.t
    DEPENDS ${ThorScriptCompiler}
    )

# visibility
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_ACCESS_PRIVATE
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/visibility/INVALID_ACCESS_PRIVATE.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INVALID_ACCESS_PROTECTED
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/2_modifier/visibility/INVALID_ACCESS_PROTECTED.t
    DEPENDS ${ThorScriptCompiler}
    )

#zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-MISSING_CONST_INIT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-WRITE_CONST)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_CALL)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-INVALID_NONSTATIC_REF)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-MISSING_STATIC_INIT)
#zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-INVALID_ACCESS_PRIVATE)
#zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-INVALID_ACCESS_PROTECTED)

#####################################################################
# 3_statement
#####################################################################

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_BREAK_TARGET
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/3_statement/MISSING_BREAK_TARGET.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_CONTINUE_TARGET
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/3_statement/MISSING_CONTINUE_TARGET.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-MISSING_BREAK_TARGET)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-MISSING_CONTINUE_TARGET)

#####################################################################
# 4_parameter
#####################################################################

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_PARAM_INIT
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/4_parameter/MISSING_PARAM_INIT.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNEXPECTED_VARIADIC_PARAM
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/4_parameter/UNEXPECTED_VARIADIC_PARAM.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNEXPECTED_VARIADIC_TEMPLATE_PARAM
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/4_parameter/UNEXPECTED_VARIADIC_TEMPLATE_PARAM.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-MISSING_PARAM_INIT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-UNEXPECTED_VARIADIC_PARAM)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-UNEXPECTED_VARIADIC_TEMPLATE_PARAM)

#####################################################################
# 5_limit
#####################################################################

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-EXCEED_PARAM_LIMIT
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/5_limit/EXCEED_PARAM_LIMIT.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-EXCEED_TEMPLATE_PARAM_LIMIT
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/error/5_limit/EXCEED_TEMPLATE_PARAM_LIMIT.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-EXCEED_PARAM_LIMIT)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0 TARGET thorscript-compiler-static-test-EXCEED_TEMPLATE_PARAM_LIMIT)

#################################################
# warn
#################################################

#####################################################################
# control_path
#####################################################################

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-CONTROL_REACHES_END
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/warn/control_path/CONTROL_REACHES_END.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-DEAD_CODE
    SHELL ${ThorScriptCompiler} ${compile_options_s0} ${CMAKE_CURRENT_SOURCE_DIR}/data/warn/control_path/DEAD_CODE.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-INFINITE_RECURSION
    SHELL ${ThorScriptCompiler} ${compile_options} ${CMAKE_CURRENT_SOURCE_DIR}/data/warn/control_path/INFINITE_RECURSION.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_CASE
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/warn/control_path/MISSING_CASE.t
    DEPENDS ${ThorScriptCompiler}
    )

#zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1     TARGET thorscript-compiler-static-test-CONTROL_REACHES_END)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s0     TARGET thorscript-compiler-static-test-DEAD_CODE)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-future TARGET thorscript-compiler-static-test-INFINITE_RECURSION)
#zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1     TARGET thorscript-compiler-static-test-MISSING_CASE)

#####################################################################
# [.]
#####################################################################

zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-MISSING_RETURN
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/warn/MISSING_RETURN.t
    DEPENDS ${ThorScriptCompiler}
    )
zillians_add_complex_test(
    TARGET thorscript-compiler-static-test-UNINIT_REF
    SHELL ${ThorScriptCompiler} ${compile_options_s1} ${CMAKE_CURRENT_SOURCE_DIR}/data/warn/UNINIT_REF.t
    DEPENDS ${ThorScriptCompiler}
    )

zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-MISSING_RETURN)
zillians_add_test_to_subject(SUBJECT thorscript-compiler-static-test-s1 TARGET thorscript-compiler-static-test-UNINIT_REF)
