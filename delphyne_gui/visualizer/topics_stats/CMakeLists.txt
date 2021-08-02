include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
)

#-------------------------------------------------------------------------------
# TopicsStats Plugin (ign-gui 3)
QT5_WRAP_CPP(TopicsStats_headers_MOC topics_stats.hh)
QT5_ADD_RESOURCES(TopicsStats_RCC topics_stats.qrc)

add_library(TopicsStats
  ${CMAKE_CURRENT_SOURCE_DIR}/topics_stats.cc
  ${TopicsStats_headers_MOC}
  ${TopicsStats_RCC}
)
add_library(delphyne_gui::TopicsStats ALIAS TopicsStats)
set_target_properties(TopicsStats
  PROPERTIES
    OUTPUT_NAME TopicsStats
)

target_link_libraries(TopicsStats
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
  TARGETS TopicsStats
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
