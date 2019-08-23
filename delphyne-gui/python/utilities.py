#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute


##############################################################################
# Imports
##############################################################################

import os
import time

import delphyne.utilities

from . import launcher

from contextlib import contextmanager

##############################################################################
# Launchers
##############################################################################


@contextmanager
def launch_interactive_simulation(simulation_runner,
                                  layout="layout_with_teleop.config",
                                  bare=False):
    """Defines a context manager function used to hande the execution of an
    interactive simulation. An interactive simulation launches the delphyne
    visualizer in a separate process and ends the simulation when the
    visualizer is closed."""

    launch_manager = launcher.Launcher()
    try:
        if not bare:
            launch_visualizer(launch_manager, layout)
        yield launch_manager
        launch_manager.wait(float("Inf"))
    except RuntimeError as error_msg:
        sys.stderr.write("ERROR: {}".format(error_msg))
    finally:
        if simulation_runner.is_logging():
            print("Simulation has been logged in {}".format(
                simulation_runner.get_log_filename()))
            simulation_runner.stop_logging()
        if simulation_runner.is_interactive_loop_running():
            simulation_runner.stop()
        print("Simulation ended. I'm happy, you should be too.")
        print_simulation_stats(simulation_runner)
        # This is needed to avoid a possible deadlock. See
        # SimulatorRunner class description.
        time.sleep(0.5)
        launch_manager.kill()


def launch_visualizer(launcher_manager, layout_filename=None,
                      plugin_injection=None, bundle_path=None):
    """
    Launches the project's visualizer with a given layout and using the
    given bundled package, if any.
    """
    ign_visualizer = "visualizer"
    ign_visualizer_args = []
    if layout_filename:
        layout_key = "--layout="
        layout_path = get_delphyne_gui_resource(
            os.path.join("layouts", layout_filename)
        )
        ign_visualizer_args.append(layout_key + layout_path)
    # Force line buffering for child process.
    ign_visualizer_args.append("--use-line-buffer=yes")
    if plugin_injection:
        ign_visualizer_args.append("--inject-plugin=" + plugin_injection)
    if bundle_path:
        ign_visualizer_args.append("--package=" + bundle_path)
    launcher_manager.launch([ign_visualizer] + ign_visualizer_args)


def get_delphyne_gui_resource_root():
    """Return the root path where delphyne-gui resources live"""
    return delphyne.utilities.get_from_env_or_fail('DELPHYNE_GUI_RESOURCE_ROOT')


def get_delphyne_gui_resource(path):
    """Resolve the path against delphyne resources root location."""
    for root in get_delphyne_gui_resource_root().split(':'):
        resolved_path = os.path.join(root, path)
        if os.path.exists(resolved_path):
            return resolved_path
    return ''


def print_simulation_stats(simulation_runner):
    """Get the interactive simulation statistics and print them on standard
    output.
    """
    stats = simulation_runner.get_stats()
    print("= Simulation stats ==========================")
    print("  Simulation runs: {}".format(stats.total_runs()))
    print("  Simulation steps: {}".format(stats.total_executed_steps()))
    print("  Elapsed simulation time: {}s".format(
        stats.total_elapsed_simtime()))
    print("  Elapsed real time: {}s".format(stats.total_elapsed_realtime()))
    print("=============================================")
