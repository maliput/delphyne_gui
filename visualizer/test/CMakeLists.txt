include (${project_cmake_dir}/Utils.cmake)
include (${project_cmake_dir}/TestUtils.cmake)

set (gtest_sources
  GlobalAttributes_TEST.cc
)

# ----------------------------------------
# Tests
delphyne_build_tests(${gtest_sources})
