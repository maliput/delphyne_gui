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

import os
import subprocess
import sys
import time

"""
Bazel doesn't allow us to run two different "bazel run ...." commands
at the same time, so we need to have a separate program that launches
both the bridge and the mock publisher.
"""

def launch(command, stdin):

    label = os.path.basename(command)

    if not os.path.exists(command):
        print "Missing file %s; available files are:" % (command)
        sys.stdout.flush()
        subprocess.call(["/usr/bin/find", "-L", "."])
        raise RuntimeError(command + " not found")

    print("Launching %s" %(command))
    sys.stdout.flush()

    return subprocess.Popen(
        [command],
        stdin=stdin,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

def main(binaries):
    devnull = open('/dev/null')
    process_list = []
    for binary in binaries:
        process_list.append(launch(binary, devnull))
        time.sleep(0.5)

    for process in process_list:
        process.wait()

if __name__ == '__main__':
    main(sys.argv[1:])
