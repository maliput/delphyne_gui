## visualizer

The `visualizer` application launchs a visualization window based on ignition-gui,
 that is intended to be used for the visual part of simulations or simply to visualize different static scenes.

### Configuration files

The visualizer can be executed using different configuration files.
You can select a different layout by passing `--layout` flag to the execution
```sh
visualizer --layout=<path-to-config>
```



### Gui-Plugins

The gui-plugins that can be attached to the visualizer are available from three different sources.
1. - **Built-in plugins**: They come with `ign-gui` package.

2. - **Delphyne-gui's custom plugins**: All gui-plugins created in `delphyne_gui` are installed where the [` PLUGIN_INSTALL_PATH`](https://github.com/ToyotaResearchInstitute/delphyne_gui/blob/main/delphyne_gui/cmake/config.hh.in) defined variable indicates: at `${CMAKE_INSTALL_PREFIX}/lib/gui_plugins`.

    - To avoid mistakenly try to load libraries (`.so`) that aren't meant to be loaded as gui-plugins, a subfolder `gui_plugins` was added to contain only gui-plugins. The rest of the `.so` are expected to be located at `${CMAKE_INSTALL_PREFIX}/lib` as usual. See [Issue#460](https://github.com/ToyotaResearchInstitute/delphyne_gui/issue/460) and [PR#461](https://github.com/ToyotaResearchInstitute/delphyne_gui/pull/461) for further information.

3. - **User's custom plugins**: Any external path that contains gui-plugins can be added to the discovery process. To do so, the `VISUALIZER_PLUGIN_PATH` environment variable should set before executing the `visualizer`.
    ```
      export VISUALIZER_PLUGIN_PATH=<new_path_containing_gui_plugins>
    ```
