// Copyright 2021 Toyota Research Institute
#pragma once

#include <memory>
#include <string>

#include <ignition/gui/Plugin.hh>
#include <ignition/gui/qt.h>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/transport.hh>

#include "delphyne/protobuf/agent_state_v.pb.h"

namespace delphyne {
namespace gui {

struct AgentInfoText;

/// @brief Implements a plugin to display the state information for agents in the scene.
/// @details ign-gui3 does not have DisplayPlugins, so this plugin
///          implements a slightly different logic to what the original ign-gui0
///          DisplayPlugin did. It gets the scene name from the plugin
///          configuration and tries to get a pointer to it. If the scene is not
///          available, then it starts a timer and tries periodically to
///          retrieve the scene.
///          Typically, this plugin goes hand in hand with the Scene3D plugin.
///          The plugin UI has a checkbox to toggle visibility. It is paired
///          with `isVisible`
class AgentInfoDisplay : public ignition::gui::Plugin {
  Q_OBJECT

  Q_PROPERTY(bool isVisible READ IsVisible WRITE SetIsVisible NOTIFY IsVisibleChanged)

 public:
  AgentInfoDisplay() = default;

  /// @brief Loads the plugin configuration and tries to render the agent info in the
  ///        scene.
  /// @details When the scene is not available, a timer is started to try every
  ///          `kTimerPeriodInMs` ms to load the agent info.
  ///          It only works with ogre rendering engine.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// @{ isVisible accessors.
  Q_INVOKABLE bool IsVisible() const { return isVisible; }

  Q_INVOKABLE void SetIsVisible(bool _isVisible) {
    isVisible = _isVisible;
    IsVisibleChanged();
    ChangeAgentInfoVisibility();
  }
  /// @}

 private slots:
  void ProcessMsg();

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

  /// @brief Triggers an event every `kTimerPeriodInMs` to try to draw the agent info.
  QBasicTimer timer;

  /// @brief The scene name.
  std::string sceneName{"scene"};

  /// @brief The scene pointer.
  ignition::rendering::ScenePtr scenePtr;

  /// @brief Holds the visibility status of the agent info.
  bool isVisible{true};

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
