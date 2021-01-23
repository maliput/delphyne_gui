#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#
# WTF(daniel.stonier) this demo would be easier if there were
# agents on a road moving from the start (currently have to teleop it).
"""
Manipulating the rate of time at startup and in runtime.
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


class RealtimeRateChanger(object):

    """Simple class that hooks to the simulation callback and dynamically
    changes the real-time rate"""

    def __init__(self, initial_steps):
        self._steps = initial_steps

    def tick(self, behaviour_tree):
        """Process simulation step"""
        if self._steps == 0:
            last_round_realtime_rate = behaviour_tree.runner.get_stats(
            ).get_current_realtime_rate()
            rate = behaviour_tree.runner.get_realtime_rate() + 0.2
            if rate >= 1.6:
                rate = 0.6
            self._steps = int(rate * 400)
            behaviour_tree.runner.set_realtime_rate(rate)
            print("Running at real-time rate {0} for {1} steps."
                  " Last real-time measure was {2}"
                  .format(rate, self._steps, last_round_realtime_rate))
        else:
            self._steps -= 1


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "Realtime Rate Changer",
        """
This example shows how the real-time simulation rate can be set both when the
simulator runner is created and while the simulation is running.

To pass an initial real-time rate use the `--realtime_rate` flag, like:

$ {0} --realtime_rate=2.0

If none is specified the default will be set to `1.0` (i.e. run the simulation
in real-time).

Once the scripts starts running it will cycle between a real-time rate of `0.6`
to `1.6` to depict how dynamic real-time rate impacts on the simulation.
        """.format(os.path.basename(sys.argv[0])))
    return parser.parse_args()

def create_realtime_scenario_subtree():
    scenario_subtree = delphyne.behaviours.roads.Road()
    scenario_subtree.add_child(
        delphyne.behaviours.agents.SimpleCar(name=str(0), speed=0.0))
    return scenario_subtree

##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    # Read the initial real-time rate from command line. Default to 1.0 if none
    # specified.
    args = parse_arguments()

    # Since this is the first time the simulator runs we compensate for the
    # startup time by running it 2 times longer than the dynamically changing
    # loop.
    initial_steps = int(args.realtime_rate * 800)
    rate_changer = RealtimeRateChanger(initial_steps)

    simulation_tree = delphyne.trees.BehaviourTree(
        root=create_realtime_scenario_subtree())

    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name
    )

    simulation_tree.add_post_tick_handler(rate_changer.tick)

    print("Running at real-time rate {0} for {1} steps"
          .format(simulation_tree.runner.get_realtime_rate(), initial_steps))

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
