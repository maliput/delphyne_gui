# Delphyne-gui

This is the repository for Delphyne GUI, a new front-end visualizer for the
Drake simulator. As of right now, the only supported platform is
Ubuntu 16.04 amd64. These instructions contain information for building the
Delphyne back-end and the front-end.

# Setup instructions

1.  Setup and install dependencies:

    ```
    $ sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
    $ sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116
    $ sudo apt-get update
    $ sudo apt-get install mercurial cmake pkg-config python ruby-ronn libprotoc-dev libprotobuf-dev protobuf-compiler uuid-dev libzmq3-dev git libogre-1.9-dev libglew-dev qtbase5-dev libicu-dev libboost-filesystem-dev libfreeimage-dev libtinyxml2-dev libgts-dev libavdevice-dev python3-vcstool mesa-utils lcov gcovr libqt5multimedia5 libqwt-dev
    ```

1.  Now build a workspace for Delphyne. If you are familiar with ROS catkin
workspaces, this is a similar concept. The steps to setup the workspace are:

    ```
    $ mkdir -p delphyne_ws/src
    $ cd delphyne_ws
    ```

1.  Download the delphyne.repos file from [delphyne.repos](https://github.com/ToyotaResearchInstitute/delphyne-gui/blob/master/delphyne.repos) into
 `delphyne_ws` (we can't automate this because it is a private repository).

1.  Import the repositories from delphyne.repos:

    ```
    $ vcs import src < delphyne.repos
    ```

1.  Install the dependencies for Drake. While Delphyne doesn't directly depend
on Drake right now, it may in the future and it depends on many similar
components. The instructions are [here](http://drake.mit.edu/from_source.html),
but in brief:

    ```
    $ sudo ./src/drake/setup/ubuntu/16.04/install_prereqs.sh
    ```

# Setup the environment
In order to successfully build and use Delphyne, Drake, or the ignition tools
here, a few environment variables must be setup. Delphyne GUI provides a script
to do this for you; for this to work correctly, your current working directory
must be somewhere within the delphyne workspace:

```
$ . src/delphyne_gui/setup.bash
```

Next we can go on and build the components.

# Build and install drake

Drake must be built using the Bazel build tool. To build drake, do the
following:

```
$ pushd src/drake
$ bazel run --compiler=gcc-5 //:install `pwd`/../../install
$ bazel build --compiler=gcc-5 //automotive:*
$ popd
```

Note that this will take a long time to compile.

# Build dependencies

Delphyne depends on a number of external dependencies. To make the tools and
libraries easy to develop with, we build them from source and install them into
the workspace. Right now this is done manually, by running the following
commands:

```
$ mkdir -p build
$ pushd build
$ mkdir -p pybind11
$ pushd pybind11
$ cmake ../../src/pybind11 -DCMAKE_INSTALL_PREFIX=../../install -DPYBIND11_TEST=False
$ make -j2 install
$ popd
$ for igndep in ign_cmake ign_tools ign_common ign_msgs ign_transport ign_rendering ign_gui ; do mkdir -p $igndep ; pushd $igndep ; cmake ../../src/$igndep -DCMAKE_INSTALL_PREFIX=../../install ; make -j$( getconf _NPROCESSORS_ONLN ) install ; popd ; done
$ popd
```

This may take a little while to build the dependencies. At the end of the build,
a new subdirectory called `install` will be at the top level of your
delphyne workspace.

# Build Delphyne back-end

The Delphyne back-end can now be built with CMake:

```
$ mkdir -p build/delphyne
$ pushd build/delphyne
$ cmake ../../src/delphyne/ -DCMAKE_INSTALL_PREFIX=../../install
$ make -j$( getconf _NPROCESSORS_ONLN ) install
$ popd
```

# Build Delphyne visualizer

The Visualizer can now be built with CMake:

```
$ mkdir -p build/delphyne_gui
$ pushd build/delphyne_gui
$ cmake ../../src/delphyne_gui/ -DCMAKE_INSTALL_PREFIX=../../install
$ make -j$( getconf _NPROCESSORS_ONLN ) install
$ popd
```

The Visualizer will be installed in `<delphyne_ws>/install/bin`.

# Running the binaries

There are a few different binaries that currently work from delphyne.

## Running the mock bridge demo

This is a demo to show off the bridging aspects of delphyne along with the new
visualizer.  The visualizer, the bidirectional lcm-to-ign-transport bridge, and
a mock" backend that publishes LCM messages are all launched.  The mock backend
continually publishes some LCM messages, which are accepted by the bridge and
translated into ign-transport messages.  These messages are then sent to the
visualizer for display.  To run this demo, do the following:

```
$ mocked_robot_demo.py
```

## Running the Visualizer standalone

The visualizer is a new front-end visualizer for the drake simulator.
To run just the visualizer standalone, run:

```
$ visualizer
```

## Running the bridge standalone

To run just the bridge, run:

```
$ duplex-ign-lcm-bridge
```

## Running the automotive-demo from Drake

In order to ease launching the demos, a `demo_launcher` command is provided inside
`<delphyne_ws>/install/bin`, along with the other generated binaries. The launcher
takes a single parameter (the demo name) and can be one of `simple`, `trajectory`
or `dragway`. To use it, just run:

```
$ demo_launcher.py --demo=dragway
```

Note that currently this launcher is using Drake's automotive demo implementation
and we are in the process of migrating this to the delphyne's backend (see next
section).

## Running the backend automotive-demo

The backend automotive demo aims to show off the new delphyne backend that does
not depend on a bridge for converting LCM messages to ign-transport messages.
Instead, the backend is linked to drake, and directly publishes ign-transport
messages for visualization.  To run the backend automotive demo, open three
terminal emulators and execute one of the following commands on each of them,
in the following order:

```
$ duplex-ign-lcm-bridge
```

```
$ visualizer
```

```
$ automotive-demo
```

By running those commands, you should be able to see a single prius car standing in the middle of an empty world.


# Instructions for the clang-format tool
In order to get all the C++ code in the project compliant with a single style, we strongly recommend you using the auto-formatting tool called clang-format.

You can execute it against your source code by doing:
```
/usr/bin/clang-format-3.9 -i -style=file <path/to/file.cpp>
```
This will automatically apply the code conventions specified in the .clang-format file, found on the root of the repository.

There is also an automated script that looks for all the C++ src/header files and then calls clang-format accordingly. You can invoque it by doing:

```
./tools/reformat_code.sh
```

This script must be run from the top-level of the repository in order to find
all of the files. It is recommended to run this before opening any pull request.
