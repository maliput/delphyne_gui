#!/usr/bin/env python3

# BSD 3-Clause License
#
# Copyright (c) 2022, Woven Planet. All rights reserved.
# Copyright (c) 2018-2022, Toyota Research Institute. All rights reserved.
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

"""
Replays a simulation out of a log file previously obtained.
The layout used for the window is `layout_for_playback` which
posses a playback plugin for rewinding, pausing and stepping forward the
simulation.

-- How to use:
1 - To use this application a log file is needed.
    (e.g.: `delphyne_demos` package allows you to get a log file when running its demos)
    "delphyne_gazoo -l -f <path/to/somewhere/my_awesome_log_file.log>"

2 - Execute the application using the log as argument:
    "delphyne_replay <path/to/somewhere/my_awesome_log_file.log>"

"""

##############################################################################
# Imports
##############################################################################

import argparse
import contextlib
import os
import shutil
import sys
import tempfile
import time
import zipfile

import delphyne.cmdline
import delphyne_gui.utilities


##############################################################################
# Supporting Classes & Methods
##############################################################################


TOPIC_LOG_FILENAME = 'topic.db'
# Directory names in zip files have
# a trailing slash.
BUNDLE_DIRNAME = 'bundle/'


def is_log_file(path):
    """
    Checks whether the provided `path` points to a valid
    Delphyne log file or not.
    """
    if not path or not zipfile.is_zipfile(path):
        return False
    with zipfile.ZipFile(path) as archive:
        members = archive.namelist()
        return (TOPIC_LOG_FILENAME in members
                and BUNDLE_DIRNAME in members)


@contextlib.contextmanager
def open_log_file(path):
    """
    Extracts the Delphyne log file at `path` at a temporary
    location and returns, in order of appearance, ignition
    transport topic logs and Delphyne bundled package as a
    a tuple.

    Upon context exit, extracted content is dropped.
    """
    with zipfile.ZipFile(path) as archive:
        tempdir = tempfile.mkdtemp()
        try:
            archive.extractall(tempdir)
            yield (os.path.join(tempdir, TOPIC_LOG_FILENAME),
                   os.path.join(tempdir, BUNDLE_DIRNAME))
        finally:
            shutil.rmtree(tempdir)


def parse_arguments():
    """Argument passing and demo documentation."""
    parser = argparse.ArgumentParser(
        description=delphyne.cmdline.create_argparse_description(
            "Simulation replaying tool",
            "A tool to replay a simulation from a log file."
        ),
        epilog=delphyne.cmdline.create_argparse_epilog(),
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('log_file', nargs='?',
                        help="Path to simulation log file.")
    parser.add_argument('-l', '--latest-log-file', action='store_true',
                        help="Use latest simulation log file.")
    default_search_paths = ['.']
    if 'HOME' in os.environ:
        default_search_paths.append(
            os.path.join(os.environ['HOME'], '.delphyne', 'logs')
        )
    parser.add_argument('-p', '--search-path', action='append',
                        default=default_search_paths,
                        help=("Search paths to look for latest log file"
                              " (default: '.' and $HOME/.delphyne/logs)."))
    parser.add_argument(
        '-b', '--bare', action='store_true', default=False,
        help="If true, replay without visualization (default: False)."
    )
    return parser.parse_args()


##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    if not args.log_file and args.latest_log_file:
        search_paths = [
            path for path in args.search_path if os.path.isdir(path)
        ]
        found_paths = [
            os.path.join(root_path, relative_path)
            for root_path in search_paths
            for relative_path in os.listdir(root_path)
        ]
        found_files = [path for path in found_paths if os.path.isfile(path)]
        args.log_file = next(filter(is_log_file, sorted(
            found_files, key=os.path.getmtime, reverse=True
        )), None)

    if not is_log_file(args.log_file):
        print("No valid log file to replay.")
        quit()
    print("Replaying logs at {}.".format(args.log_file))

    with open_log_file(args.log_file) as (topic_log_path, bundle_path):
        launch_manager = delphyne_gui.launcher.Launcher()
        try:
            replayer = "delphyne_replayer"
            launch_manager.launch([replayer, topic_log_path])
            if not args.bare:
                delphyne_gui.utilities.launch_visualizer(
                    launch_manager, bundle_path=bundle_path,
                    ign_visualizer="visualizer",
                    layout="layout_for_playback.config",
                )
                launch_manager.wait(float("Inf"))
        except RuntimeError as error_msg:
            sys.stderr.write("ERROR: {}".format(error_msg))
        finally:
            # This is needed to avoid a possible deadlock.
            # See SimulatorRunner class description.
            time.sleep(0.5)
            launch_manager.kill()
