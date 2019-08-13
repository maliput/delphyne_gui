#!/usr/bin/env python3
#
# Copyright 2018 Toyota Research Institute
#
"""
The city demo.

"""
##############################################################################
# Imports
##############################################################################

import os.path
import random
import sys

import delphyne.trees
import delphyne.behaviours
import delphyne.decorators
import delphyne.blackboard.blackboard_helper as bb_helper

import delphyne_gui.utilities

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "City traffic",
        """
An example of city traffic with a variable number of railcars and
MOBIL controlled cars running in a closed-loop maliput road.
        """
    )
    parser.add_argument(
        "-n", "--num-rail-cars", default=40, type=int,
        help="The number of rails cars on scene (default: 40)."
    )
    parser.add_argument(
        "-m", "--num-mobil-cars", default=10, type=int,
        help="The number of MOBIL cars on scene (default: 10)."
    )

    return parser.parse_args()

##############################################################################
# Main
##############################################################################

def random_lane(road_geometry):
    lane_provider = bb_helper.LaneAndLocationProvider(
        distance_between_agents=6.0)
    return lane_provider.random_lane(road_geometry)

def random_position_for_mobil_car(road_geometry):
    lane_provider = bb_helper.LaneAndLocationProvider(
        distance_between_agents=6.0)
    road_index = road_geometry.ById()
    lane_id = random_lane(road_geometry)
    lane_position = lane_provider.random_position(road_geometry, lane_id)
    lane = road_index.GetLane(lane_id)
    geo_position = lane.ToGeoPosition(lane_position)
    geo_orientation = lane.GetOrientation(lane_position)
    initial_x, initial_y, _ = geo_position.xyz()
    initial_heading = geo_orientation.rpy().yaw_angle()
    return (initial_x, initial_y, initial_heading)

def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    filename = delphyne_gui.utilities.get_delphyne_gui_resource(
        "roads/little_city.yaml"
    )

    if not os.path.isfile(filename):
        print("Required file {} not found."
              " Please, make sure to install the latest delphyne-gui."
              .format(os.path.abspath(filename)))
        sys.exit()

    scenario_subtree = delphyne.behaviours.roads.Multilane(
        file_path=filename,
        name="little_city"
    )

    # Sets up all railcars.
    railcar_speed = 5.0  # (m/s)
    for n in range(args.num_rail_cars):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.RailCar(
                name='rail{}'.format(n),
                lane_id=random_lane,
                longitudinal_position=0.0,
                lateral_offset=0.0,
                speed=4.0
            )
        )

    # Sets up all MOBIL cars.
    mobilcar_speed = 4.0  # (m/s)
    for m in range(args.num_mobil_cars):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.MobilCar(
                name='mobil{}'.format(m),
                speed=mobilcar_speed,
                initial_pose=random_position_for_mobil_car
            )
        )

    simulation_tree = delphyne.trees.BehaviourTree(
        root=scenario_subtree
    )

    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name,
    )

    time_step = 0.01
    with launch_interactive_simulation(
        simulation_tree.runner, bare=args.bare
    ) as launcher:
        if args.duration < 0:
            # run indefinitely
            print("Running simulation indefinitely.")
            simulation_tree.tick_tock(period=time_step)
        else:
            # run for a finite time
            print("Running simulation for {0} seconds.".format(args.duration))
            simulation_tree.tick_tock(
                period=time_step, number_of_iterations=args.duration/time_step
            )
