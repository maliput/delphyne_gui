##############################################################################
# Project
##############################################################################

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project (delphyne_gui)

string (TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string (TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

set (PROJECT_MAJOR_VERSION 0)
set (PROJECT_MINOR_VERSION 1)
set (PROJECT_PATCH_VERSION 1)

set (PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION})
set (PROJECT_VERSION_FULL ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION})

message (STATUS "${PROJECT_NAME} version ${PROJECT_VERSION_FULL}")

##############################################################################
# CMake Support
##############################################################################

# Useful string/list macros
include (${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)

# Use GNUInstallDirst to get canonical paths
include(GNUInstallDirs)

set (project_cmake_dir ${PROJECT_SOURCE_DIR}/cmake
  CACHE PATH "Location of CMake scripts")

##############################################################################
# Project Configuration
##############################################################################

message(STATUS "\n\n========= Project Configuration ========\n")

set(BUILD_SHARED_LIBS true)


##############################################################################
# Find 3rd Party Packages
##############################################################################

message(STATUS "\n\n====== Finding 3rd Party Packages ======\n")

find_package(ament_cmake REQUIRED)

# ignition
find_package(ignition-common3 REQUIRED)
find_package(ignition-math6 REQUIRED)
find_package(ignition-msgs5 REQUIRED)
find_package(ignition-gui0 0.1 REQUIRED)
find_package(ignition-gui3 REQUIRED)
find_package(ignition-rendering3 REQUIRED)
find_package(ignition-transport8 REQUIRED)

# Qt
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Test REQUIRED)

# delphyne, drake and maliput
find_package(drake_vendor REQUIRED)
find_package(delphyne REQUIRED)
find_package(maliput REQUIRED)

set (CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set (BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})

set (BUILD_DELPHYNE ON CACHE INTERNAL
  "Build Delphyne GUI" FORCE)

include (${project_cmake_dir}/DissectVersion.cmake)

#####################################
# Handle CFlags
unset (CMAKE_CXX_FLAGS CACHE)

include(${project_cmake_dir}/DefaultCFlags.cmake)

# Check whether sanitizers should be enabled or not.
include(${project_cmake_dir}/SanitizersConfig.cmake)

# Check if warning options are avaliable for the compiler and return WARNING_CXX_FLAGS variable
list(APPEND WARN_LEVEL -Waddress -Warray-bounds -Wcomment -Wformat -Wnonnull)
list(APPEND WARN_LEVEL -Wparentheses -Wreorder -Wreturn-type)
list(APPEND WARN_LEVEL -Wsequence-point -Wsign-compare -Wstrict-aliasing)
list(APPEND WARN_LEVEL -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized)
list(APPEND WARN_LEVEL -Wunused-function -Wunused-label -Wunused-value)
list(APPEND WARN_LEVEL -Wunused-variable -Wvolatile-register-var)

  # Unable to be filtered flags (failing due to limitations in filter_valid_compiler_warnings)
  set(UNFILTERED_FLAGS "-Wc++11-compat")

filter_valid_compiler_warnings(${WARN_LEVEL} -Wextra -Wno-long-long
  -Wno-unused-value -Wno-unused-value -Wno-unused-value -Wno-unused-value
  -Wshadow -Winit-self -Wswitch-default
  -Wmissing-include-dirs -pedantic -Wno-pragmas)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}${WARNING_CXX_FLAGS} ${UNFILTERED_FLAGS}")

########################################
# Write the config.hh file
configure_file (${project_cmake_dir}/config.hh.in
  ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.hh)

include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_BINARY_DIR}/visualizer
)
link_directories(${PROJECT_BINARY_DIR}/src)

if (DEFINED CMAKE_CXX_FLAGS)
  message (STATUS "Custom CFlags:${CMAKE_CXX_FLAGS}")
else()
  message (STATUS "Use default CFlags")
endif()

add_subdirectory(visualizer)

if (BUILD_TESTING)
  set(TEST_TYPE "UNIT")
  enable_testing()
  find_package(ament_cmake_clang_format REQUIRED)
  find_package(ament_cmake_flake8 REQUIRED)
  add_subdirectory(test)
  ament_flake8("--config" ${CMAKE_CURRENT_SOURCE_DIR}/.flake8)
  ament_clang_format(CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../.clang-format)
endif()

add_subdirectory(python)
ament_environment_hooks(setup.sh.in)

#################################################
# Installs maliput_viewer0.sh and maliput_viewer2.sh as a shortcut to run both
# visualizers.
install (PROGRAMS tools/maliput_viewer0.sh DESTINATION ${BIN_INSTALL_DIR})
install (PROGRAMS tools/maliput_viewer2.sh DESTINATION ${BIN_INSTALL_DIR})

ament_package()
