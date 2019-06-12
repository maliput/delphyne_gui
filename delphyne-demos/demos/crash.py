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

from delphyne.simulation import (
    AgentSimulationBuilder,
    SimulationRunner
)
import delphyne.utilities as utilities

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


def on_agent_collision(_, agent_collisions):
    """
    Callback on collision between agents in simulation.

    :param _: Current simulation runner, unused.
    :type _: :class:`delphyne.simulation.SimulationRunner`
    :param agents_in_collision: List of agents (e.g. cars) currently
                                in collision.
    :type agents_in_collision: list[tuple[:class:`delphyne.agents.AgentBase`,
                                          :class:`delphyne.agents.AgentBase`]]
    """
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
    print("\nSimulation paused.")


##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    builder = AgentSimulationBuilder()

    utilities.add_simple_car(
        builder,
        name="racer0",
        position_x=0.0,  # scene x-coordinate (m)
        position_y=-50.0,  # scene y-coordinate (m)
        heading=math.pi/2,    # heading (radians)
        speed=5.0     # speed in the direction of travel (m/s)
    )

    utilities.add_simple_car(
        builder,
        name="racer1",
        position_x=-50.0,  # scene x-coordinate (m)
        position_y=0.0,     # scene y-coordinate (m)
        heading=0.0,    # heading (radians)
        speed=5.1     # speed in the direction of travel (m/s)
    )

    utilities.add_simple_car(
        builder,
        name="racer2",
        position_x=0.0,  # scene x-coordinate (m)
        position_y=50.0,  # scene y-coordinate (m)
        heading=-math.pi/2,    # heading (radians)
        speed=5.0     # speed in the direction of travel (m/s)
    )

    utilities.add_simple_car(
        builder,
        name="racer3",
        position_x=50.0,  # scene x-coordinate (m)
        position_y=0.0,     # scene y-coordinate (m)
        heading=math.pi,    # heading (radians)
        speed=5.1     # speed in the direction of travel (m/s)
    )

    runner = SimulationRunner(
        simulation=builder.build(),
        time_step=0.001,  # (secs)
        realtime_rate=args.realtime_rate,
        paused=args.paused,
        log=args.log,
        logfile_name=args.logfile_name)

    with launch_interactive_simulation(runner, bare=args.bare):
        # Adds a callback to check for agent collisions.
        runner.add_collision_callback(
            lambda agents_in_collision: on_agent_collision(
                runner, agents_in_collision
            )
        )
        runner.enable_collisions()
        runner.start()
