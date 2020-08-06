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
    NAME smoke_test_delphyne_mali2_l_shape_road
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n LShapeRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_l_shape_road_variable_lanes
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n LShapeRoadVariableLanes -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_t_shape_road
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n TShapeRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_line_multiple_sections
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n LineMultipleSections -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_flat_town_01
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n FlatTown01 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_parking_garage_ramp
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n ParkingGarageRamp -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_parking_garage_ramp
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-mali2 -n SShapeSuperelevatedRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_gazoo
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne-gazoo -b -d 2
  )
endif()
