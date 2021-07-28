include (${project_cmake_dir}/Utils.cmake)
include (CheckCXXSourceCompiles)

########################################
# Find drake
find_package(drake REQUIRED)
if (NOT drake_FOUND)
  message(STATUS "Looking for drake-config.cmake - not found")
  BUILD_ERROR ("Missing: Drake library (libdrake).")
else()
  message(STATUS "Looking for drake-config.cmake - found")
  include_directories(${DRAKE_INCLUDE_DIRS})
  link_directories(${DRAKE_LIBRARY_DIRS})
endif()

########################################
# Find ignition common
find_package(ignition-common3 REQUIRED)
if (NOT ignition-common3_FOUND)
  message(STATUS "Looking for ignition-common3-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition common0 library (libignition-common3-dev).")
else()
  message(STATUS "Looking for ignition-common3-config.cmake - found")
endif()

########################################
# Find ignition math
find_package(ignition-math6 REQUIRED)
if (NOT ignition-math6_FOUND)
  message(STATUS "Looking for ignition-math6-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition math4 library (libignition-math6-dev).")
else()
  message(STATUS "Looking for ignition-math6-config.cmake - found")
endif()

########################################
# Find ignition msgs
find_package(ignition-msgs5 QUIET)
if (NOT ignition-msgs5_FOUND)
  message(STATUS "Looking for ignition-msgs5-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition msgs5 library (libignition-msgs5-dev).")
else()
  message(STATUS "Looking for ignition-msgs5-config.cmake - found")
endif()

########################################
# Find ignition gui0
find_package(ignition-gui0 0.1 QUIET)
if (NOT ignition-gui0_FOUND)
  message(STATUS "Looking for ignition-gui0-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition gui0 library (libignition-gui0-dev).")
else()
  message(STATUS "Looking for ignition-gui0-config.cmake - found")
endif()

########################################
# Find ignition gui3
find_package(ignition-gui3 QUIET)
if (NOT ignition-gui3_FOUND)
  message(STATUS "Looking for ignition-gui3-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition gui3 library (libignition-gui3-dev).")
else()
  message(STATUS "Looking for ignition-gui3-config.cmake - found")
endif()

########################################
# Find ignition rendering
find_package(ignition-rendering3 QUIET)
if (NOT ignition-rendering3_FOUND)
  message(STATUS "Looking for ignition-rendering3-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition rendering3 library (libignition-rendering3-dev).")
else()
  message(STATUS "Looking for ignition-rendering3-config.cmake - found")
endif()

########################################
# Find ignition transport
find_package(ignition-transport8 REQUIRED)
if (NOT ignition-transport8_FOUND)
  message(STATUS "Looking for ignition-transport8-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition transport8 library (libignition-transport8-dev).")
else()
  message(STATUS "Looking for ignition-transport8-config.cmake - found")
endif()

########################################
# Find QT
find_package (Qt5Widgets)
if (NOT Qt5Widgets_FOUND)
  BUILD_ERROR("Missing: Qt5Widgets")
endif()

find_package (Qt5Core)
if (NOT Qt5Core_FOUND)
  BUILD_ERROR("Missing: Qt5Core")
endif()

find_package (Qt5OpenGL)
if (NOT Qt5OpenGL_FOUND)
  BUILD_ERROR("Missing: Qt5OpenGL")
endif()

find_package (Qt5Quick)
if (NOT Qt5Quick_FOUND)
  BUILD_ERROR("Missing: Qt5Quick")
endif()

find_package (Qt5QuickControls2)
if (NOT Qt5QuickControls2_FOUND)
  BUILD_ERROR("Missing: Qt5QuickControls2")
endif()

find_package (Qt5Test)
if (NOT Qt5Test_FOUND)
  BUILD_ERROR("Missing: Qt5Test")
endif()

########################################
# Find Delphyne
find_package(delphyne REQUIRED)
find_package(maliput REQUIRED)
