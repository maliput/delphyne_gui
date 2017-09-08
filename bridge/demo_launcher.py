# Copyright 2017 Open Source Robotics Foundation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import argparse
import lcm
import os
import select
import sys
import time
from mocked_robot_demo import Launcher


def wait_for_lcm_message_on_channel(channel):
    """Wait for a single message to arrive on the specified LCM channel.
    """
    m = lcm.LCM()

    def receive(channel, data):
        _ = (channel, data)
        raise StopIteration()

    sub = m.subscribe(channel, receive)
    start_time = time.time()
    try:
        while True:
            if time.time() - start_time > 10.:
                raise RuntimeError(
                    "Timeout waiting for channel %s" % channel)
            rlist, _, _ = select.select([m], [], [], 0.1)
            if m in rlist:
                m.handle()
    except StopIteration:
        pass
    finally:
        m.unsubscribe(sub)

def main():
    """Launches drake's automotive_demo along with the bridge and the
    ignition-visualizer.
    It can spawn optionally the drake-visualizer and the lcm tools.
    Terminates all the processes if one of them is killed.
    """
    launcher = Launcher()
    parser = argparse.ArgumentParser()

    # Required argument
    parser.add_argument(dest='drake_path', nargs='?', action='store',
                        help="path to drake's directory")
    # Optional arguments
    parser.add_argument("--no-drake-visualizer", action='store_false',
                        default=True, dest='drake_visualizer',
                        help="don't launch drake-visualizer")
    parser.add_argument("--no-lcm-spy", action='store_false',
                        default=True, dest='lcm_spy',
                        help="don't launch lcm-spy")
    parser.add_argument("--no-lcm-logger", action='store_false',
                        default=True, dest='lcm_logger',
                        help="don't launch lcm-logger")
    parser.add_argument("--duration", type=float, default=float('Inf'),
                        help="demo run duration in seconds") 
    args, tail = parser.parse_known_args()

    if args.drake_path is None or not os.path.exists(args.drake_path):
        print "You must specify a valid drake_path argument."
        parser.print_help()
        sys.exit(1)


    drake_bazel_bin_path = os.path.normpath(args.drake_path) + '/bazel-bin/'

    # drake's binaries path
    demo_path = drake_bazel_bin_path + "drake/automotive/automotive_demo"
    steering_command_driver_path = drake_bazel_bin_path + "drake/automotive/steering_command_driver"
    drake_visualizer_path = drake_bazel_bin_path + "tools/drake_visualizer"
    lcm_spy_path = drake_bazel_bin_path + "drake/automotive/lcm-spy"
    lcm_logger_path = drake_bazel_bin_path + "external/lcm/lcm-logger"

    # arguments for drake's automotive_demo
    demo_args = "--num_trajectory_car=1"

    # delphyne's binaries path
    lcm_ign_bridge = "bridge/lcm-ign-transport-bridge"
    ign_visualizer = "visualizer/visualizer"

    try:
        if args.lcm_spy:
            launcher.launch([lcm_spy_path])
        if args.lcm_logger:
            launcher.launch([lcm_logger_path])
        launcher.launch([steering_command_driver_path])

        launcher.launch([lcm_ign_bridge])
        launcher.launch([ign_visualizer])

        # TODO: replace this delay with a
        # feedback from the ignition visualizer
        time.sleep(1)

        if args.drake_visualizer:
            launcher.launch([drake_visualizer_path])
            # wait for the drake_visualizer to be up
            wait_for_lcm_message_on_channel('DRAKE_VIEWER_STATUS')
        launcher.launch([demo_path, demo_args], cwd=args.drake_path)

        launcher.wait(args.duration)

    finally:
        launcher.kill()

if __name__ == '__main__':
    main()
