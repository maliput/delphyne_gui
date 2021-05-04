include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)

# Origin display plugin.
QT5_WRAP_CPP(OriginDisplay0_MOC origin_display0.hh)

add_library(origin_display0
  ${CMAKE_CURRENT_SOURCE_DIR}/origin_display0.cc
  ${OriginDisplay0_MOC}
)
add_library(delphyne_gui::origin_display0 ALIAS origin_display0)
set_target_properties(origin_display0
  PROPERTIES
    OUTPUT_NAME delphyne_gui_origin_display0
)

target_link_libraries(origin_display0
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-transport8::ignition-transport8
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(
  TARGETS origin_display0
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# Agent Info display plugin.
QT5_WRAP_CPP(AgentInfoDisplay0_MOC agent_info_display0.hh)

add_library(agent_info_display0
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_info_display0.cc
  ${AgentInfoDisplay0_MOC}
)
add_library(delphyne_gui::agent_info_display0 ALIAS agent_info_display0)
set_target_properties(agent_info_display0
  PROPERTIES
    OUTPUT_NAME delphyne_gui_agent_info_display0
)

target_link_libraries(agent_info_display0
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-transport8::ignition-transport8
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(
  TARGETS agent_info_display0
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
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
    ignition-common3::ignition-common3
    ignition-gui3::ignition-gui3
    ignition-math6::ignition-math6
    ignition-rendering3::ignition-rendering3
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
  PRIVATE
    ignition-plugin1::register
)

install(
  TARGETS AgentInfoDisplay
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
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
    ignition-common3::ignition-common3
    ignition-gui3::ignition-gui3
    ignition-math6::ignition-math6
    ignition-rendering3::ignition-rendering3
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
  PRIVATE
    ignition-plugin1::register
)

install(
  TARGETS OriginDisplay
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
