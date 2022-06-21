include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
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
  LIBRARY DESTINATION lib/gui_plugins
  ARCHIVE DESTINATION lib/gui_plugins
)
