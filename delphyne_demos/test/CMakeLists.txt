# TODO(francocipollone): Improve this check by wrapping add_test() function.
# When sanitizers are activated python scripts are disabled.
if (NOT ${SANITIZERS})
  add_test(
    NAME smoke_test_delphyne_mali
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne_mali -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_line_single_lane
    COMMAND delphyne_mali2 -n LineSingleLane -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_arc_single_lane
    COMMAND delphyne_mali2 -n ArcSingleLane -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_s_shape_road
    COMMAND delphyne_mali2 -n SShapeRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_l_shape_road
    COMMAND delphyne_mali2 -n LShapeRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_l_shape_road_variable_lanes
    COMMAND delphyne_mali2 -n LShapeRoadVariableLanes -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_t_shape_road
    COMMAND delphyne_mali2 -n TShapeRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_line_multiple_sections
    COMMAND delphyne_mali2 -n LineMultipleSections -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_flat_town_01
    COMMAND delphyne_mali2 -n FlatTown01 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_parking_garage_ramp
    COMMAND delphyne_mali2 -n ParkingGarageRamp -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_s_shape_superelevated_road
    COMMAND delphyne_mali2 -n SShapeSuperelevatedRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_r_r_long_road
    COMMAND delphyne_mali2 -n RRLongRoad -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_highway
    COMMAND delphyne_mali2 -n Highway -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_variable_width
    COMMAND delphyne_mali2 -n LineVariableWidth -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_figure_8
    COMMAND delphyne_mali2 -n Figure8 -b -d 2
  )
  # TODO(maliput_malidrive#6): Restore once we have the right tolerance value.
  add_test(
    NAME smoke_test_delphyne_mali2_r_r_figure_8
    COMMAND delphyne_mali2 -n RRFigure8 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_01
    COMMAND delphyne_mali2 -n Town01 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_02
    COMMAND delphyne_mali2 -n Town02 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_03
    COMMAND delphyne_mali2 -n Town03 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_04
    COMMAND delphyne_mali2 -n Town04 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_05
    COMMAND delphyne_mali2 -n Town05 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_06
    COMMAND delphyne_mali2 -n Town06 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_town_07
    COMMAND delphyne_mali2 -n Town07 -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_mali2_variable_offset
    COMMAND delphyne_mali2 -n LineVariableOffset -b -d 2
  )
  add_test(
    NAME smoke_test_delphyne_gazoo
    COMMAND ${PROJECT_SOURCE_DIR}/examples/delphyne_gazoo -b -d 2
  )
endif()
