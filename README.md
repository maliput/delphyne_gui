# Delphyne-gui

This is the repository for Delphyne GUI, a new front-end visualizer for the
Drake simulator. As of right now, the only supported platform is
Ubuntu 16.04 amd64.

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

1.  Download the delphyne-gui.repos file from [delphyne-gui.repos](https://github.com/ToyotaResearchInstitute/delphyne-gui/blob/master/delphyne-gui.repos) into `delphyne_ws` (we can't automate this because it is a private repository).

1.  Import the repositories from delphyne-gui.repos:

    ```
    $ vcs import src < delphyne-gui.repos
    ```

# Setup the environment
In order to successfully build and use Delphyne GUI, the ignition tools here, a
few environment variables must be setup. Delphyne GIO provides a script to do
this for you; for this to work correctly, you must be in the root of your
delphyne GUI workspace:

```
$ . src/delphyne/setup.bash
```

Next we can go on and build the components.

# Build dependencies

Delphyne GUI depends on a number of external dependencies. To make the tools and
libraries easy to develop with, we build them from source and install them into
the workspace. Right now this is done manually, by running the following
commands:

```
$ mkdir -p build
$ pushd build
$ for igndep in ign_tools ign_math ign_common ign_msgs ign_transport ign_gui ign_rendering; do mkdir -p $igndep ; pushd $igndep ; cmake ../../src/$igndep -DCMAKE_INSTALL_PREFIX=../../install ; make -j$( getconf _NPROCESSORS_ONLN ) install ; popd ; done
$ popd
```

This may take a little while to build the dependencies. At the end of the build,
a new subdirectory called `install` will be at the top level of your
delphyne GUI workspace.

# Build delphyne GUI

The Visualizer can now be built with CMake:

```
$ pushd build
$ mkdir -p delphyne-gui
$ pushd delphyne-gui
$ cmake ../../src/delphyne-gui/ -DCMAKE_INSTALL_PREFIX=../../install
$ make -j$( getconf _NPROCESSORS_ONLN ) install
```

The Visualizer will be installed in `<delphyne_ws>/install/bin`.

# Running the Visualizer standalone:

To run just the visualizer standalone, type:

```
visualizer
```

## Uninstall the Visualizer

The Visualizer can be uninstalled by typing:

```
$ pushd build/delphyne-gui
$ make uninstall
```
