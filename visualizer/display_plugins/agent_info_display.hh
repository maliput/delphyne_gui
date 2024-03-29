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

#include <memory>
#include <string>

#include <delphyne/protobuf/agent_state_v.pb.h>
#include <ignition/gui/Plugin.hh>
#include <ignition/gui/qt.h>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/transport.hh>

namespace delphyne {
namespace gui {

struct AgentInfoText;

/// @brief Implements a plugin to display the state information for agents in the scene.
/// @details ign-gui3 does not have DisplayPlugins, so this plugin
///          implements a slightly different logic to what the original ign-gui0
///          DisplayPlugin did. It subscribes to events emitted by the MainWindow and
///          checks for `ignition::gui::events::Render` to make rendering calls. On its
///          first Render event, it gets a pointer to the scene and subscribes to the
///          agent info topic. On subsequent Render events, it checks for new agent info
///          data and creates or updates a text geometry to display this data.
///          Typically, this plugin goes hand in hand with the Scene3D plugin.
///          The plugin UI has a checkbox to toggle visibility. It is paired
///          with `isVisible`
class AgentInfoDisplay : public ignition::gui::Plugin {
  Q_OBJECT

  Q_PROPERTY(bool isVisible READ IsVisible WRITE SetIsVisible NOTIFY IsVisibleChanged)

 public:
  AgentInfoDisplay() = default;

  /// @brief Loads the plugin configuration.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// @{ isVisible accessors.
  Q_INVOKABLE bool IsVisible() const { return isVisible; }

  Q_INVOKABLE void SetIsVisible(bool _isVisible) {
    isVisible = _isVisible;
    IsVisibleChanged();
    dirty = true;
  }
  /// @}

 private slots:
  void ProcessMsg();

 signals:
  void IsVisibleChanged();

 private:
  /// @brief Callback for all installed event filters. On Render events, if the scene pointer
  /// is not yet available, it will try to get it and then subscibe to the agent info topic if
  /// successful. On subsequent calls, it will create and update text geometries if new data
  /// is available (indicated by the dirty flag).
  /// @param[in] _obj Object that received the event
  /// @param[in] _event Event
  bool eventFilter(QObject* _obj, QEvent* _event) override;

  /// @brief The rendering engine name.
  const std::string kEngineName{"ogre"};

  /// @brief The scene name.
  std::string sceneName{"scene"};

  /// @brief The scene pointer.
  ignition::rendering::ScenePtr scenePtr;

  /// @brief Holds the visibility status of the agent info.
  bool isVisible{true};

  /// @brief Flag to indicate that new data is available for rendering.
  bool dirty{false};

  /// @brief Message holding latest agent states
  ignition::msgs::AgentState_V msg;

  /// @brief Mutex to protect msg
  std::recursive_mutex mutex;

  /// @brief Map from agent name to AgentInfoText.
  std::map<std::string, std::shared_ptr<AgentInfoText>> mapAgentInfoText;

  /// \brief A transport node.
  ignition::transport::Node node;

  /// @brief Toggles the visibility of the agent info.
  void ChangeAgentInfoVisibility();

  /// @brief Callback for agent info subscriber
  void OnAgentState(const ignition::msgs::AgentState_V& _msg);

  /// @brief Extract the agent name from the topic name.
  std::string NameFromAgent(const ignition::msgs::AgentState& agent);

  /// @brief Create floating text visuals for each agent.
  std::shared_ptr<AgentInfoText> CreateAgentText(const std::string& _agentName,
                                                 ignition::rendering::ScenePtr _scenePtr);

  /// @brief Update pose and content of floating text visuals.
  void UpdateAgentLabel(const ignition::msgs::AgentState& _agent, const std::string& _agentName,
                        std::shared_ptr<AgentInfoText> _agentInfoText);
};

}  // namespace gui
}  // namespace delphyne
