##
 # Copyright (c) 2015 Renato Utsch <renatoutsch@gmail.com>
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

# Basic configuration
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
project( clTracer C CXX )

set( CLTRACER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( CLTRACER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

set( CLTRACER_DEFINITIONS "" )
set( CLTRACER_INCLUDE_DIRS "" )
set( CLTRACER_LIBRARIES "" )

# Add gcc options
if( UNIX )
    set( CLTRACER_DEFINITIONS
        "${CLTRACER_DEFINITIONS} -Ofast -Wall -Wextra -Werror -pedantic -Wno-unused-parameter -Wno-deprecated-declarations" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99" )
endif()

# Find OpenCL
find_package( OpenCL REQUIRED )
set( CLTRACER_DEFINITIONS "${CLTRACER_DEFINITIONS} ${OpenCL_DEFINITIONS}" )
set( CLTRACER_INCLUDE_DIRS ${CLTRACER_INCLUDE_DIRS} ${OpenCL_INCLUDE_DIRS} )
set( CLTRACER_LIBRARIES ${CLTRACER_LIBRARIES} ${OpenCL_LIBRARIES} )

# clTracer sources
set( CLTRACER_SOURCE_FILES "${CLTRACER_SOURCE_DIR}/source/main.cpp"
    "${CLTRACER_SOURCE_DIR}/source/CmdArgs.cpp"
    "${CLTRACER_SOURCE_DIR}/source/PPMImage.cpp"
    "${CLTRACER_SOURCE_DIR}/source/Screen.cpp"
    "${CLTRACER_SOURCE_DIR}/source/World.cpp"
    "${CLTRACER_SOURCE_DIR}/source/clSampler/clUtils.c"
    "${CLTRACER_SOURCE_DIR}/source/clSampler/CodeGenerator.cpp"
    "${CLTRACER_SOURCE_DIR}/source/clSampler/Sampler.cpp" )

# Compile
add_definitions( ${CLTRACER_DEFINITIONS} )
add_definitions( -DCL_SOURCE_DIR="${CLTRACER_SOURCE_DIR}/source/clSampler/" )
include_directories( ${CLTRACER_INCLUDE_DIRS} )

add_executable( clTracer ${CLTRACER_SOURCE_FILES} )
target_link_libraries( clTracer ${CLTRACER_LIBRARIES} )

