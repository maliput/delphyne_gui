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
import fcntl
import lcm
import os
import select
import subprocess
import sys
import time

class TrackedProcess(object):
    """A handy wrapper for a process object that remembers its label, and adds
    the fileno() accessor for select() compatibility.
    """
    def __init__(self, label, process):
        self.label = label
        self.process = process

    def fileno(self):
        return self.process.stdout.fileno()


class Launcher(object):
    """Launch and manage a group of processes as a group. It one exits, all
    the rest are killed. The aggregate return code is that of whichever one
    exited first.

    Any process output to stdout or stderr is echoed to the stdout of this
    script, prefixed by process-specific labels.
    """
    def __init__(self):
        self.children = []  # list of TrackedProcess
        self.devnull = open('/dev/null')
        self.returncode = None  # First one to exit wins.
        self.name = os.path.basename(__file__)

    def launch(self, command, label=None, cwd=None):
        """Launch a process to be managed with the group. If no label is
        supplied, a label is synthesized from the supplied command line.
        """
        if label is None:
            label = os.path.basename(command[0])
        if not os.path.exists(command[0]):
            print "[%s] Missing file %s; available files are:" % (
                self.name, command[0])
            sys.stdout.flush()
            subprocess.call(["/usr/bin/find", "-L", "."])
            raise RuntimeError(command[0] + " not found")
        if not cwd:
            process = subprocess.Popen(
                command,
                stdin=self.devnull,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        else:
            process = subprocess.Popen(
                command,
                stdin=self.devnull,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=cwd)
        flags = fcntl.fcntl(process.stdout, fcntl.F_GETFL)
        fcntl.fcntl(process.stdout, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        self.children.append(TrackedProcess(label, process))

        # Fail-fast on infant mortality.
        time.sleep(0.05)
        self._poll()
        if self.returncode is not None:
            print process.stdout.read(),
            print "[%s] %s failed to launch" % (self.name, label)
            sys.exit(self.returncode or 1)

    def _poll(self):
        for child in self.children:
            ret = child.process.poll()
            if ret is not None:
                print "[%s] %s exited %d" % (self.name, child.label, ret)
                if self.returncode is None:
                    self.returncode = ret
        return self.returncode

    def _wait(self, duration):
        done = False
        start = time.time()
        while not done:
            rlist, _, _ = select.select(self.children, [], [], 0.1)
            if self._poll() is not None:
                done = True

            now = time.time()
            elapsed = now - start
            if elapsed > duration:
                print "[%s] %s exited via duration elapsed" % (
                    self.name, self.name)
                self.returncode = 0
                done = True

            for child in rlist:
                try:
                    lines = child.process.stdout.read().splitlines()
                except IOError:
                    lines = []

                lines = [l for l in lines if l]
                if not lines:
                    continue

                print '\n'.join(
                    ["[%s] %s" % (child.label, line) for line in lines])
                sys.stdout.flush()

    def wait(self, duration):
        """Wait for any of the managed processes to exit, for a keyboard
        interrupt from the user, or for the specified duration to expire.
        Print a message explaining which event occurred. Set the return
        code as that of the first-exiting process, or 0 for keyboard
        interrupt or timeout.
        """
        try:
            self._wait(duration)
        except KeyboardInterrupt:
            # This is considered success; we ran until the user stopped us.
            print "[%s] %s exited via keyboard interrupt" % (
                self.name, self.name)
            self.returncode = 0
        assert self.returncode is not None

    def kill(self):
        """Kill any still-running managed processes."""
        for child in self.children:
            if child.process.poll() is None:
                child.process.kill()


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
    """
    Docstring
    """
    launcher = Launcher()
    home_dir = os.environ['HOME']
    parser = argparse.ArgumentParser()
    parser.add_argument("--drake-path", action='store', dest='drake_path',
                        default=home_dir + "/drake_distro-2/", help="path to drake's directory")
    parser.add_argument("--no-drake-visualizer", action='store_false',
                        default=True, dest='drake_visualizer',
                        help="don't launch drake-visualizer")
    parser.add_argument("--no-lcm-spy", action='store_false',
                        default=True, dest='lcm_spy',
                        help="don't launch drake-visualizer")
    parser.add_argument("--no-lcm-logger", action='store_false',
                        default=True, dest='lcm_logger',
                        help="don't launch drake-visualizer")
    parser.add_argument("--duration", type=float, default=float('Inf'),
                        help="demo run duration in seconds") 
    args, tail = parser.parse_known_args()


    drake_bazel_bin_path = os.path.normpath(args.drake_path) + '/bazel-bin/'

    # drake's binaries
    demo_path = drake_bazel_bin_path + "drake/automotive/automotive_demo"
    steering_command_driver_path = drake_bazel_bin_path + "drake/automotive/steering_command_driver"
    drake_visualizer_path = drake_bazel_bin_path + "tools/drake_visualizer"
    lcm_spy_path = drake_bazel_bin_path + "drake/automotive/lcm-spy"
    lcm_logger_path = drake_bazel_bin_path + "external/lcm/lcm-logger"

    demo_args = "--num_trajectory_car=1"

    # delphyne's binaries
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
        # TODO: replace this delay
        time.sleep(1)

        if args.drake_visualizer:
            launcher.launch([drake_visualizer_path])
        wait_for_lcm_message_on_channel('DRAKE_VIEWER_STATUS')
        launcher.launch([demo_path, demo_args], cwd=args.drake_path)

        launcher.wait(args.duration)

    finally:
        launcher.kill()

if __name__ == '__main__':
    main()
