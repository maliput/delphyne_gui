include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
)

#-------------------------------------------------------------------------------
# TopicInterfacePlugin (ign-gui 3)
QT5_WRAP_CPP(TopicInterfacePlugin_MOC topic_interface_plugin.h)
QT5_ADD_RESOURCES(TopicInterfacePlugin_RCC topic_interface_plugin.qrc)

add_library(TopicInterfacePlugin
  ${CMAKE_CURRENT_SOURCE_DIR}/topic_interface_plugin.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/message.cc
  ${TopicInterfacePlugin_MOC}
  ${TopicInterfacePlugin_RCC}
)
add_library(delphyne_gui::TopicInterfacePlugin ALIAS TopicInterfacePlugin)
set_target_properties(TopicInterfacePlugin
  PROPERTIES
    OUTPUT_NAME TopicInterfacePlugin
)

target_link_libraries(TopicInterfacePlugin
  PUBLIC
    ignition-common4::ignition-common4
    ignition-gui6::ignition-gui6
    ignition-math6::ignition-math6
    ignition-rendering6::ignition-rendering6
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
  PRIVATE
    ignition-plugin1::register
)

install(
  TARGETS TopicInterfacePlugin
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/gui_plugins
  ARCHIVE DESTINATION lib/gui_plugins
)
