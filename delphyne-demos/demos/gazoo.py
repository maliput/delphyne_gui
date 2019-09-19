#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#
"""
The gazoo demo.
"""

##############################################################################
# Imports
##############################################################################

import os.path

import delphyne.behaviours
import delphyne.trees
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


def create_gazoo_scenario_subtree(filename, mobil_cars_num):
    # The road geometry
    scenario_subtree = delphyne.behaviours.roads.Multilane(
        file_path=filename,
        name="circuit",
    )

    # Setup railcar 1
    railcar_speed = 4.0  # (m/s)
    railcar_s = 0.0      # (m)
    robot_id = 1
    lane_1 = "l:s1_0"
    scenario_subtree.add_child(
        delphyne.behaviours.agents.RailCar(
            name=str(robot_id),
            lane_id=lane_1,
            longitudinal_position=railcar_s,
            lateral_offset=0.0,
            speed=railcar_speed
        )
    )

    # Setup railcar 2
    railcar_speed = 8.0  # (m/s)
    railcar_s = 0.0      # (m)
    robot_id += 1
    lane_2 = "l:s1_1"
    scenario_subtree.add_child(
        delphyne.behaviours.agents.RailCar(
            name=str(robot_id),
            lane_id=lane_2,
            longitudinal_position=railcar_s,
            lateral_offset=0.0,
            speed=railcar_speed
        )
    )

    # Setup railcar 3
    railcar_speed = 7.0  # (m/s)
    railcar_s = 0.0      # (m)
    robot_id += 1
    lane_3 = "l:s1_2"
    scenario_subtree.add_child(
        delphyne.behaviours.agents.RailCar(
            name=str(robot_id),
            lane_id=lane_3,
            longitudinal_position=railcar_s,
            lateral_offset=0.0,
            speed=railcar_speed
        )
    )

    # Setup MOBIL cars.
    for i in range(mobil_cars_num):
        x_offset = 5.0       # (m)
        y_offset = 5.0       # (m)
        velocity_base = 2.0  # (m/s)
        robot_id += 1
        scenario_subtree.add_child(
            delphyne.behaviours.agents.MobilCar(
                name=str(robot_id),
                initial_pose=(
                    -10.0 + x_offset * (1 + i / 3),
                     0.0 + y_offset * (i % 3),
                     0.0
                ),
                speed=velocity_base * i
            )
        )

    return scenario_subtree


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

    filename = delphyne_gui.utilities.get_delphyne_gui_resource(
        'roads/circuit.yaml')

    if not os.path.isfile(filename):
        print("Required file {} not found."
              " Please, make sure to install the latest delphyne-gui."
              .format(os.path.abspath(filename)))
        quit()

    simulation_tree = delphyne.trees.BehaviourTree(
        root=create_gazoo_scenario_subtree(filename, mobil_cars_num)
    )

    sim_runner_time_step=0.015
    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name,
        time_step=sim_runner_time_step
    )

    tree_time_step = 0.03
    with launch_interactive_simulation(
        simulation_tree.runner, bare=args.bare
    ) as launcher:
        if args.duration < 0:
            # run indefinitely
            print("Running simulation indefinitely.")
            simulation_tree.tick_tock(period=tree_time_step)
        else:
            # run for a finite time
            print("Running simulation for {0} seconds.".format(args.duration))
            simulation_tree.tick_tock(
                period=tree_time_step, number_of_iterations=int(args.duration / tree_time_step)
            )
        launcher.terminate()
