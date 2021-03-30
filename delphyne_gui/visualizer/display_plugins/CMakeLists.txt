include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)

# Origin display plugin.
QT5_WRAP_CPP(OriginDisplay_MOC origin_display.hh)

add_library(origin_display
  ${CMAKE_CURRENT_SOURCE_DIR}/origin_display.cc
  ${OriginDisplay_MOC}
)
add_library(delphyne_gui::origin_display ALIAS origin_display)
set_target_properties(origin_display
  PROPERTIES
    OUTPUT_NAME delphyne_gui_origin_display
)

target_link_libraries(origin_display
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs5::ignition-msgs5
  ignition-transport8::ignition-transport8
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(
  TARGETS origin_display
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
