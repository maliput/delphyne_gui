include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
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
