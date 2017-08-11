import os
import sys
import time
import subprocess

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

def main():
    bridge_path = "bridge/lcm-to-ign-transport-bridge"
    mock_publisher_path = "bridge/lcm-mock-robot-publisher"
    devnull = open('/dev/null')
    p1 = launch(bridge_path, devnull)
    time.sleep(0.5)
    P2 = launch(mock_publisher_path, devnull)

    p1.wait()
    p2.wait()

if __name__ == '__main__':
    main()
