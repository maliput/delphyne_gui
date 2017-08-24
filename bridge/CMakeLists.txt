include (${project_cmake_dir}/Utils.cmake)

# lcm_mock_robot_publisher
add_executable(lcm_mock_robot_publisher lcm_mock_robot_publisher.cc)

target_link_libraries(lcm_mock_robot_publisher
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-MATH_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${lcm_LIBRARIES}
)

# lcm-ign-transport-bridge
add_executable(lcm_ign_transport_bridge
  lcm_to_ign_transport_bridge.cc
  lcm_to_ign_translation.cc)

target_link_libraries(lcm_ign_transport_bridge
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-MATH_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${lcm_LIBRARIES}
)

add_subdirectory(test)
