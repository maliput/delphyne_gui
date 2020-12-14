// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_DISPLAYS_AGENTINFODISPLAY_HH
#define DELPHYNE_GUI_DISPLAYS_AGENTINFODISPLAY_HH

#include <memory>
#include <string>

#include <ignition/gui/DisplayPlugin.hh>
#include <ignition/gui/qt.h>
#include <ignition/transport.hh>

#include "delphyne/protobuf/agent_state_v.pb.h"

namespace delphyne {
namespace gui {
namespace display_plugins {

struct AgentInfoText;
class AgentInfoDisplayPrivate;

/// \brief Display the origin on an Ignition Rendering scene.
class AgentInfoDisplay : public ignition::gui::DisplayPlugin {
  Q_OBJECT

  /// \brief Constructor
 public:
  AgentInfoDisplay();

  /// \brief Destructor
 public:
  virtual ~AgentInfoDisplay();

 private slots:
  void ProcessMsg();

 private slots:
  void ToggleText(const QString& _agentName);

  // Documentation inherited
 public:
  QWidget* CreateCustomProperties() const override;

 private:
  void OnAgentState(const ignition::msgs::AgentState_V& _msg);

 private:
  std::string NameFromAgent(const ignition::msgs::AgentState& agent);

 private:
  std::shared_ptr<AgentInfoText> CreateAgentText(const std::string& _agentName, QVBoxLayout* _layout,
                                                 std::shared_ptr<ignition::rendering::Scene> _scenePtr);

 private:
  void UpdateAgentLabel(const ignition::msgs::AgentState& _agent, const std::string& _agentName,
                        std::shared_ptr<AgentInfoText> _agentInfoText);

 private:
  QVBoxLayout* CreateLayout();

  /// \internal
  /// \brief Pointer to private data.
 private:
  std::unique_ptr<AgentInfoDisplayPrivate> dataPtr;
};
}  // namespace display_plugins
}  // namespace gui
}  // namespace delphyne

#endif
