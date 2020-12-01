include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)

# Origin display plugin.
set(origin_display OriginDisplay)
QT5_WRAP_CPP(${origin_display}_MOC origin_display.hh)
add_library(${origin_display} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/origin_display.cc
  ${${origin_display}_MOC}
)
target_link_libraries(${origin_display}
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs4::ignition-msgs4
  ignition-transport7::ignition-transport7
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(TARGETS ${origin_display} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

# Origin display plugin.
set(agent_info_display AgentInfoDisplay)
QT5_WRAP_CPP(${agent_info_display}_MOC agent_info_display.hh)
add_library(${agent_info_display} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_info_display.cc
  ${${agent_info_display}_MOC}
)
target_link_libraries(${agent_info_display}
  ignition-common3::ignition-common3
  ignition-gui0::ignition-gui0
  ignition-msgs4::ignition-msgs4
  ignition-transport7::ignition-transport7
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(TARGETS ${agent_info_display} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)
