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

#ADD_SUBDIRECTORY(SimpleGrammarTest) # NOTE 20100811 Nothing - Not used anymore as testing now all done by zcc, not directly linking with grammar and frontend.
IF(CUDA_FOUND AND CUDPP_FOUND)
    ADD_SUBDIRECTORY(PTXCodeGenTest)
    ADD_SUBDIRECTORY(PTXParserTest)
    ADD_SUBDIRECTORY(PTXParserTest-RealExecute)
    ADD_SUBDIRECTORY(PTXFunctionMapperAdvanceTest)
ELSE()
    MESSAGE(STATUS "CUDA or CUDPP not available, disabling build for PTXCodeGenTest, PTXParserTest, PTXFunctionMapperAdvanceTest")
ENDIF()
ADD_SUBDIRECTORY(LLVM_IR_Test)
ADD_SUBDIRECTORY(RuntimeApiTest)

#ADD_SUBDIRECTORY(CodeGenCommonTest)
#ADD_SUBDIRECTORY(LLVMFunctionMapperBasicTest)
#ADD_SUBDIRECTORY(PTXFunctionMapperBasicTest)

ADD_SUBDIRECTORY(ZccMidendTest)
#ADD_SUBDIRECTORY(DebugApiTest)

ADD_SUBDIRECTORY(SemanticCheckTest)
ADD_SUBDIRECTORY(StubCodeGenTest)
ADD_SUBDIRECTORY(PTXBackendTest)

ADD_CUSTOM_TARGET( zcc-test-all ALL echo )

ADD_DEPENDENCIES( zcc-test-all 
		zcc-test-llvm-all 
		zcc-test-cond-all 
		zcc-test-expr-all 
		zcc-test-stmt-all 
		zcc-test-object-all 
		zcc-test-logical-stmt
		)	
