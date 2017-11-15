include (${project_cmake_dir}/Utils.cmake)

# ----------------------------------------
# Generate protobuf headers
add_subdirectory(protobuf)

set_source_files_properties(${PROTO_SRC} ${PROTO_HEADER}
  PROPERTIES GENERATED TRUE)

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
)

install (TARGETS visualizer DESTINATION ${BIN_INSTALL_DIR})

# Install initialLayout.config
install (FILES initialLayout.config DESTINATION
  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/)

# Install layoutWithTeleop.config
install (FILES layoutWithTeleop.config DESTINATION
  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/delphyne/)
