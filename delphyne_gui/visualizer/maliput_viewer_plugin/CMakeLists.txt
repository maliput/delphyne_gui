include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
)

# ----------------------------------------
# maliput_viewer_model library.
add_library(maliput_viewer_model
  maliput_viewer_model.cc
  maliput_mesh_converter.cc
)
add_library(delphyne_gui::maliput_viewer_model ALIAS maliput_viewer_model)
set_target_properties(maliput_viewer_model
  PROPERTIES
    OUTPUT_NAME delphyne_gui_maliput_viewer_model
)

target_link_libraries(maliput_viewer_model
  ${drake_LIBRARIES}
  delphyne::public_headers
  delphyne::roads_utilities
  ignition-common3::ignition-common3
  ignition-rendering3::ignition-rendering3
  maliput::api
  maliput::utilities
)

install(
  TARGETS maliput_viewer_model
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# ----------------------------------------
# arrow_mesh library.
add_library(arrow_mesh
  arrow_mesh.cc
)
add_library(delphyne_gui::arrow_mesh ALIAS arrow_mesh)
set_target_properties(arrow_mesh
  PROPERTIES
    OUTPUT_NAME delphyne_gui_arrow_mesh
)

target_link_libraries(arrow_mesh
  ${drake_LIBRARIES}
  ignition-common3::ignition-common3
  ignition-math6::ignition-math6
  ignition-rendering3::ignition-rendering3
)

install(
  TARGETS arrow_mesh
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# ----------------------------------------
# selector library.
add_library(selector
  selector.cc
)
add_library(delphyne_gui::selector ALIAS selector)
set_target_properties(selector
  PROPERTIES
    OUTPUT_NAME delphyne_gui_selector
)

target_link_libraries(selector
  delphyne::public_headers
  ignition-common3::ignition-common3
  ignition-math6::ignition-math6
  ignition-rendering3::ignition-rendering3
  maliput::api
)

install(
  TARGETS selector
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# ----------------------------------------
# traffic light manager library.
add_library(traffic_light_manager
  traffic_light_manager.cc
)
add_library(delphyne_gui::traffic_light_manager ALIAS traffic_light_manager)
set_target_properties(traffic_light_manager
  PROPERTIES
    OUTPUT_NAME delphyne_gui_traffic_light_manager
)

target_link_libraries(traffic_light_manager
  delphyne::public_headers
  ignition-common3::ignition-common3
  ignition-math6::ignition-math6
  ignition-rendering3::ignition-rendering3
  maliput::api
)

install(
  TARGETS traffic_light_manager
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#-------------------------------------------------------------------------------
# MaliputViewerPlugin (ign-gui 3)
QT5_WRAP_CPP(MaliputViewerPlugin_headers_MOC maliput_viewer_plugin.hh)
QT5_ADD_RESOURCES(MaliputViewerPlugin_RCC maliput_viewer_plugin.qrc)

add_library(MaliputViewerPlugin
  ${CMAKE_CURRENT_SOURCE_DIR}/maliput_viewer_plugin.cc
  ${MaliputViewerPlugin_headers_MOC}
  ${MaliputViewerPlugin_RCC}
)
add_library(delphyne_gui::MaliputViewerPlugin ALIAS MaliputViewerPlugin)
set_target_properties(MaliputViewerPlugin
  PROPERTIES
    OUTPUT_NAME MaliputViewerPlugin
)

target_link_libraries(MaliputViewerPlugin
    delphyne_gui::arrow_mesh
    delphyne_gui::global_attributes
    delphyne_gui::maliput_viewer_model
    delphyne_gui::selector
    delphyne_gui::traffic_light_manager
    ignition-gui3::ignition-gui3
    ignition-common3::ignition-common3
    ignition-transport8::ignition-transport8
    ignition-rendering3::ignition-rendering3
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ignition-plugin1::register
)

install(
  TARGETS MaliputViewerPlugin
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
