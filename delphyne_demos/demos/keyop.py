#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#

"""
A keyop demo.
"""
##############################################################################
# Imports
##############################################################################

from select import select

import atexit
import sys
import termios
import time

import delphyne.trees
import delphyne.behaviours

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


class KeyboardHandler(object):
    """A keyboard-interrupt poller. Allows users to read a keyboard
    input with a non-locking behavior making use of the select function,
    available on most *nix systems.

    This class is based on the work done by Frank Deng, available on GitHub
    as part of a set of python tools released under the MIT licence:
    https://github.com/frank-deng/experimental-works/blob/master/kbhit.py .
    """

    def __init__(self, input_stream=None):
        if input_stream:
            self.input_stream = input_stream
        else:
            self.input_stream = sys.stdin
        # Save current terminal settings
        self.file_descriptor = self.input_stream.fileno()
        self.new_terminal = termios.tcgetattr(self.file_descriptor)
        self.old_terminal = termios.tcgetattr(self.file_descriptor)
        # New terminal setting unbuffered
        self.new_terminal[3] = (self.new_terminal[3] &
                                ~termios.ICANON & ~termios.ECHO)
        termios.tcsetattr(self.file_descriptor,
                          termios.TCSAFLUSH, self.new_terminal)
        # Support normal-terminal reset at exit
        atexit.register(self.set_normal_term)

    def set_normal_term(self):
        """Resets to default terminal settings."""
        termios.tcsetattr(self.file_descriptor,
                          termios.TCSAFLUSH, self.old_terminal)

    def get_character(self):
        """Reads a character from the keyboard."""
        char = self.input_stream.read(1)
        if char == '\x00' or ord(char) >= 0xA1:
            return char + self.input_stream.read(1)
        return char

    def key_hit(self):
        """Returns True if a keyboard key has been pressed, False otherwise."""
        key_hit, _, _ = select([self.input_stream], [], [], 0)
        return key_hit != []


def demo_callback(behaviour_tree, keyboard_handler, time_step_seconds):
    """Callback function invoqued by the SimulationRunner
    at every time step.
    """
    if keyboard_handler.key_hit():
        key = keyboard_handler.get_character().lower()
        if key == 'p':
            if behaviour_tree.runner.is_simulation_paused():
                behaviour_tree.runner.unpause_simulation()
                print("Simulation is now running.")
            else:
                behaviour_tree.runner.pause_simulation()
                print("Simulation is now paused.")
        elif key == 'q':
            behaviour_tree.interrupt()
            print("Quitting simulation.")
        elif key == 's':
            if behaviour_tree.runner.is_simulation_paused():
                behaviour_tree.runner.request_simulation_step_execution(1)
                print("Simulation step of {0}s executed.".
                      format(time_step_seconds))
            else:
                print("Simulation step only supported in paused mode.")


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "Keyboard Teleoperation & Time Manipulation",
        """
This example shows how to use the keyboard events to control the
advance of a simulation. The simulation will open the usual simple
car in the center of the scene, which can be driven using the
keyboard on the GUI's teleop widget. In the console, we can toggle
(p) play/pause, (s) step and (q) quit the simulation.
        """
        )
    return parser.parse_args()

def create_scenario_subtree():
    scenario_subtree = delphyne.behaviours.roads.Road()
    scenario_subtree.add_child(
        delphyne.behaviours.agents.SimpleCar(
            name='0',
            speed=0.0))
    return scenario_subtree

##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    simulation_tree = delphyne.trees.BehaviourTree(
        root=create_scenario_subtree())

    sim_runner_time_step = 0.01
    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name,
        time_step=sim_runner_time_step
    )

    keyboard = KeyboardHandler()

    # We add it as a step callback because the runner gets stuck in a while loop until it's unpaused.
    # See simulation_runner.cc line 185 at delphyne's repository.
    simulation_tree.runner.add_step_callback(
        lambda: demo_callback(simulation_tree, keyboard,
                              sim_runner_time_step))

    print("\n"
          "************************************************************\n"
          "* Instructions for running the demo:                       *\n"
          "* <p> will pause the simulation if unpaused and viceversa. *\n"
          "* <s> will step the simulation once if paused.             *\n"
          "* <q> will stop the simulation and quit the demo.          *\n"
          "************************************************************\n")

    tree_time_step = 0.02
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
