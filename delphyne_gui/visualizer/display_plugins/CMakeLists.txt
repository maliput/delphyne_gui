include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
)

#-------------------------------------------------------------------------------
# AgentInfo display (ign-gui 3)
QT5_WRAP_CPP(AgentInfoDisplay_MOC agent_info_display.hh)
QT5_ADD_RESOURCES(AgentInfoDisplay_RCC agent_info_display.qrc)

add_library(AgentInfoDisplay
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_info_display.cc
  ${AgentInfoDisplay_MOC}
  ${AgentInfoDisplay_RCC}
)
add_library(delphyne_gui::AgentInfoDisplay ALIAS AgentInfoDisplay)
set_target_properties(AgentInfoDisplay
  PROPERTIES
    OUTPUT_NAME AgentInfoDisplay
)

target_link_libraries(AgentInfoDisplay
  PUBLIC
    delphyne::protobuf_messages
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
  TARGETS AgentInfoDisplay
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/gui_plugins
  ARCHIVE DESTINATION lib/gui_plugins
)

#-------------------------------------------------------------------------------
# Origin display (ign-gui 3)
QT5_WRAP_CPP(OriginDisplay_MOC origin_display.hh)
QT5_ADD_RESOURCES(OriginDisplay_RCC origin_display.qrc)

add_library(OriginDisplay
  ${CMAKE_CURRENT_SOURCE_DIR}/origin_display.cc
  ${OriginDisplay_MOC}
  ${OriginDisplay_RCC}
)
add_library(delphyne_gui::OriginDisplay ALIAS OriginDisplay)
set_target_properties(OriginDisplay
  PROPERTIES
    OUTPUT_NAME OriginDisplay
)

target_link_libraries(OriginDisplay
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
  TARGETS OriginDisplay
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/gui_plugins
  ARCHIVE DESTINATION lib/gui_plugins
)
