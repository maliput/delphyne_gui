include (${project_cmake_dir}/TestUtils.cmake)

set (gtest_sources
  global_attributes_TEST.cc
)

# ----------------------------------------
# Tests
delphyne_build_tests(${gtest_sources})
