include (${project_cmake_dir}/Utils.cmake)

# ----------------------------------------
# MaliputMesh library.
set(maliput_mesh MaliputMesh)

add_library(${maliput_mesh}
  maliput_mesh_builder.cc)

target_link_libraries(${maliput_mesh}
  ${drake_LIBRARIES}
  ${IGNITION-COMMON_LIBRARIES}
  delphyne::public_headers)

install(TARGETS ${maliput_mesh} DESTINATION
  ${LIB_INSTALL_DIR} COMPONENT shlib)

# GlobalAttributes library.
set(global_attributes GlobalAttributes)
add_library(${global_attributes}
  global_attributes.cc
)
target_link_libraries(${global_attributes}
  ${IGNITION-COMMON_LIBRARIES}
)

install(TARGETS ${global_attributes} DESTINATION
  ${LIB_INSTALL_DIR} COMPONENT shlib)

# MaliputViewerWidget GUI plugin.
set(maliput_viewer_widget MaliputViewerWidget)
QT5_WRAP_CPP(${maliput_viewer_widget}_MOC maliput_viewer_widget.hh)
QT5_WRAP_CPP(LayerSelectionWidget_MOC layer_selection_widget.hh)
QT5_WRAP_CPP(RenderMaliputWidget_MOC render_maliput_widget.hh)
add_library(${maliput_viewer_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/layer_selection_widget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/maliput_viewer_widget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/render_maliput_widget.cc
  maliput_viewer_model.cc
  orbit_view_control.cc
  render_maliput_widget.cc
  ${${maliput_viewer_widget}_MOC}
  ${LayerSelectionWidget_MOC}
  ${RenderMaliputWidget_MOC}
)
target_link_libraries(${maliput_viewer_widget}
  ${drake_LIBRARIES}
  ${global_attributes}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-RENDERING_LIBRARIES}
  ${maliput_mesh}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES})

install(TARGETS ${maliput_viewer_widget} DESTINATION
  ${LIB_INSTALL_DIR} COMPONENT shlib)

# RenderWidget GUI plugin.
set(render_widget RenderWidget)
QT5_WRAP_CPP(${render_widget}_MOC render_widget.hh)
add_library(${render_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/render_widget.cc
  orbit_view_control.cc
  ${${render_widget}_MOC}
)
target_link_libraries(${render_widget}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-RENDERING_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
  delphyne::utility)

install(TARGETS ${render_widget} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

# TeleopWidget GUI plugin.
set(teleop_widget TeleopWidget)
QT5_WRAP_CPP(${teleop_widget}_MOC teleop_widget.hh)
add_library(${teleop_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/teleop_widget.cc
  ${${teleop_widget}_MOC}
)
target_link_libraries(${teleop_widget}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(TARGETS ${teleop_widget} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)

# PlaybackWidget GUI plugin.
set(playback_widget PlaybackWidget)
QT5_WRAP_CPP(${playback_widget}_MOC playback_widget.hh)
QT5_ADD_RESOURCES(${playback_widget}_resources_RCC playback.qrc)
add_library(${playback_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/playback_widget.cc
  ${${playback_widget}_MOC}
  ${${playback_widget}_resources_RCC}
)
target_link_libraries(${playback_widget}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(TARGETS ${playback_widget} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)

# Visualizer
add_executable(visualizer
  visualizer.cc
)

target_link_libraries(visualizer
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${global_attributes}
  delphyne::utility
)

install (TARGETS visualizer DESTINATION ${BIN_INSTALL_DIR})

# Install .config files.
install (FILES
  layout_with_render_only.config layout_with_teleop.config
  layout_maliput_viewer.config layout_for_playback.config
  DESTINATION
  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/layouts)

# Install .yaml files.
install (FILES
  roads/double_ring.yaml
  roads/circuit.yaml
  roads/curved_lanes.yaml
  roads/straight_lanes.yaml
  roads/little_city.yaml
  DESTINATION
  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/roads)

add_subdirectory(test)
