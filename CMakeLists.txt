cmake_minimum_required(VERSION 2.8.3)
project(drc_shared)

set(CMAKE_CXX_FLAGS "-std=c++0x -O4 -g") # -g for debugging 

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES drc_shared planner_msgs 
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

# find PCL
find_package(PCL 1.7 REQUIRED COMPONENTS common)
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
include_directories(${PCL_INCLUDE_DIRS})

# find YARP
find_package(YARP 2.3.60)
include_directories(${YARP_INCLUDE_DIRS})

add_subdirectory(src/MotorControlHelpers)

## Declare a cpp library
add_library(drc_shared
  src/OccupancyGrid.cpp
  src/utils/module_state_streamer.cpp
  src/yarp_status_interface.cpp
  src/yarp_ft_interface.cpp
  src/yarp_single_chain_interface.cpp
  #${PCL_LIBRARIES}
)
 
add_library(planner_msgs
  src/planner_msgs/flat_walk_cmd.cpp
  src/planner_msgs/flat_walk_plan.cpp
)

install(TARGETS drc_shared planner_msgs
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch)

add_subdirectory(src/tests)