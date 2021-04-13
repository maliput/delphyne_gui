// Copyright 2021 Toyota Research Institute
#pragma once

#include <array>
#include <string>

#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/Visual.hh>

namespace delphyne {
namespace gui {

/// @brief Implements a plugin to display the Inertial Frame axes in the scene.
/// @details ign-gui3 does not have DisplayPlugins, so this plugin
///          implements a slightly different logic to what the original ign-gui0
///          DisplayPlugin did. It gets the scene name from the plugin
///          configuration and tries to get a pointer to it. If the scene is not
///          available, then it starts a timer and tries periodically to
///          retrieve the scene.
///          Typically, this plugin goes hand in hand with the Scene3D plugin.
///          The plugin UI has a checkbox to toggle visibility. It is paired
///          with `isVisible`
class OriginDisplay : public ignition::gui::Plugin {
  Q_OBJECT

  Q_PROPERTY(bool isVisible READ IsVisible WRITE SetIsVisible NOTIFY IsVisibleChanged)

 public:
  OriginDisplay() = default;

  /// @brief Loads the plugin configuration and tries to load the axes in the
  ///        scene.
  /// @details When the scene is not available, a timer is started to try every
  ///          `kTimerPeriodInMs` ms to load the axes.
  ///          It only works with ogre rendering engine.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// @{ isVisible accessors.
  Q_INVOKABLE bool IsVisible() const { return isVisible; }

  Q_INVOKABLE void SetIsVisible(bool _isVisible) {
    isVisible = _isVisible;
    IsVisibleChanged();
    ChangeAxesVisibility();
  }
  /// @}

 signals:
  void IsVisibleChanged();

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
  void DrawAxes(ignition::rendering::ScenePtr scene);

  /// @brief Toggles the visibility of the axes.
  void ChangeAxesVisibility();

  /// @brief Triggers an event every `kTimerPeriodInMs` to try to draw the axes.
  QBasicTimer timer;

  /// @brief The scene name.
  std::string sceneName{"scene"};

  /// @brief Holds the visibility status of the axes.
  bool isVisible{true};

  /// @brief Pointers to the visual axes
  std::array<ignition::rendering::VisualPtr, 3> axes{nullptr, nullptr, nullptr};
};

}  // namespace gui
}  // namespace delphyne
