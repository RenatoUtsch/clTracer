##
 # Author: Renato Utsch Gonçalves
 # Computer Science, UFMG
 # Advanced Computer Graphics
 # Practical exercise 1 - Distributed Ray Tracer
 #
 # Copyright (c) 2015 Renato Utsch
 #
 # Permission is hereby granted, free of charge, to any person obtaining a copy
 # of this software and associated documentation files (the "Software"), to deal
 # in the Software without restriction, including without limitation the rights
 # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 # copies of the Software, and to permit persons to whom the Software is
 # furnished to do so, subject to the following conditions:
 #
 # The above copyright notice and this permission notice shall be included in
 # all copies or substantial portions of the Software.
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 # THE SOFTWARE.
 ##

# CMakeLists.txt file for compiling the project with CMake.

# Basic configuration
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
project( raytracer C CXX )

set( RAYTRACER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( RAYTRACER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

set( RAYTRACER_DEFINITIONS "" )
set( RAYTRACER_INCLUDE_DIRS "" )
set( RAYTRACER_LIBRARIES "" )

# Add gcc options
if( UNIX )
    set( RAYTRACER_DEFINITIONS
        "${RAYTRACER_DEFINITIONS} -Ofast -Wall -Wextra -Werror -pedantic -Wno-unused-parameter -Wno-deprecated-declarations" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99" )
endif()

# Compile GLFW.
#set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "no examples" )
#set( GLFW_BUILD_TESTS OFF CACHE BOOL "no tests" )
#set( GLFW_BUILD_DOCS OFF CACHE BOOL "no docs" )
#set( GLFW_INSTALL OFF CACHE BOOL "no installation" )
#add_subdirectory( "${RAYTRACER_SOURCE_DIR}/3rdparty/glfw" )
#set( RAYTRACER_INCLUDE_DIRS ${RAYTRACER_INCLUDE_DIRS} "${RAYTRACER_SOURCE_DIR}/3rdparty/glfw/include" )
#set( RAYTRACER_LIBRARIES ${RAYTRACER_LIBRARIES} glfw ${GLFW_LIBRARIES} )

# Find OpenCL
find_package( OpenCL REQUIRED )
set( RAYTRACER_DEFINITIONS "${RAYTRACER_DEFINITIONS} ${OpenCL_DEFINITIONS}" )
set( RAYTRACER_INCLUDE_DIRS ${RAYTRACER_INCLUDE_DIRS} ${OpenCL_INCLUDE_DIRS} )
set( RAYTRACER_LIBRARIES ${RAYTRACER_LIBRARIES} ${OpenCL_LIBRARIES} )

# Boids sources
set( RAYTRACER_SOURCE_FILES "${RAYTRACER_SOURCE_DIR}/source/main.cpp"
                        "${RAYTRACER_SOURCE_DIR}/source/parser.cpp"
                        "${RAYTRACER_SOURCE_DIR}/source/PPMImage.cpp"
                        "${RAYTRACER_SOURCE_DIR}/source/Screen.cpp"
                        "${RAYTRACER_SOURCE_DIR}/source/World.cpp"
                        "${RAYTRACER_SOURCE_DIR}/source/clSampler/clUtils.c"
                        "${RAYTRACER_SOURCE_DIR}/source/clSampler/CodeGenerator.cpp"
                        "${RAYTRACER_SOURCE_DIR}/source/clSampler/Sampler.cpp" )

# Compile
add_definitions( ${RAYTRACER_DEFINITIONS} )
add_definitions( -DCL_SOURCE_DIR="${RAYTRACER_SOURCE_DIR}/source/clSampler/" )
include_directories( ${RAYTRACER_INCLUDE_DIRS} )

add_executable( raytracer ${RAYTRACER_SOURCE_FILES} )
target_link_libraries( raytracer ${RAYTRACER_LIBRARIES} )

