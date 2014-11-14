# Copyright: (C) Luca Muratore
# Authors: Luca Muratore (luca.muratore@iit.it)
# CopyPolicy: Released under the terms of the GNU LGPL v3.0.

# find YARP
find_package(YARP REQUIRED)
# pick up yarp's cmake scripts
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpInstallationHelpers)    
# get all the .ini files in conf
file(GLOB conf conf/*.ini)
# install all the .ini files in conf in the "default context" directory
yarp_install(FILES ${conf} DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/generic_tutorial)

add_custom_target(  copy_conf_files_to_build ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/app/conf"
        "${CMAKE_BINARY_DIR}/bin")
