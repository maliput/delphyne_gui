#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#
"""
The gazoo demo.
```
"""
##############################################################################
# Imports
##############################################################################

import os.path

import delphyne.maliput as maliput
import delphyne.simulation as simulation
import delphyne.utilities
import delphyne_gui.utilities

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "Gazoo Racing!",
        """
An example of three railcars and a variable number of MOBIL controlled
cars running in a closed-loop maliput road.

See also https://toyotagazooracing.com/
        """
    )
    parser.add_argument("-n", "--num-cars", default=3, type=int,
                        help="The number of MOBIL cars on scene (default: 3).")

    return parser.parse_args()

##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    if args.num_cars > 6 or args.num_cars < 0:
        print("The number of cars must be in the range of 0 to 6.")
        quit()

    mobil_cars_num = args.num_cars

    builder = simulation.AgentSimulationBuilder()

    filename = delphyne_gui.utilities.get_delphyne_gui_resource(
        'roads/circuit.yaml')

    if not os.path.isfile(filename):
        print("Required file {} not found."
              " Please, make sure to install the latest delphyne-gui."
              .format(os.path.abspath(filename)))
        quit()

    features = maliput.ObjFeatures()
    features.draw_arrows = True
    features.draw_elevation_bounds = False
    features.draw_stripes = True
    features.draw_lane_haze = False
    features.draw_branch_points = False

    # The road geometry
    road_geometry = builder.set_road_geometry(
        maliput.create_multilane_from_file(
            file_path=filename
        ), features
    )

    # Setup railcar 1
    railcar_speed = 4.0  # (m/s)
    railcar_s = 0.0      # (m)
    robot_id = 1
    lane_1 = road_geometry.junction(2).segment(0).lane(0)
    delphyne.utilities.add_rail_car(
        builder,
        name=str(robot_id),
        lane=lane_1,
        position=railcar_s,
        offset=0.0,
        speed=railcar_speed
    )

    # Setup railcar 2
    railcar_speed = 8.0  # (m/s)
    railcar_s = 0.0      # (m)
    robot_id += 1
    lane_2 = road_geometry.junction(2).segment(0).lane(1)
    delphyne.utilities.add_rail_car(
        builder,
        name=str(robot_id),
        lane=lane_2,
        position=railcar_s,
        offset=0.0,
        speed=railcar_speed
    )

    # Setup railcar 3
    railcar_speed = 7.0  # (m/s)
    railcar_s = 0.0      # (m)
    robot_id += 1
    lane_3 = road_geometry.junction(2).segment(0).lane(2)
    delphyne.utilities.add_rail_car(
        builder,
        name=str(robot_id),
        lane=lane_3,
        position=railcar_s,
        offset=0.0,
        speed=railcar_speed
    )

    # Setup MOBIL cars.
    for i in range(mobil_cars_num):
        x_offset = 5.0       # (m)
        y_offset = 5.0       # (m)
        velocity_base = 2.0  # (m/s)
        robot_id += 1
        delphyne.utilities.add_mobil_car(
            builder,
            name=str(robot_id),
            scene_x=-10.0 + x_offset * (1 + i / 3),
            scene_y=0.0 + y_offset * (i % 3),
            heading=0.0,
            speed=velocity_base * i
        )

    runner = simulation.SimulationRunner(
        simulation=builder.build(),
        time_step=0.015,  # (secs)
        realtime_rate=args.realtime_rate,
        paused=args.paused,
        log=args.log,
        logfile_name=args.logfile_name)

    with launch_interactive_simulation(runner, bare=args.bare) as launcher:
        if args.duration < 0:
            # run indefinitely
            runner.start()
        else:
            # run for a finite time
            print("Running simulation for {0} seconds.".format(
                args.duration))
            runner.run_async_for(args.duration, launcher.terminate)
