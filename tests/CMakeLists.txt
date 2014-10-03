cmake_minimum_required(VERSION 2.8.7)
include(ExternalProject)

set(PROJECTNAME tests)
project(${PROJECTNAME})
set(TESTSDIR ${CMAKE_BINARY_DIR}/devel/lib/drc_shared)

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

if( YCM_EP_DEVEL_MODE_drc_shared)
    add_custom_target(  drc_shared_test ALL
                        COMMAND ${CMAKE_CTEST_COMMAND}
                        DEPENDS drc_utils idynutils drc_shared tests_utils
                                CartesianUtilsTest
                                iDynUtilsTest
                                testUtilsTest
                                interfacesTest )
endif()

SET(interfacesTestLibs drc_shared ${YARP_LIBRARIES} ${ACE_LIBRARIES})
ADD_EXECUTABLE(interfacesTest test_interfaces.cpp)
TARGET_LINK_LIBRARIES(interfacesTest ${interfacesTestLibs})
add_dependencies(interfacesTest GTest-ext drc_shared)
#add_test(test_drc_shared    ${TESTSDIR}/interfacesTest)

SET(CartesianUtilsTestLibs idynutils ${moveit_core_LIBRARIES} ${srdfdom_LIBRARIES}
                                ${kdl_codyco_LIBRARIES} ${iDynTree_LIBRARIES}
                                ${orocos_kdl_LIBRARIES} ${urdf_LIBRARIES} ${kdl_parser_LIBRARIES}
                                ${YARP_LIBRARIES} icubmod ${GTEST_BOTH_LIBRARIES})
ADD_EXECUTABLE(CartesianUtilsTest     cartesian_utils_tests.cpp)
TARGET_LINK_LIBRARIES(CartesianUtilsTest ${CartesianUtilsTestLibs})
add_dependencies(CartesianUtilsTest GTest-ext idynutils)
add_test(cartesian_utils_tests ${TESTSDIR}/CartesianUtilsTest)

SET(iDynUtilsTestLibs tests_utils idynutils ${moveit_core_LIBRARIES} ${srdfdom_LIBRARIES}
                                ${kdl_codyco_LIBRARIES} ${iDynTree_LIBRARIES}
                                ${orocos_kdl_LIBRARIES} ${urdf_LIBRARIES} ${kdl_parser_LIBRARIES}
                                ${YARP_LIBRARIES} icubmod ${GTEST_BOTH_LIBRARIES})
ADD_EXECUTABLE(iDynUtilsTest    idyn_utils_tests.cpp)
TARGET_LINK_LIBRARIES(iDynUtilsTest ${iDynUtilsTestLibs})
add_dependencies(iDynUtilsTest GTest-ext idynutils)
add_test(idyn_utils_tests ${TESTSDIR}/iDynUtilsTest)

SET(testUtilsTestLibs tests_utils idynutils ${moveit_core_LIBRARIES} ${srdfdom_LIBRARIES}
                                ${kdl_codyco_LIBRARIES} ${iDynTree_LIBRARIES}
                                ${orocos_kdl_LIBRARIES} ${urdf_LIBRARIES} ${kdl_parser_LIBRARIES}
                                ${YARP_LIBRARIES} icubmod ${GTEST_BOTH_LIBRARIES})
ADD_EXECUTABLE(testUtilsTest     tests_utils_tests.cpp)
TARGET_LINK_LIBRARIES(testUtilsTest ${testUtilsTestLibs})
add_dependencies(testUtilsTest GTest-ext idynutils)
add_test(tests_utils_tests ${TESTSDIR}/testUtilsTest)
