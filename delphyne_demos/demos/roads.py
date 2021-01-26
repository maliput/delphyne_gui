#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#
"""
Load a simulation with one of a few sample maliput road networks.
"""
##############################################################################
# Imports
##############################################################################

import os
import sys

import delphyne.trees
import delphyne.behaviours

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "Maliput Roads",
        """
Load one of the various types of maliput road networks
into an empty (free of agents) simulation. For the time
being the following road network types are supported:
dragway, onramp, and multilane.

This demo uses the subcommand style, where each road
type can handle different parameters. To get help on each
road type's parameters, run for example:

$ {0} multilane --help

Some examples:

$ {0} dragway --length=200 --shoulder-width=2.5
$ {0} onramp
$ {0} multilane
--filename='./install/share/delphyne/roads/circuit.yaml'
$ {0} malidrive
--filename='./install/share/delphyne/roads/circuit.xodr'
--name=my_circuit
        """.format(os.path.basename(sys.argv[0]))
    )
    subparsers = parser.add_subparsers(dest="road_type")

    # Dragway subcommand
    dragway_parser = subparsers.add_parser("dragway")
    dragway_parser.add_argument("--lanes", default=3,
                                type=int,
                                help="the number of lanes the dragway has")
    dragway_parser.add_argument("--length", default=100.0,
                                type=float,
                                help="the length of the dragway, in meters")
    dragway_parser.add_argument("--lane-width", default=3.7,
                                type=float,
                                help="the width of each lane, in meters")
    dragway_parser.add_argument("--shoulder-width", default=1.0,
                                type=float,
                                help="the width of the road shoulder,\
                                in meters")
    dragway_parser.add_argument("--max-height", default=5.0,
                                type=float,
                                help="the maximum allowed height for the road,\
                                in meters")

    # Onramp subcommand
    subparsers.add_parser("onramp")

    # Multilane subcommand
    multilane_parser = subparsers.add_parser("multilane")
    multilane_parser.add_argument("--filename",
                                  help="multilane file path",
                                  required=True)

    # Malidrive subcommand
    malidrive_parser = subparsers.add_parser("malidrive")
    malidrive_parser.add_argument("--filename",
                                  help="malidrive file path",
                                  required=True)
    malidrive_parser.add_argument("--name",
                                  help="malidrive road name",
                                  default="maliroad")

    return parser.parse_args()

##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    if args.road_type == "dragway":
        scenario_subtree = delphyne.behaviours.roads.Dragway(
            name="Demo Dragway",
            num_lanes=args.lanes,
            length=args.length,
            lane_width=args.lane_width,
            shoulder_width=args.shoulder_width,
            maximum_height=args.max_height)
    elif args.road_type == "onramp":
        scenario_subtree = delphyne.behaviours.roads.OnRamp()
    elif args.road_type == "multilane":
        scenario_subtree = delphyne.behaviours.roads.Multilane(
            file_path=args.filename)
    elif args.road_type == "malidrive":
        scenario_subtree = delphyne.behaviours.roads.Malidrive(
            file_path=args.filename,
            name=args.name)
    else:
        print("Option {} not recognized".format(args.road_type))
        sys.exit()

    simulation_tree = delphyne.trees.BehaviourTree(
        root=scenario_subtree)

    try:
        simulation_tree.setup(
            realtime_rate=args.realtime_rate,
            start_paused=args.paused,
            logfile_name=args.logfile_name
        )
    except RuntimeError as error:
        print("An error ocurred while trying to run the simulation with : {}".format(args.filename))
        print(str(error))
        print("Exiting the simulation")
        sys.exit()

    tree_time_step = 0.02
    with launch_interactive_simulation(
        simulation_tree.runner, bare=args.bare, ign_visualizer="visualizer0"
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
