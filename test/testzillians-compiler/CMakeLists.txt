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

zillians_create_test_subject(SUBJECT language-compiler-critical)
zillians_create_test_subject(SUBJECT language-compiler-misc)

zillians_add_subject_to_subject(PARENT language-critical CHILD language-compiler-critical)
zillians_add_subject_to_subject(PARENT language-misc CHILD language-compiler-misc)

# in case we might need them later
get_target_property(ThorScriptBundler ts-bundle LOCATION)
get_target_property(ThorScriptCompiler ts-compile LOCATION)
get_target_property(ThorScriptDep ts-dep LOCATION)
get_target_property(ThorScriptDriver tsc LOCATION)
get_target_property(ThorScriptLinker ts-link LOCATION)
get_target_property(ThorScriptMake ts-make LOCATION)
get_target_property(ThorScriptManifestBuilder ts-manifest LOCATION)
get_target_property(ThorScriptStrip ts-strip LOCATION)
#get_target_property(ThorScriptStubGenerator ts-stub LOCATION)
get_target_property(ThorScriptVM ts-vm LOCATION)

ADD_SUBDIRECTORY(ThorScriptCompilerManglingTest)
ADD_SUBDIRECTORY(ThorScriptCompilerStaticTest)
ADD_SUBDIRECTORY(ThorScriptDepTest)
#ADD_SUBDIRECTORY(ThorScriptDriverTest)
ADD_SUBDIRECTORY(ThorScriptMakeTest)
ADD_SUBDIRECTORY(ThorScriptParserTest)
ADD_SUBDIRECTORY(ThorScriptStripTest)
#ADD_SUBDIRECTORY(ThorScriptStubTest)
ADD_SUBDIRECTORY(ThorScriptTransformerTest)
ADD_SUBDIRECTORY(ThorScriptTreeTest)
ADD_SUBDIRECTORY(ThorScriptResolutionTest)
ADD_SUBDIRECTORY(ThorScriptDebugInfoTest)
