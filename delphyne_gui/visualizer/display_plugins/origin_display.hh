// Copyright 2021 Toyota Research Institute
#pragma once

#include <string>

#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/RenderTypes.hh>

namespace delphyne {
namespace gui {

/// @brief Implements a plugin to display the Inertial Frame axes in the scene.
/// @details ign-gui3 does not have DisplayPlugins, so this plugin implements
///          implements a slightly different logic to what the original ign-gui0
///          DisplayPlugin did. It gets the scene name from the plugin
///          configuration and tries to get a pointer to it. If the scene is not
///          available, then it starts a timer and tries periodically to
///          retrieve the scene.
///          Typically, this plugin goes hand in hand with the Scene3D plugin.
class OriginDisplay : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  OriginDisplay() = default;

  /// @brief Loads the plugin configuration and tries to load the axes in the
  ///        scene.
  /// @details When the scene is not available, a timer is started to try every
  ///          `kTimerPeriodInMs` ms to load the axes.
  ///          It only works with ogre rendering engine.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

 protected:
  /// @brief Timer event callback which handles the logic to draw the axes when
  ///        the scene is not ready yet.
  void timerEvent(QTimerEvent* _event) override;

 private:
  /// @brief The period in milliseconds of the timer to try to draw the axes.
  static constexpr int kTimerPeriodInMs{500};

  /// @brief The rendering engine name.
  const std::string kEngineName{"ogre"};

  /// @brief Draws the axes in the scene.
  /// @param scene The scene to draw the axes into.
  void DrawAxes(ignition::rendering::ScenePtr scene) const;

  /// @brief Triggers an event every `kTimerPeriodInMs` to try to draw the axes.
  QBasicTimer timer;

  /// @brief The scene name.
  std::string sceneName{"scene"};
};

}  // namespace gui
}  // namespace delphyne
