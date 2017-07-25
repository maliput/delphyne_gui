# Drakaina

This is the repository for Drakaina.  As of right now, the only supported platform is Ubuntu 16.04 amd64.

# Setup instructions

1.  You first need to install Bazel.  The instructions are [here](https://docs.bazel.build/versions/master/install-ubuntu.html), but in brief:

```
$ sudo apt-get install openjdk-8-jdk
$ echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
$ curl https://bazel.build/bazel-release.pub.gpg | sudo apt-key add -
$ sudo apt-get update
$ sudo apt-get install bazel
```

1. Next you need to add the gazebo repositories to your setup.  The canonical instructions are [here](https://ignition-transport.readthedocs.io/en/latest/installation/installation.html#ubuntu-linux), in brief:

```
$ sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
$ wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
$ sudo apt-get update
```

1. Now you can install additional dependencies

```
$ sudo apt-get install mercurial cmake pkg-config python ruby-ronn libprotoc-dev libprotobuf-dev protobuf-compiler uuid-dev libzmq3-dev git libignition-transport3-dev libogre-1.9-dev libglew-dev libignition-common-dev qtbase5-dev libicu-dev
```

# Repository structure
There are 2 things hosted here right now; the bridge from LCM messages to ignition-transport messages, and the new drake visualizer.  The bridge is stored in the bridge/ subdirectory, and the visualizer
is stored in the visualizer/ subdirectory.

# Building
To build the bridge, run:

```
$ bazel build //bridge:*
```

To build the visualizer, run:

```
$ bazel build //visualizer:*
```
