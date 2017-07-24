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
