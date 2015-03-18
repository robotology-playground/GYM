# 
#  Copyright (C) 2014 Walkman
#  Author: Mirko Ferrati, Enrico Mingo, Alessio Rocchi, Luca Muratore
#  email: mirko.ferrati@gmail.com, enrico.mingo@iit.it, alessio.rocchi@iit.it, luca.muratore@iit.it
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program. If not, see <http://www.gnu.org/licenses/>
#

cmake_minimum_required(VERSION 2.8.12)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 OLD)
    cmake_policy(SET CMP0005 NEW)
    cmake_policy(SET CMP0017 NEW)
endif(COMMAND cmake_policy)
include(ExternalProject)
PROJECT(GYM)

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

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(paramHelp REQUIRED)
FIND_PACKAGE(Boost 1.54.0 COMPONENTS system thread)
# to avoid header not found when using idynutils 
FIND_PACKAGE(Eigen3 REQUIRED)

INCLUDE_DIRECTORIES(include ${YARP_INCLUDE_DIRS} ${paramHelp_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})

# for every file in INCLUDES CMake already sets the property HEADER_FILE_ONLY
file(GLOB_RECURSE GYM_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/GYM" *.h*)

ADD_LIBRARY(GYM SHARED  src/yarp_status_interface.cpp
                        ${GYM_INCLUDES})

TARGET_LINK_LIBRARIES(GYM   ${YARP_LIBRARIES} 
			    ${paramHelp_LIBRARIES}
			    ${Boost_LIBRARIES}
		     )

########################################################################
# use YCM to export GYM so that it can be found using find_package #
########################################################################

set(VARS_PREFIX "GYM")

set(GYM_MAJOR_VERSION 0)
set(GYM_MINOR_VERSION 0)
set(GYM_PATCH_VERSION 1)
set(GYM_VERSION ${GYM_MAJOR_VERSION}.${GYM_MINOR_VERSION}.${GYM_PATCH_VERSION})

find_package(YCM REQUIRED)
include(YCMDefaultDirs)
ycm_default_dirs(${VARS_PREFIX})

target_include_directories(GYM PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>"
                                      "$<INSTALL_INTERFACE:${${VARS_PREFIX}_INSTALL_INCLUDEDIR}>")

set_target_properties(GYM   PROPERTIES VERSION ${${VARS_PREFIX}_VERSION}
                            SOVERSION ${${VARS_PREFIX}_VERSION})

install(DIRECTORY include/ 
        DESTINATION "${${VARS_PREFIX}_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h*")

install(TARGETS GYM  
        EXPORT GYM
        ARCHIVE DESTINATION "${${VARS_PREFIX}_INSTALL_BINDIR}" COMPONENT lib
        RUNTIME DESTINATION "${${VARS_PREFIX}_INSTALL_BINDIR}" COMPONENT bin
        LIBRARY DESTINATION "${${VARS_PREFIX}_INSTALL_LIBDIR}" COMPONENT shlib)
        
#enabling it will add all GYM dependencies as dependencies for third party users
set_property(GLOBAL APPEND PROPERTY ${VARS_PREFIX}_TARGETS GYM)


include(InstallBasicPackageFiles)
install_basic_package_files(GYM     VARS_PREFIX ${VARS_PREFIX}
                                    VERSION ${${VARS_PREFIX}_VERSION}
                                    COMPATIBILITY SameMajorVersion
                                    TARGETS_PROPERTY ${VARS_PREFIX}_TARGETS
                                    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

include(AddUninstallTarget)

# remove old installed script
file(REMOVE ${CMAKE_INSTALL_PREFIX}/bin/generate_GYM_skeleton.sh)

# install new scripts
INSTALL(PROGRAMS scripts/generate_GYM_generic_skeleton.sh DESTINATION bin)
INSTALL(PROGRAMS scripts/generate_GYM_control_skeleton.sh DESTINATION bin)

add_subdirectory(doc)
#add_subdirectory(examples)

#######################
# Add Testing target  #
#######################
enable_testing()
#add_subdirectory(tests)
