# Origin display plugin.
set(origin_display OriginDisplay)
QT5_WRAP_CPP(${origin_display}_MOC origin_display.hh)
add_library(${origin_display} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/origin_display.cc
  ${${origin_display}_MOC}
)
target_link_libraries(${origin_display}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  delphyne::protobuf_messages
)

install(TARGETS ${origin_display} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib)

include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}
)
