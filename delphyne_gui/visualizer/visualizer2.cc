// Copyright 2021 Toyota Research Institute

#include <iostream>
#include <string>

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
static const char versionStr[] = "Visualizer 0.2.0";
static const std::string initialConfigFile =
    ignition::common::joinPaths(DELPHYNE_INITIAL_CONFIG_PATH, "layout2_with_teleop.config");

/////////////////////////////////////////////////
int Main(int argc, char** argv) {
  ignition::common::Console::SetVerbosity(3);
  ignmsg << versionStr << std::endl;

  if (argc > 1) {
    delphyne::gui::GlobalAttributes::ParseArguments(argc - 1, &(argv[1]));
  }

  // If we run the visualizer as a child process (like a demo written in
  // python), we need to ensure that it's not using a block buffer
  // to display everything that goes to the stdout in realtime.
  if (delphyne::gui::GlobalAttributes::HasArgument("use-line-buffer")) {
    std::string use_line_buffer_arg = delphyne::gui::GlobalAttributes::GetArgument("use-line-buffer");
    if (use_line_buffer_arg == "yes") {
      setlinebuf(stdout);
    }
  }

  // Create app
  ignition::gui::Application app(argc, argv);

  // Load plugins / config
  if (!app.LoadConfig(initialConfigFile)) {
    return 1;
  }

  // Customize main window
  auto win = app.findChild<ignition::gui::MainWindow*>()->QuickWindow();
  win->setProperty("title", "Hello Window!");

  // Run window
  app.exec();

  return 0;
}

}  // namespace
}  // namespace visualizer
}  // namespace delphyne_gui

int main(int argc, char** argv) { return delphyne_gui::visualizer::Main(argc, argv); }
