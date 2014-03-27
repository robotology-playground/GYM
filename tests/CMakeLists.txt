cmake_minimum_required(VERSION 2.8.7)

set(PROJECTNAME tests)

project(${PROJECTNAME})

# add local include
include_directories(${CMAKE_CURRENT_SOURCE_DIR} )

add_executable(${PROJECTNAME}_bin test_interfaces.cpp)
TARGET_LINK_LIBRARIES(${PROJECTNAME}_bin drc_shared ${YARP_LIBRARIES} ${ACE_LIBRARIES})

