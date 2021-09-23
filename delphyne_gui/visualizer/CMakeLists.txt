include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
)

# global_attributes library.
add_library(global_attributes
  global_attributes.cc
)
add_library(delphyne_gui::global_attributes ALIAS global_attributes)
set_target_properties(global_attributes
  PROPERTIES
    OUTPUT_NAME delphyne_gui_global_attributes
)

target_link_libraries(global_attributes
  ignition-common4::ignition-common4
)

install(
  TARGETS global_attributes
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# Visualizer
add_executable(visualizer
  visualizer.cc
)

target_link_libraries(visualizer
  ignition-common4::ignition-common4
  ignition-gui5::ignition-gui5
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::utility
  global_attributes
)

install(
  TARGETS visualizer
  EXPORT ${PROJECT_NAME}-targets
  DESTINATION bin
)

# Install .config files.
install(
  FILES
    layout_for_playback.config
    layout_maliput_viewer.config
    layout_with_teleop.config
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/layouts
)

# Install .yaml files.
install(
  FILES
    roads/double_ring.yaml
    roads/circuit.yaml
    roads/curved_lanes.yaml
    roads/straight_lanes.yaml
    roads/little_city.yaml
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/roads
)

install(
  FILES
    resources/arrow_bulb.obj
    resources/arrow_bulb.mtl
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/resources
)

add_subdirectory(display_plugins)
add_subdirectory(maliput_viewer_plugin)
add_subdirectory(playback_plugin)
add_subdirectory(teleop_plugin)
add_subdirectory(topic_interface_plugin)
add_subdirectory(topics_stats)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
