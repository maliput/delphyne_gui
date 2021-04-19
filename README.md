# delphyne_gui

This is the repository for Delphyne GUI, a front-end visualizer for `delphyne`.

## Build

1. Setup a development workspace as described [here](https://github.com/ToyotaResearchInstitute/maliput-documentation/blob/main/docs/installation_quickstart.rst).

2. Bring up your development workspace:

```sh
cd path/to/my/workspace
source ./bringup
```

3. Build Delphyne packages and their dependencies:

  - If not building drake from source:

   ```sh
   colcon build --packages-up-to delphyne_gui
   ```

  - If building drake from source:

   ```sh
   colcon build --cmake-args -DWITH_PYTHON_VERSION=3 --packages-up-to delphyne_gui
   ```

## Tools

An automated script that looks for all C++ source files and calls `clang-format` accordingly:

```sh
./tools/reformat_code.sh
```

This script must be run from the top-level of the repository in order to find all of the files.
It is recommended to run this before opening any pull request.
