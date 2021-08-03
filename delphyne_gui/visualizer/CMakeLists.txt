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
  ignition-common3::ignition-common3
)

install(
  TARGETS global_attributes
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#-------------------------------------------------------------------------------
# MaliputViewerWidget GUI plugin.
QT5_WRAP_CPP(MaliputViewerWidget_MOC maliput_viewer_widget.hh)
QT5_WRAP_CPP(LayerSelectionWidget_MOC layer_selection_widget.hh)
QT5_WRAP_CPP(RenderMaliputWidget_MOC render_maliput_widget.hh)
QT5_WRAP_CPP(RulesVisualizerWidget_MOC rules_visualizer_widget.hh)

add_library(maliput_viewer_widget
  ${CMAKE_CURRENT_SOURCE_DIR}/layer_selection_widget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/maliput_viewer_widget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/render_maliput_widget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rules_visualizer_widget.cc
  orbit_view_control.cc
  render_maliput_widget.cc
  ${MaliputViewerWidget_MOC}
  ${LayerSelectionWidget_MOC}
  ${RenderMaliputWidget_MOC}
  ${RulesVisualizerWidget_MOC}
)
add_library(delphyne_gui::maliput_viewer_widget ALIAS maliput_viewer_widget)
set_target_properties(maliput_viewer_widget
  PROPERTIES
    OUTPUT_NAME delphyne_gui_maliput_viewer_widget
)

target_link_libraries(maliput_viewer_widget
  ${drake_LIBRARIES}
  delphyne_gui::arrow_mesh
  delphyne_gui::global_attributes
  delphyne_gui::maliput_viewer_model
  delphyne_gui::selector
  delphyne_gui::traffic_light_manager
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-rendering3::ignition-rendering3
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
)

install(
  TARGETS maliput_viewer_widget
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# RenderWidget GUI plugin.
QT5_WRAP_CPP(RenderWidget_MOC render_widget.hh)

add_library(render_widget
  ${CMAKE_CURRENT_SOURCE_DIR}/render_widget.cc
  orbit_view_control.cc
  ${RenderWidget_MOC}
)
add_library(delphyne_gui::render_widget ALIAS render_widget)
set_target_properties(render_widget
  PROPERTIES
    OUTPUT_NAME delphyne_gui_render_widget
)

target_link_libraries(render_widget
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-rendering3::ignition-rendering3
  ignition-transport8::ignition-transport8
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
  delphyne::utility
)

install(
  TARGETS render_widget
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# TeleopWidget GUI plugin.
QT5_WRAP_CPP(TeleopWidget_MOC teleop_widget.hh)

add_library(teleop_widget
  ${CMAKE_CURRENT_SOURCE_DIR}/teleop_widget.cc
  ${TeleopWidget_MOC}
)
add_library(delphyne_gui::teleop_widget ALIAS teleop_widget)
set_target_properties(teleop_widget
  PROPERTIES
    OUTPUT_NAME delphyne_gui_teleop_widget
)

target_link_libraries(teleop_widget
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-transport8::ignition-transport8
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(
  TARGETS teleop_widget
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# PlaybackWidget GUI plugin.
QT5_WRAP_CPP(PlaybackWidget_MOC playback_widget.hh)
QT5_ADD_RESOURCES(PlaybackWidget_resources_RCC playback.qrc)

add_library(playback_widget
  ${CMAKE_CURRENT_SOURCE_DIR}/playback_widget.cc
  ${PlaybackWidget_MOC}
  ${PlaybackWidget_resources_RCC}
)
add_library(delphyne_gui::playback_widget ALIAS playback_widget)
set_target_properties(playback_widget
  PROPERTIES
    OUTPUT_NAME delphyne_gui_playback_widget
)

target_link_libraries(playback_widget
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-transport8::ignition-transport8
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
  delphyne::public_headers
)

install(
  TARGETS playback_widget
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#-------------------------------------------------------------------------------

# Visualizer with ign-gui0
add_executable(visualizer0
  visualizer0.cc
)

target_link_libraries(visualizer0
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::utility
  global_attributes
)

install(
  TARGETS visualizer0
  EXPORT ${PROJECT_NAME}-targets
  DESTINATION bin
)

# Visualizer with ign-gui3
add_executable(visualizer
  visualizer2.cc
)

target_link_libraries(visualizer
  ignition-common3::ignition-common3
  ignition-gui3::ignition-gui3
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
    layout_with_render_only.config
    layout_with_teleop.config
    layout_maliput_viewer.config
    layout_for_playback.config
    layout2_for_playback.config
    layout2_maliput_viewer.config
    layout2_with_teleop.config
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
