// Copyright 2021 Toyota Research Institute

#include <iostream>
#include <string>

#include <delphyne/utility/package.h>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>

#ifndef Q_MOC_RUN
#include <ignition/gui/Application.hh>
#include <ignition/gui/MainWindow.hh>
#include <ignition/gui/qt.h>
#endif

#include "delphyne_gui/config.hh"
#include "global_attributes.hh"

namespace delphyne_gui {
namespace visualizer {
namespace {

/// Constants.
constexpr char kVersionStr[] = "Visualizer 0.2.0";
constexpr char kDefaultLayout[] = "layout2_with_teleop.config";

/////////////////////////////////////////////////
/// \brief Get the path of the default configuration file for Delphyne.
/// \return The default configuration path.
std::string defaultConfigPath() {
  std::string homePath;
  ignition::common::env("HOME", homePath);
  return ignition::common::joinPaths(homePath, ".delphyne", "delphyne.config");
}

/////////////////////////////////////////////////
int Main(int argc, char** argv) {
  static const std::string initialConfigFile =
      ignition::common::joinPaths(DELPHYNE_INITIAL_CONFIG_PATH, kDefaultLayout);

  ignition::common::Console::SetVerbosity(3);
  ignmsg << kVersionStr << std::endl;

  if (argc > 1) {
    delphyne::gui::GlobalAttributes::ParseArguments(argc - 1, &(argv[1]));
  }

  // If we run the visualizer as a child process (like a demo written in
  // python), we need to ensure that it's not using a block buffer
  // to display everything that goes to the stdout in realtime.
  if (delphyne::gui::GlobalAttributes::HasArgument("use-line-buffer")) {
    const std::string use_line_buffer_arg = delphyne::gui::GlobalAttributes::GetArgument("use-line-buffer");
    if (use_line_buffer_arg == "yes") {
      setlinebuf(stdout);
    }
  }

  // Parse custom config file from args.
  delphyne::utility::PackageManager* package_manager = delphyne::utility::PackageManager::Instance();
  if (delphyne::gui::GlobalAttributes::HasArgument("package")) {
    package_manager->Use(
        std::make_unique<delphyne::utility::BundledPackage>(delphyne::gui::GlobalAttributes::GetArgument("package")));
  } else {
    package_manager->Use(std::make_unique<delphyne::utility::SystemPackage>());
  }

  // Initialize app
  ignition::gui::Application app(argc, argv);

  // Set the default location for saving user settings.
  app.SetDefaultConfigPath(defaultConfigPath());

  // Look for all plugins in the same place
  app.SetPluginPathEnv("VISUALIZER_PLUGIN_PATH");

  // Then look for plugins on compile-time defined path.
  // Plugins installed by gazebo end up here
  app.AddPluginPath(PLUGIN_INSTALL_PATH);

  // Attempt to load window layout from parsed arguments.
  bool layout_loaded = delphyne::gui::GlobalAttributes::HasArgument("layout") &&
                       app.LoadConfig(delphyne::gui::GlobalAttributes::GetArgument("layout"));
  // If no layout was found, attempt to use the default config file.
  layout_loaded = layout_loaded || app.LoadDefaultConfig();
  // If that's not available either, load it from initial config file.
  layout_loaded = layout_loaded || app.LoadConfig(initialConfigFile);
  // If no layout has been loaded so far, exit the application.
  if (!layout_loaded) {
    ignerr << "Unable to load a configuration file, exiting." << std::endl;
    return 1;
  }

  // Create main window
  // this is a placeholder comment to ease comparisons with visualizer0.cc

  auto win = app.findChild<ignition::gui::MainWindow*>()->QuickWindow();
  win->setProperty("title", kVersionStr);

  // Run window
  app.exec();

  return 0;
}

}  // namespace
}  // namespace visualizer
}  // namespace delphyne_gui

int main(int argc, char** argv) { return delphyne_gui::visualizer::Main(argc, argv); }
