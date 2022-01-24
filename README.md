| GCC | Sanitizers(Clang) | Scan-Build |
| --------- | --------- | -------- |
|[![gcc](https://github.com/ToyotaResearchInstitute/delphyne_gui/actions/workflows/build.yml/badge.svg)](https://github.com/ToyotaResearchInstitute/delphyne_gui/actions/workflows/build.yml) | [![clang](https://github.com/ToyotaResearchInstitute/delphyne_gui/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/ToyotaResearchInstitute/delphyne_gui/actions/workflows/sanitizers.yml) | [![scan_build](https://github.com/ToyotaResearchInstitute/delphyne_gui/actions/workflows/scan_build.yml/badge.svg)](https://github.com/ToyotaResearchInstitute/delphyne_gui/actions/workflows/scan_build.yml) |

# delphyne_gui

This is the repository for Delphyne GUI, a front-end visualizer for `delphyne`.

## Build

1. Setup a development workspace as described [here](https://github.com/ToyotaResearchInstitute/maliput_documentation/blob/main/docs/installation_quickstart.rst).

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

## Visualizer
See [visualizer/README.md](delphyne_gui/visualizer/README.md)

## Exported variables

Exported variables can be found at [delphyne_gui/setup.sh.in](delphyne_gui/setup.sh.in):
 - `DELPHYNE_GUI_RESOURCE_ROOT`: Contains the path of the installed delphyne_gui's resources.
