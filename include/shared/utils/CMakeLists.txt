# YARP needs CMake 2.6 or greater
cmake_minimum_required(VERSION 2.8.7)

set_property(GLOBAL APPEND PROPERTY YARP_UTILS ${CMAKE_CURRENT_SOURCE_DIR}/OccupancyGrid.cpp)