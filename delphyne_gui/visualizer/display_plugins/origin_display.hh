// BSD 3-Clause License
//
// Copyright (c) 2022, Woven Planet. All rights reserved.
// Copyright (c) 2021-2022, Toyota Research Institute. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#pragma once

#include <array>
#include <atomic>
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

  Q_INVOKABLE void SetIsVisible(bool _isVisible);
  /// @}

 signals:
  void IsVisibleChanged();

 protected:
  /// @brief Timer event callback which handles the logic to get the scene.
  void timerEvent(QTimerEvent* _event) override;

  /// \brief Filters ignition::gui::events::Render events to update the visualization of the axis if needed.
  /// \details To make this method be called by Qt Event System, install the event filter in target object.
  ///          \see QObject::installEventFilter() method.
  bool eventFilter(QObject* _obj, QEvent* _event) override;

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

  /// @brief Triggers an event every `kTimerPeriodInMs` to try to get the scene.
  QBasicTimer timer;

  /// @brief The scene name.
  std::string sceneName{"scene"};

  /// @brief Holds the visibility status of the axes.
  bool isVisible{true};

  /// @brief Pointers to the visual axes
  std::array<ignition::rendering::VisualPtr, 3> axes{nullptr, nullptr, nullptr};

  /// @brief Indicates whether the axes are drawn.
  std::atomic<bool> areAxesDrawn{false};

  /// @brief Indicates whether the axes visibility should be updated.
  std::atomic<bool> isDirty{true};

  /// @brief Holds a pointer to the scene.
  ignition::rendering::ScenePtr scene{nullptr};
};

}  // namespace gui
}  // namespace delphyne
