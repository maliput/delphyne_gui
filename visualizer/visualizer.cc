// Copyright 2017 Toyota Research Institute

#include <string>

#include <delphyne/utility/package.h>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#ifndef Q_MOC_RUN
#include <ignition/gui/MainWindow.hh>
#include <ignition/gui/qt.h>
#include <ignition/gui/Iface.hh>
#endif

#include "delphyne-gui/config.hh"

#include "global_attributes.hh"

/// Constants.
static const char versionStr[] = "Visualizer 0.1.0";
static const std::string initialConfigFile = ignition::common::joinPaths(
    DELPHYNE_INITIAL_CONFIG_PATH, "layout_with_teleop.config");

/////////////////////////////////////////////////
/// \brief Get the path of the default configuration file for Delphyne.
/// \return The default configuration path.
std::string defaultConfigPath() {
  std::string homePath;
  ignition::common::env("HOME", homePath);
  std::string defaultConfigPath =
      ignition::common::joinPaths(homePath, ".delphyne", "delphyne.config");

  return defaultConfigPath;
}

/////////////////////////////////////////////////
int main(int argc, const char* argv[]) {
  ignition::common::Console::SetVerbosity(3);
  ignmsg << versionStr << std::endl;

  if (argc > 1) {
    delphyne::gui::GlobalAttributes::ParseArguments(argc - 1, &(argv[1]));
  }

  // Parse custom config file from args.
  const std::string configFile =
    delphyne::gui::GlobalAttributes::HasArgument("layout") ?
      delphyne::gui::GlobalAttributes::GetArgument("layout") :
      initialConfigFile;

  delphyne::utility::PackageManager* package_manager =
      delphyne::utility::PackageManager::Instance();
  if (delphyne::gui::GlobalAttributes::HasArgument("package")) {
    package_manager->Use(
        std::make_unique<delphyne::utility::BundledPackage>(
            delphyne::gui::GlobalAttributes::GetArgument("package")));
  } else {
    package_manager->Use(
        std::make_unique<delphyne::utility::SystemPackage>());
  }

  Q_INIT_RESOURCE(resources);

  // Initialize app
  ignition::gui::initApp();

  // Set the default location for saving user settings.
  ignition::gui::setDefaultConfigPath(defaultConfigPath());

  // Look for all plugins in the same place
  ignition::gui::setPluginPathEnv("VISUALIZER_PLUGIN_PATH");

  // Then look for plugins on compile-time defined path.
  // Plugins installed by gazebo end up here
  ignition::gui::addPluginPath(PLUGIN_INSTALL_PATH);

  // Attempt to load window layout from default config file.
  // If it's not availeble, load it from configFile.
  if (!ignition::gui::loadDefaultConfig()) {
    // If config file parsed from args is not
    // a valid path, exit the application.
    if(!ignition::gui::loadConfig(configFile)) {
      return 1;
    }
  }

  // Create main window
  ignition::gui::createMainWindow();

  auto win = ignition::gui::mainWindow();
  win->setWindowTitle(versionStr);

  ignition::gui::runMainWindow();

  ignition::gui::stop();

  return 0;
}
