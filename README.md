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
    $ sudo apt-get install mercurial cmake pkg-config python ruby-ronn libprotoc-dev libprotobuf-dev protobuf-compiler uuid-dev libzmq3-dev git libogre-1.9-dev libglew-dev qtbase5-dev libicu-dev libboost-filesystem-dev libfreeimage-dev libtinyxml2-dev libgts-dev libavdevice-dev python3-vcstool mesa-utils
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
here, a few environment variables must be setup. Delphyne GIO provides a script
to do this for you; for this to work correctly, you must be in the root of your
delphyne workspace:

```
$ . src/delphyne/setup.bash
```

Next we can go on and build the components.

# Build dependencies

Delphyne depends on a number of external dependencies. To make the tools and
libraries easy to develop with, we build them from source and install them into
the workspace. Right now this is done manually, by running the following
commands:

```
$ mkdir -p build
$ pushd build
$ for igndep in ign_tools ign_math ign_common ign_msgs ign_transport ign_gui ign_rendering lcm; do mkdir -p $igndep ; pushd $igndep ; cmake ../../src/$igndep -DCMAKE_INSTALL_PREFIX=../../install ; make -j$( getconf _NPROCESSORS_ONLN ) install ; popd ; done
$ popd
```

This may take a little while to build the dependencies. At the end of the build,
a new subdirectory called `install` will be at the top level of your
delphyne workspace.

# Build drake

Drake must be built using the Bazel build tool. To build drake, do the
following:

```
$ pushd src/drake
$ bazel build //...
$ popd
```

Note that this will take a long time to compile.

# Build Delphyne back-end

The Delphyne back-end can now be built with CMake:

```
$ pushd build
$ mkdir -p delphyne
$ pushd delphyne
$ cmake ../../src/delphyne/ -DCMAKE_INSTALL_PREFIX=../../install
$ make -j$( getconf _NPROCESSORS_ONLN ) install
$ popd
```

# Build Delphyne visualizer

The Visualizer can now be built with CMake:

```
$ pushd build
$ mkdir -p delphyne-gui
$ pushd delphyne-gui
$ cmake ../../src/delphyne-gui/ -DCMAKE_INSTALL_PREFIX=../../install
$ make -j$( getconf _NPROCESSORS_ONLN ) install
```

The Visualizer will be installed in `<delphyne_ws>/install/bin`.

# Running the Delphyne visualizer

The visualizer is a new front-end visualizer for the drake simulator.

## Running the Visualizer standalone:

To run just the visualizer standalone, type:

```
visualizer
```

# Running the Delphyne back-end

Two different applications can be executed right now; the bridge
and the mock demo. The bridge is a bi-directional LCM-to-ignition-transport to
take messages from/to Drake and translate them to/from ignition-transport
messages that Delphyne understands. The mock demo runs the bridge,
the visualizer, and a "mock" backend that simulates the messages that Drake
would send.

## Running the bridge standalone

To run just the bridge, run:

```
$ duplex-ign-lcm-bridge
```

## Running the mock demo

To run the mock demo, type:

```
$ lcm-mock-robot-publisher
```

## Running the automotive-demo using CMake (experimental)

In order to run the automotive-demo, we make use of drake installed as a library.
So let's install drake by executing this commant from drake's project root:

```
$ bazel run install </path/to/delphyne_ws>/install_drake
```
Note:

- The directory `install_drake` will be created automatically by bazel.

After that, you'll be ready to compile and run the project.

Please follow the steps defined on the [build instructions](#instructions-for-building-the-visualizer-using-cmake-experimental).

When you reach this step:
```
$ cmake ../../src/delphyne/ -DCMAKE_INSTALL_PREFIX=../../install
```

You'll be able to pass a different path for the DRAKE_INSTALL_PREFIX by appending `DDRAKE_INSTALL_PREFIX=<path/to/drake-install>` to it.

After the build finishes, open three terminal emulators and execute one of the following commands on each of them, in the following order:

```
$ cd <path/to/delphyne_ws>/install/bin && ./duplex-ign-lcm-bridge
```

```
$ cd <path/to/delphyne_ws>/install/bin && ./visualizer
```

```
$ cd <path/to/delphyne_ws>/install/bin && ./automotive-demo
```

By running those commands, you should be able to see a single prius car standing in the middle of an empty world.


# Instructions for the clang-format tool
In order to get all the C++ code in the project compliant with a single style, we strongly recommend you using the auto-formatting tool called clang-format.
