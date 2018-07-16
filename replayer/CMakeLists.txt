include (${project_cmake_dir}/Utils.cmake)

# Replayer
add_executable(replayer
  replayer.cc
)

target_link_libraries(replayer
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
)

install (TARGETS replayer DESTINATION ${BIN_INSTALL_DIR})
