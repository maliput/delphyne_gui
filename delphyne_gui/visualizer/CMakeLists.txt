include (${project_cmake_dir}/Utils.cmake)

include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)

# ----------------------------------------
# maliput_viewer_model library.
add_library(maliput_viewer_model
  maliput_viewer_model.cc
  maliput_mesh_converter.cc
)
add_library(delphyne_gui::maliput_viewer_model ALIAS maliput_viewer_model)
set_target_properties(maliput_viewer_model
  PROPERTIES
    OUTPUT_NAME delphyne_gui_maliput_viewer_model
)

ament_target_dependencies(maliput_viewer_model
  "maliput"
  "maliput_multilane"
)

target_link_libraries(maliput_viewer_model
  ${drake_LIBRARIES}
  delphyne::public_headers
  delphyne::roads_utilities
  ignition-common3::ignition-common3
  ignition-rendering3::ignition-rendering3
  maliput::api
  maliput::utilities
)

install(
  TARGETS maliput_viewer_model
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# ----------------------------------------
# arrow_mesh library.
add_library(arrow_mesh
  arrow_mesh.cc
)
add_library(delphyne_gui::arrow_mesh ALIAS arrow_mesh)
set_target_properties(arrow_mesh
  PROPERTIES
    OUTPUT_NAME delphyne_gui_arrow_mesh
)

target_link_libraries(arrow_mesh
  ${drake_LIBRARIES}
  ignition-common3::ignition-common3
  ignition-math6::ignition-math6
  ignition-rendering3::ignition-rendering3
)

install(
  TARGETS arrow_mesh
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# ----------------------------------------
# selector library.
add_library(selector
  selector.cc
)
add_library(delphyne_gui::selector ALIAS selector)
set_target_properties(selector
  PROPERTIES
    OUTPUT_NAME delphyne_gui_selector
)

ament_target_dependencies(selector
  "maliput"
)

target_link_libraries(selector
  delphyne::public_headers
  ignition-common3::ignition-common3
  ignition-math6::ignition-math6
  ignition-rendering3::ignition-rendering3
  maliput::api
)

install(
  TARGETS selector
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# ----------------------------------------
# traffic light manager library.
add_library(traffic_light_manager
  traffic_light_manager.cc
)
add_library(delphyne_gui::traffic_light_manager ALIAS traffic_light_manager)
set_target_properties(traffic_light_manager
  PROPERTIES
    OUTPUT_NAME delphyne_gui_traffic_light_manager
)

ament_target_dependencies(traffic_light_manager
  "maliput"
)

target_link_libraries(traffic_light_manager
  delphyne::public_headers
  ignition-common3::ignition-common3
  ignition-math6::ignition-math6
  ignition-rendering3::ignition-rendering3
  maliput::api
)

install(
  TARGETS traffic_light_manager
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
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
# MaliputViewerPlugin (ign-gui 3)
QT5_WRAP_CPP(MaliputViewerPlugin_headers_MOC maliput_viewer_plugin.hh)
QT5_ADD_RESOURCES(MaliputViewerPlugin_RCC maliput_viewer_plugin.qrc)

add_library(MaliputViewerPlugin
  ${CMAKE_CURRENT_SOURCE_DIR}/maliput_viewer_plugin.cc
  ${MaliputViewerPlugin_headers_MOC}
  ${MaliputViewerPlugin_RCC}
)
add_library(delphyne_gui::MaliputViewerPlugin ALIAS MaliputViewerPlugin)
set_target_properties(MaliputViewerPlugin
  PROPERTIES
    OUTPUT_NAME MaliputViewerPlugin
)

target_link_libraries(MaliputViewerPlugin
    delphyne_gui::arrow_mesh
    delphyne_gui::global_attributes
    delphyne_gui::maliput_viewer_model
    delphyne_gui::selector
    delphyne_gui::traffic_light_manager
    ignition-gui3::ignition-gui3
    ignition-common3::ignition-common3
    ignition-transport8::ignition-transport8
    ignition-rendering3::ignition-rendering3
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ignition-plugin1::register
)

install(
  TARGETS MaliputViewerPlugin
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
# Teleop Plugin (ign-gui 3)
QT5_WRAP_CPP(TeleopPlugin_headers_MOC teleop_plugin.h)
QT5_ADD_RESOURCES(TeleopPlugin_RCC teleop_plugin.qrc)

add_library(TeleopPlugin
  ${CMAKE_CURRENT_SOURCE_DIR}/teleop_plugin.cc
  ${TeleopPlugin_headers_MOC}
  ${TeleopPlugin_RCC}
)
add_library(delphyne_gui::TeleopPlugin ALIAS TeleopPlugin)
set_target_properties(TeleopPlugin
  PROPERTIES
    OUTPUT_NAME TeleopPlugin
)

target_link_libraries(TeleopPlugin
  PUBLIC
    ignition-gui3::ignition-gui3
    ignition-common3::ignition-common3
    ignition-transport8::ignition-transport8
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    delphyne::protobuf_messages
  PRIVATE
    ignition-plugin1::register
)

install(
  TARGETS TeleopPlugin
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#-------------------------------------------------------------------------------
# TopicsStats Plugin (ign-gui 3)
QT5_WRAP_CPP(TopicsStats_headers_MOC topics_stats.hh)
QT5_ADD_RESOURCES(TopicsStats_RCC topics_stats.qrc)

add_library(TopicsStats
  ${CMAKE_CURRENT_SOURCE_DIR}/topics_stats.cc
  ${TopicsStats_headers_MOC}
  ${TopicsStats_RCC}
)
add_library(delphyne_gui::TopicsStats ALIAS TopicsStats)
set_target_properties(TopicsStats
  PROPERTIES
    # TODO(#379): Use TopicsStats instead after removing ign-gui0 from workspace.
    OUTPUT_NAME TopicsStats
)

target_link_libraries(TopicsStats
  PUBLIC
    ignition-gui3::ignition-gui3
    ignition-common3::ignition-common3
    ignition-transport8::ignition-transport8
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    delphyne::protobuf_messages
  PRIVATE
    ignition-plugin1::register
)

install(
  TARGETS TopicsStats
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#-------------------------------------------------------------------------------

# PlaybackWidget (ign-gui 3)
QT5_WRAP_CPP(PlaybackPlugin_MOC playback_plugin.hh)
QT5_ADD_RESOURCES(PlaybackPlugin_resources_RCC PlaybackPlugin.qrc)

add_library(PlaybackPlugin
  ${CMAKE_CURRENT_SOURCE_DIR}/playback_plugin.cc
  ${PlaybackPlugin_MOC}
  ${PlaybackPlugin_resources_RCC}
)
add_library(delphyne_gui::PlaybackPlugin ALIAS PlaybackPlugin)
set_target_properties(PlaybackPlugin
  PROPERTIES
    OUTPUT_NAME PlaybackPlugin
)

ament_target_dependencies(PlaybackPlugin
  "maliput"
)

target_link_libraries(PlaybackPlugin
  PUBLIC
    ignition-common3::ignition-common3
    ignition-gui3::ignition-gui3
    ignition-msgs5::ignition-msgs5
    ignition-transport8::ignition-transport8
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    delphyne::protobuf_messages
    delphyne::public_headers
    maliput::common
  PRIVATE
    ignition-plugin1::register
)

install(
  TARGETS PlaybackPlugin
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
  DESTINATION ${BIN_INSTALL_DIR}
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
  DESTINATION ${BIN_INSTALL_DIR}
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
add_subdirectory(topic_interface_plugin)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
