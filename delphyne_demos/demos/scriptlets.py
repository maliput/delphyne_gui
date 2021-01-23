#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#

"""This is a minimal example of starting an automotive simulation using a
python binding to the C++ `SimulationRunner` class.

Note that this is not a configurable demo, it will just create a sample
simulation with a prius car that can be driven around and road tests
a couple of python scriptlets (callbacks) in the simulator's tick-tock.

Check the other examples in this directory for more advanced uses.
"""
##############################################################################
# Imports
##############################################################################

import os.path

import random
import time

import delphyne.trees
import delphyne.behaviours
import delphyne.blackboard
import py_trees.behaviour
import py_trees.blackboard
import py_trees.common
import py_trees.decorators

import delphyne_gui.utilities

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "Scriptlets",
        """
This is a minimal example demonstrating the inclusion of a python
callback (scriptlet) to be triggered at each tick of the simulation.
        """
    )
    return parser.parse_args()


class DelayedChangeSpeed(py_trees.behaviour.Behaviour):
    """
    Change speed of agent after 10 seconds have passed in simulation.
    """

    def __init__(self, agent_name, speed=1.0,
                 name=py_trees.common.Name.AUTO_GENERATED):
        super().__init__(name)
        self.speed = speed
        self.agent_name = agent_name

    def initialise(self):
        self.status = py_trees.common.Status.RUNNING

    def late_setup(self, simulation):
        self.simulation = simulation
        self.agent = simulation.get_agent_by_name(self.agent_name)

    def update(self):
        if self.simulation.get_current_time() >= 10.0:
            print("Speed up!")
            self.agent.set_speed(self.speed)
            self.status = py_trees.common.Status.SUCCESS
        return self.status


class SimulationStats(object):
    """
    This is a simple class to keep statistics of the simulation, just
    averaging the time it takes to execute a simulation step from the outside
    world. Every 1000 measures, the values are printed to stdout.
    """

    def __init__(self):
        """Just init the stats"""
        self.reset()
        self._current_start_time = None

    def reset(self):
        """Clear all values"""
        self._time_sum = 0.0
        self._samples_count = 0
        self._current_start_time = None

    def print_stats(self):
        """Print the stats"""
        print(
            "Average simulation step takes {delta}ms"
            .format(delta=(self._time_sum / self._samples_count) * 1000))

    def start(self):
        """Record the time when we start measuring"""
        self._current_start_time = time.time()

    def record_tick(self):
        """A simulation tick happened. Record it.
        Every 1000 ticks print the stats and reset
        """
        end_time = time.time()
        delta = end_time - self._current_start_time
        self._time_sum += delta
        self._samples_count += 1
        if self._samples_count == 1000:
            self.print_stats()
            self.reset()
        self.start()

    def pos_tick_handler(self, behaviour_tree):
        self.record_tick()


def random_print(behaviour_tree):
    """
    Print a message at random, roughly every 500 calls.
    behaviour_tree parameter is necessary to add it as a pos/pre tick handler
    """
    if random.randint(1, 500) == 1:
        print("One in five hundred")


def create_scriptlets_scenario_subtree():
    file_path = delphyne_gui.utilities.get_delphyne_gui_resource(
        'roads/circuit.yaml'
    )

    if not os.path.isfile(file_path):
        print("Required file {} not found."
              " Please, make sure to install the latest delphyne-gui."
              .format(os.path.abspath(file_path)))
        quit()

    scenario_subtree = delphyne.behaviours.roads.Multilane(
        file_path=file_path, name="circuit"
    )

    scenario_subtree.add_children([
        delphyne.behaviours.agents.SimpleCar(name='simple0'),
        delphyne.behaviours.agents.RailCar(
            name='rail0',
            lane_id='l:s1_1',
            longitudinal_position=0.0,
            lateral_offset=0.0,
            speed=4.0
        ),
        py_trees.decorators.OneShot(
            child=DelayedChangeSpeed(agent_name='rail0', speed=20.0),
            policy=py_trees.common.OneShotPolicy.ON_COMPLETION
        ),
    ])

    return scenario_subtree


##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    simulation_tree = delphyne.trees.BehaviourTree(
        root=create_scriptlets_scenario_subtree()
    )

    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name
    )

    stats = SimulationStats()
    simulation_tree.add_pre_tick_handler(random_print)
    simulation_tree.add_post_tick_handler(stats.pos_tick_handler)

    tree_time_step = 0.02
    with delphyne_gui.utilities.launch_interactive_simulation(
        simulation_tree.runner, bare=args.bare, ign_visualizer="visualizer0"
    ) as launcher:
        if args.duration < 0:
            # run indefinitely
            print("Running simulation indefinitely.")
            stats.start()
            simulation_tree.tick_tock(period=tree_time_step)
        else:
            # run for a finite time
            print("Running simulation for {0} seconds.".format(args.duration))
            stats.start()
            simulation_tree.tick_tock(
                period=tree_time_step, number_of_iterations=int(args.duration / tree_time_step)
            )
        launcher.terminate()
