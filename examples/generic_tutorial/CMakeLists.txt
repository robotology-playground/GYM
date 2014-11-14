# Copyright: (C) Luca Muratore
# Authors: Luca Muratore (luca.muratore@iit.it)
# CopyPolicy: Released under the terms of the GNU LGPL v3.0.

#minimum cmake required version
cmake_minimum_required(VERSION 2.8)
# generic_tutorial project
project(generic_tutorial)

# add app cmake subdirectory
add_subdirectory(app)

###################
## Configuration ##
###################

#enable C++ 11 : try with two different flags
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# find the required packages
find_package(idynutils REQUIRED)
find_package(paramHelp REQUIRED)
find_package(YARP REQUIRED)

###########
## Build ##
###########

file(GLOB_RECURSE generic_tutorial_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/" *.h*)

# include directories list
include_directories( include ${CMAKE_SOURCE_DIR}/include ${idynutils_INCLUDE_DIRS} ${paramHelp_INCLUDE_DIRS} ${YARP_INCLUDE_DIRS} )

# create the generic tutorial executable
add_executable( ${PROJECT_NAME} src/tutorial_main.cpp 
                                src/tutorial_control_thread.cpp
                                ${generic_tutorial_INCLUDES} )

# wait for GYM to compile before building the tutorial
add_dependencies( ${PROJECT_NAME} GYM )


##########
## Link ##
##########

# linking the tutorial executable against YARP, idynutils and the tutorial lib
target_link_libraries( ${PROJECT_NAME} ${idynutils_LIBRARIES} GYM ${YARP_LIBRARIES} )

#############
## Install ##
#############

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
