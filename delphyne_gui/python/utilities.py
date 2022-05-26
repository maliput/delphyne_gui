#!/usr/bin/env python3

# BSD 3-Clause License
#
# Copyright (c) 2022, Woven Planet. All rights reserved.
# Copyright (c) 2017-2022, Toyota Research Institute. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

##############################################################################
# Imports
##############################################################################

import os
import sys
import time

import delphyne.utilities

from . import launcher

from contextlib import contextmanager

##############################################################################
# Launchers
##############################################################################


@contextmanager
def launch_interactive_simulation(simulation_runner,
                                  layout=None,
                                  bare=False, ign_visualizer=None):
    """Defines a context manager function used to handle the execution of an
    interactive simulation. An interactive simulation launches the delphyne
    visualizer in a separate process and ends the simulation when the
    visualizer is closed."""

    launch_manager = launcher.Launcher()
    try:
        if not bare:
            launch_visualizer(launch_manager, layout,
                              ign_visualizer=ign_visualizer)
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


def launch_visualizer(launcher_manager, layout=None,
                      plugin_injection=None, plugin_name=None, bundle_path=None,
                      ign_visualizer=None):
    """
    Launches the project's visualizer with a given layout and using the
    given bundled package, if any.
    """
    if ign_visualizer is None:
        ign_visualizer = "visualizer"
    ign_visualizer_args = []
    if layout:
        layout_key = "--layout="
        ign_visualizer_args.append(layout_key + resolve_layout_path(layout))
    # Force line buffering for child process.
    ign_visualizer_args.append("--use-line-buffer=yes")
    if plugin_injection:
        ign_visualizer_args.append("--inject-plugin=" + plugin_injection)
    if plugin_name:
        ign_visualizer_args.append("--plugin-name=" + plugin_name)
    if bundle_path:
        ign_visualizer_args.append("--package=" + bundle_path)
    launcher_manager.launch([ign_visualizer] + ign_visualizer_args)


def resolve_layout_path(layout):
    layout_path = str()
    if os.path.isabs(layout):
        layout_path = layout
    else:
        layout_path = get_delphyne_gui_resource(
            os.path.join("layouts", layout)
        )
        if layout_path == "":
            # Then it could be relative to the execution folder.
            layout_path = layout
    return layout_path


def get_delphyne_gui_resource_root():
    """Return the root path where delphyne_gui resources live"""
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
