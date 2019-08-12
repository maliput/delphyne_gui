#!/usr/bin/env python3
#
# Copyright 2018 Toyota Research Institute
#
"""
The crash demo.
"""
##############################################################################
# Imports
##############################################################################

import math
import numpy as np

import delphyne.trees
import delphyne.behaviours

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def parse_arguments():
    """Argument passing and demo documentation."""
    parser = helpers.create_argument_parser(
        "Car Crash!",
        """
An example that exercises collision detection by setting up multiple cars
in collision course.
        """
    )
    return parser.parse_args()


def check_for_agent_collisions(simulation_subtree):
    """
    Pre tick handler that checks for collisions between agents in simulation.
    """
    simulation = simulation_subtree.runner.get_simulation()
    agent_collisions = simulation.get_collisions()
    if not agent_collisions:
        return
    print("Collisions have been detected!")
    for collision in agent_collisions:
        agent1, agent2 = collision.agents
        print("\n{} and {} have crashed at {}!".format(
            agent1.name(), agent2.name(), collision.location
        ))
        agent1_velocity = agent1.get_velocity()
        print("--> {} was going at {} m/s and hit {}.".format(
            agent1.name(), np.linalg.norm(agent1_velocity[3:]), agent2.name()
        ))
        agent1_pose = agent1.get_pose()
        print("    It now rests at {}.".format(agent1_pose.translation()))
        agent2_velocity = agent2.get_velocity()
        print("--> {} was going at {} m/s and hit {}.".format(
            agent2.name(), np.linalg.norm(agent2_velocity[3:]), agent1.name()
        ))
        agent2_pose = agent2.get_pose()
        print("    It now rests at {}.".format(agent2_pose.translation()))
    simulation_subtree.runner.pause_simulation()
    print("\nSimulation paused.")


def create_crash_scenario_subtree():
    scenario_subtree = delphyne.behaviours.roads.Road()

    scenario_subtree.add_children([
        delphyne.behaviours.agents.SimpleCar(
            name="racer0",
            initial_x=0.0,      # scene x-coordinate (m)
            initial_y=-50.0,    # scene y-coordinate (m)
            heading=math.pi/2,  # heading (radians)
            speed=5.0           # speed in the direction of travel (m/s)
        ),
        delphyne.behaviours.agents.SimpleCar(
            name="racer1",
            initial_x=-50.0,  # scene x-coordinate (m)
            initial_y=0.0,    # scene y-coordinate (m)
            heading=0.0,      # heading (radians)
            speed=5.1         # speed in the direction of travel (m/s)
        ),
        delphyne.behaviours.agents.SimpleCar(
            name="racer2",
            initial_x=0.0,       # scene x-coordinate (m)
            initial_y=50.0,      # scene y-coordinate (m)
            heading=-math.pi/2,  # heading (radians)
            speed=5.0            # speed in the direction of travel (m/s)
        ),
        delphyne.behaviours.agents.SimpleCar(
            name="racer3",
            initial_x=50.0,   # scene x-coordinate (m)
            initial_y=0.0,    # scene y-coordinate (m)
            heading=math.pi,  # heading (radians)
            speed=5.1         # speed in the direction of travel (m/s)
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
        root=create_crash_scenario_subtree()
    )
    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name,
    )

    # Adds a callback to check for agent collisions.
    simulation_tree.add_pre_tick_handler(check_for_agent_collisions)

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
        launcher.terminate()
