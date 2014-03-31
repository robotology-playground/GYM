cmake_minimum_required(VERSION 2.8.3)
project(drc_shared)

set(CMAKE_CXX_FLAGS "-std=c++0x -O4 -g") # -g for debugging 

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
)
set(VARS_PREFIX "DRC_SHARED")
                            
find_package(YCM REQUIRED)
include(YCMDefaultDirs)
ycm_default_dirs(${VARS_PREFIX})
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

## Declare drc_shared library
add_library(drc_shared SHARED
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

# examples
add_executable(grabber_example src/examples/grabber_example.cpp)
add_executable(module_state_streamer_example src/examples/module_state_streamer_example.cpp)
target_link_libraries(module_state_streamer_example
                      ${YARP_LIBRARIES}
                      drc_shared
                      )
                      
target_link_libraries(grabber_example
		      ${YARP_LIBRARIES}
		      )

install(TARGETS drc_shared planner_msgs 
  grabber_example module_state_streamer_example # examples, comment this if not needed
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
# set_target_properties(drc_shared PROPERTIES VERSION ${${VARS_PREFIX}_VERSION}
#                                               SOVERSION ${${VARS_PREFIX}_VERSION}
# #                                              PUBLIC_HEADER "${template_lib_HDRS}")
# )

target_include_directories(drc_shared PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>"
                                               "$<INSTALL_INTERFACE:${${VARS_PREFIX}_INSTALL_INCLUDEDIR}>")
                   
install(TARGETS drc_shared  
        EXPORT drc_Shared  
        RUNTIME DESTINATION "${${VARS_PREFIX}_INSTALL_BINDIR}" COMPONENT bin
        LIBRARY DESTINATION "${${VARS_PREFIX}_INSTALL_LIBDIR}"   COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${${VARS_PREFIX}_INSTALL_INCLUDEDIR}/drc_shared" COMPONENT dev)

set_property(GLOBAL APPEND PROPERTY ${VARS_PREFIX}_TARGETS drc_shared)

 install(TARGETS drc_shared planner_msgs
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 )

include(InstallBasicPackageFiles)

install_basic_package_files(drc_Shared VARS_PREFIX ${VARS_PREFIX}
                                        VERSION "1.0.0"
                                        TARGETS_PROPERTY ${VARS_PREFIX}_TARGETS
                                        COMPATIBILITY SameMajorVersion

)
include (AddUninstallTarget)
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch)

add_subdirectory(src/tests)
