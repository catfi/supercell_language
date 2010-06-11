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
IF(CUDA_FOUND AND CUDPP_FOUND)
    ADD_SUBDIRECTORY(testcuda)
    ADD_SUBDIRECTORY(testzillians-framework-vw)
ELSE()
    MESSAGE(STATUS "CUDA not available, disabling build for testcuda, testzillians-framework-vw-cuda")
ENDIF()

IF( BULLET_FOUND )
	MESSAGE( "--- Enable TestBullet")
	ADD_SUBDIRECTORY(testbullet)
ELSE()
	MESSAGE( "--- Disable TestBullet")
ENDIF()
IF( BULLET_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND )
	MESSAGE( "--- Enable TestBulletGUI")
	ADD_SUBDIRECTORY(testbullet_gui/BulletOpenGL_Support)
	ADD_SUBDIRECTORY(testbullet_gui/BulletBasicDemo)
ELSE()
	MESSAGE( "--- Disable TestBulletGUI")
ENDIF()

ADD_SUBDIRECTORY(testzillians-compiler)
ADD_SUBDIRECTORY(test_octree)
ADD_SUBDIRECTORY(ray_trace)
#ADD_SUBDIRECTORY(testzillians-framework-vw-naive)

#ADD_SUBDIRECTORY(testzillians-compiler)
