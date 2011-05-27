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

PROJECT(zillians-framework)

configure_file(include/vw/services/api/adt/ADTApi.zs ${ZILLIANS_INC}/ADTApi.zs)
configure_file(include/vw/services/api/debug/DebugApi.zs ${ZILLIANS_INC}/DebugApi.zs)
configure_file(include/vw/services/api/event/EventApi.zs ${ZILLIANS_INC}/EventApi.zs)
configure_file(include/vw/services/api/math/MathApi.zs ${ZILLIANS_INC}/MathApi.zs)
#configure_file(include/vw/services/api/runtime/RuntimeApi.zs ${ZILLIANS_INC}/RuntimeApi.zs)
configure_file(include/vw/services/api/space/SpaceApi.zs ${ZILLIANS_INC}/SpaceApi.zs)
configure_file(include/vw/services/api/test/TestApi.zs ${ZILLIANS_INC}/TestApi.zs)
configure_file(  include/vw/services/api/gameobject/GameObjectApi.zs ${ZILLIANS_INC}/GameObjectApi.zs )

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)
