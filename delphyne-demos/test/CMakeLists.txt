# TODO(francocipollone): Improve this check by wrapping add_test() function.
# When sanitizers are activated python scripts are disabled.
if (NOT ${SANITIZERS})
  add_test(
    NAME smoke_test_delphyne_mali
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_line_single_lane
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n LineSingleLane -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_arc_single_lane
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n ArcSingleLane -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_s_shape_road
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n SShapeRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_gazoo
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-gazoo -b -d 2
  )
endif()
