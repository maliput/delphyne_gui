# TODO(francocipollone): Improve this check by wrapping add_test() function.
# When sanitizers are activated python scripts are disabled.
if (NOT ${SANITIZERS})
  add_test(
    NAME smoke_test_delphyne_mali
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_gazoo
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-gazoo -b -d 2
  )
endif()
