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

# Use GNUInstallDirst to get canonical paths
include(GNUInstallDirs)

set (project_cmake_dir ${PROJECT_SOURCE_DIR}/cmake
  CACHE PATH "Location of CMake scripts")

##############################################################################
# Project Configuration
##############################################################################

message(STATUS "\n\n========= Project Configuration ========\n")

set(BUILD_SHARED_LIBS true)

include(${project_cmake_dir}/DefaultCFlags.cmake)
# Check whether sanitizers should be enabled or not.
include(${project_cmake_dir}/SanitizersConfig.cmake)

##############################################################################
# Find 3rd Party Packages
##############################################################################

message(STATUS "\n\n====== Finding 3rd Party Packages ======\n")

find_package(ament_cmake REQUIRED)

# ignition
find_package(ignition-common4 REQUIRED)
find_package(ignition-math6 REQUIRED)
find_package(ignition-msgs8 REQUIRED)
find_package(ignition-gui6 REQUIRED)
find_package(ignition-rendering6 REQUIRED)
find_package(ignition-transport11 REQUIRED)

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

# TODO(#443): Probably drake_vendor exposes its own ignition-math6 which
#             collides with the ignition-exported one.
find_package(ignition-math6 REQUIRED)

set (CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

set (BUILD_DELPHYNE ON CACHE INTERNAL
  "Build Delphyne GUI" FORCE)

include (${project_cmake_dir}/DissectVersion.cmake)

########################################
# Write the config.hh file
configure_file (${project_cmake_dir}/config.hh.in
  ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.hh)

include_directories(
  # This include path is required to find the config.hh file. It won't be
  # installed but it is required in the build space.
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_BINARY_DIR}/visualizer
)
link_directories(${PROJECT_BINARY_DIR}/src)

##############################################################################
# Tests
##############################################################################

if (BUILD_TESTING)
  set(TEST_TYPE "UNIT")
  enable_testing()
  find_package(ament_cmake_clang_format REQUIRED)
  find_package(ament_cmake_flake8 REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  ament_flake8("--config" ${CMAKE_CURRENT_SOURCE_DIR}/.flake8)
  ament_clang_format(CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../.clang-format)
endif()

##############################################################################
# Docs
##############################################################################

if(NOT DEFINED BUILD_DOCS)
  set(BUILD_DOCS 1)
endif()

if(BUILD_DOCS)
  find_package(ament_cmake_doxygen REQUIRED)
  ament_doxygen_generate(doxygen_maliput_malidrive
    CONFIG_OVERLAY doc/Doxyfile.overlay.in
    DEPENDENCIES delphyne maliput
    TEST_ON_WARNS
  )
endif()

add_subdirectory(visualizer)
add_subdirectory(python)

ament_environment_hooks(setup.sh.in)

#################################################
# Installs maliput_viewer2.sh as a shortcut to run the visualizer.
install (PROGRAMS tools/maliput_viewer.sh DESTINATION bin)

ament_package()
