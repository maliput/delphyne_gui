##################################################
macro (delphyne_build_tests)
  # Find the Python interpreter for running the
  # check_test_ran.py script
  find_package(PythonInterp QUIET)

  # Build all the tests
  foreach(GTEST_SOURCE_file ${ARGN})
    string(REGEX REPLACE ".cc" "" BINARY_NAME ${GTEST_SOURCE_file})
    set(BINARY_NAME ${TEST_TYPE}_${BINARY_NAME})

    ament_add_gtest(${BINARY_NAME} ${GTEST_SOURCE_file} TIMEOUT 240)

    # Kind of an ugly catch-all bucket
    target_link_libraries(
      ${BINARY_NAME}
        ignition-common3::ignition-common3
        delphyne_gui::global_attributes
        pthread
    )

    # Remove a warning in GTest.
    target_compile_options(${BINARY_NAME} PRIVATE "-Wno-sign-compare")
  endforeach()
endmacro()
