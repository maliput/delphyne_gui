include (${project_cmake_dir}/Utils.cmake)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/
  ${CMAKE_BINARY_DIR}/test/
)

# lcm_channel_repeater_test
delphyne_build_test(lcm_channel_repeater_test.cc
  ../lcm_to_ign_translation.cc
)

# lcm_to_delphyne_translation_test
delphyne_build_test(lcm_to_ign_translation_test.cc
  ../lcm_to_ign_translation.cc
)
