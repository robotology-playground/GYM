# Copyright: (C) 2014 Walkman Consortium
# Authors: Luca Muratore
# CopyPolicy: Released under the terms of the GNU LGPL v3.0.

cmake_minimum_required(VERSION 2.8.12)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 OLD)
    cmake_policy(SET CMP0005 NEW)
    cmake_policy(SET CMP0017 NEW)
endif(COMMAND cmake_policy)
include(ExternalProject)

set(PROJECTNAME tests)
project(${PROJECTNAME})

# compite GTest
set(gTestSource "/usr/src/gtest")
ExternalProject_Add(GTest-ext SOURCE_DIR ${gTestSource}
                              PREFIX "${CMAKE_CURRENT_BINARY_DIR}/external"
                              INSTALL_COMMAND "")
set(GTEST_LIB_DIRS "${CMAKE_CURRENT_BINARY_DIR}/external/src/GTest-ext-build/")
set(GTEST_INCLUDE_DIRS ${gTestSource})
set(GTEST_BOTH_LIBRARIES gtest gtest_main)
include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_LIB_DIRS})

# add local include
include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
enable_testing()

if( YCM_EP_DEVEL_MODE_GYM)
    add_custom_target(  GYM_test ALL
                        COMMAND ${CMAKE_CTEST_COMMAND}
                        DEPENDS GYM
                                interfacesTest )
endif()

SET(interfacesTestLibs GYM ${YARP_LIBRARIES} ${ACE_LIBRARIES})
ADD_EXECUTABLE(interfacesTest test_interfaces.cpp)
TARGET_LINK_LIBRARIES(interfacesTest ${interfacesTestLibs})
add_dependencies(interfacesTest GTest-ext GYM)
#add_test(NAME test_interfaces COMMAND interfacesTest)
