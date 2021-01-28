##############################################################################
# Project
##############################################################################

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(delphyne_demos)

include(${PROJECT_SOURCE_DIR}/cmake/SanitizersConfig.cmake)

##############################################################################
# Find 3rd Party Packages
##############################################################################

message(STATUS "\n\n====== Finding 3rd Party Packages ======\n")

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

##############################################################################
# Python Setup
##############################################################################

# Pin down python 3.6
find_package(PythonInterp 3.6 REQUIRED)

# Unfortunately no easy way to get site-packages / dist-packages
set(PYTHON_INSTALL_DIR lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)

##############################################################################
# Tests
##############################################################################

if(BUILD_TESTING)
  find_package(ament_cmake_flake8 REQUIRED)
  enable_testing()
  ament_flake8("--config" ${CMAKE_CURRENT_SOURCE_DIR}/.flake8)
endif()

add_subdirectory(demos)
add_subdirectory(examples)
add_subdirectory(test)

ament_package()
