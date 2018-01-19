include (${project_cmake_dir}/Utils.cmake)

# ----------------------------------------
# Generate protobuf headers
add_subdirectory(protobuf)

set_source_files_properties(${PROTO_SRC} ${PROTO_HEADER}
  PROPERTIES GENERATED TRUE)

# GlobalAttributes library.
set(global_attributes GlobalAttributes)
add_library(${global_attributes} SHARED
  ${global_attributes}.cc)

install(TARGETS ${global_attributes} DESTINATION
  ${LIB_INSTALL_DIR} COMPONENT shlib)

# MaliputViewerWidget GUI plugin.
set(maliput_viewer_widget MaliputViewerWidget)
QT5_WRAP_CPP(${maliput_viewer_widget}_MOC ${maliput_viewer_widget}.hh)
add_library(${maliput_viewer_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/${maliput_viewer_widget}.cc
  OrbitViewControl.cc
  ${${maliput_viewer_widget}_MOC}
)
target_link_libraries(${maliput_viewer_widget}
  ${drake_LIBRARIES}
  ${global_attributes}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-RENDERING_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES})

install(TARGETS ${maliput_viewer_widget} DESTINATION
  ${LIB_INSTALL_DIR} COMPONENT shlib)

# RenderWidget GUI plugin.
set(render_widget RenderWidget)
QT5_WRAP_CPP(${render_widget}_MOC ${render_widget}.hh)
add_library(${render_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/${render_widget}.cc
  OrbitViewControl.cc
  ${${render_widget}_MOC}
)
target_link_libraries(${render_widget}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-RENDERING_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES})

install(TARGETS ${render_widget} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

# TeleopWidget GUI plugin.
set(teleop_widget TeleopWidget)
QT5_WRAP_CPP(${teleop_widget}_MOC ${teleop_widget}.hh)
add_library(${teleop_widget} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/${teleop_widget}.cc
  ${${teleop_widget}_MOC}
)
target_link_libraries(${teleop_widget}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne_gui_protobuf_msgs
)

install(TARGETS ${teleop_widget} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

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
  ${IGNITION-MATH_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${global_attributes}
)

install (TARGETS visualizer DESTINATION ${BIN_INSTALL_DIR})

# Install .config files.
install (FILES
  initialLayout.config layoutWithTeleop.config layoutMaliputViewer.config
  DESTINATION
  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/)

add_subdirectory(test)
