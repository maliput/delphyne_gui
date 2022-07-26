[![GCC](https://github.com/maliput/delphyne_gui/actions/workflows/build.yml/badge.svg)](https://github.com/maliput/delphyne_gui/actions/workflows/build.yml)

# delphyne_gui

## Description
`delphyne_gui` is a front-end visualizer for `delphyne`.

It also provides a road network visualizer for [`Maliput`](https://maliput.readthedocs.io/en/latest/index.html)

## API Documentation

Refer to [Delphyne GUI's Online API Documentation](https://maliput.readthedocs.io/en/latest/html/deps/delphyne_gui/html/index.html).

## Visualizer

See [visualizer page](visualizer/README.md).

## Maliput Viewer

An application based on the [visualizer](visualizer/README.md) is provided for visualizing a given Maliput's road network.

### Features

 - Supports maliput_multilane and maliput_malidrive backends.
   - A XODR or YAML file could be selected to be visualized
 - Supports loading a Traffic Light Book.
 - Supports loading a RuleRegistry Book.
 - Supports loading a Rule Book.
 - Supports loading a Phase Ring Book.
   - Phases can be switched via the interface.
 - Supports loading a Intersection Book.
 - Road examination.

### Open the Maliput Viewer

After installing `delphyne_gui`, run:

```sh
  maliput_viewer.sh
```

## Examples

[delphyne_demos](https://github.com/maliput/delphyne_demos) provides demos that rely on `delphyne_gui` for the visualization.

## Installation

### Supported platforms

Ubuntu Focal Fossa 20.04 LTS.

### Source Installation on Ubuntu

#### Prerequisites

```
sudo apt install python3-rosdep python3-colcon-common-extensions
```

#### Build

1. Create colcon workspace if you don't have one yet.
    ```sh
    mkdir colcon_ws/src -p
    ```

2. Clone dependencies in the `src` folder
    ```sh
    cd colcon_ws/src
    git clone https://github.com/maliput/drake_vendor.git
    ```
    ```
    git clone https://github.com/maliput/delphyne.git
    ```

3. Clone this repository in the `src` folder
    ```sh
    cd colcon_ws/src
    git clone https://github.com/maliput/delphyne_gui.git
    ```

4. Install package dependencies via `rosdep`
    ```
    export ROS_DISTRO=foxy
    ```
    ```sh
    rosdep update
    rosdep install -i -y --rosdistro $ROS_DISTRO --from-paths src
    ```
5. Follow instructions to install drake via [`drake_vendor`](https://github.com/maliput/drake_vendor) package.

6. Build the package
    ```sh
    colcon build --packages-up-to delphyne_gui
    ```

    **Note**: To build documentation a `-BUILD_DOCS` cmake flag is required:
    ```sh
    colcon build --packages-select delphyne_gui --cmake-args " -DBUILD_DOCS=On"
    ```
    More info at [Building Documentation](https://maliput.readthedocs.io/en/latest/developer_guidelines.html#building-the-documentation).

For further info refer to [Source Installation on Ubuntu](https://maliput.readthedocs.io/en/latest/installation.html#source-installation-on-ubuntu)

## Exported variables

Exported variables can be found at [delphyne_gui/setup.sh.in](delphyne_gui/setup.sh.in):
 - `DELPHYNE_GUI_RESOURCE_ROOT`: Contains the path of the installed delphyne_gui's resources.

### For development

It is recommended to follow the guidelines for setting up a development workspace as described [here](https://maliput.readthedocs.io/en/latest/developer_setup.html).

## Contributing

Please see [CONTRIBUTING](https://maliput.readthedocs.io/en/latest/contributing.html) page.

## License

[![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://github.com/maliput/delphyne_gui/blob/main/LICENSE)
