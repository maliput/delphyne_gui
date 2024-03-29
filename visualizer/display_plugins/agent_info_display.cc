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
#include "agent_info_display.hh"

#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>

#include <delphyne/protobuf/agent_state_v.pb.h>
#include <ignition/common/Console.hh>
#include <ignition/gui/Application.hh>
#include <ignition/gui/GuiEvents.hh>
#include <ignition/gui/MainWindow.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/Text.hh>
#include <ignition/rendering/Visual.hh>

namespace delphyne {
namespace gui {
struct AgentInfoText {
  /// \brief The text display
  ignition::rendering::TextPtr text;
  ignition::rendering::VisualPtr textVis;
};

static constexpr double charHeight = 0.3;

/////////////////////////////////////////////////
void AgentInfoDisplay::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  title = "Agent Info Display";

  if (_pluginElem) {
    // Update the requested scene name even it fails to load.
    if (auto elem = _pluginElem->FirstChildElement("scene")) {
      this->sceneName = elem->GetText();
    }
    // Similarly with the visibility flag.
    if (auto elem = _pluginElem->FirstChildElement("visible")) {
      elem->QueryBoolText(&isVisible);
      IsVisibleChanged();
    }
  }

  ignition::gui::App()->findChild<ignition::gui::MainWindow*>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool AgentInfoDisplay::eventFilter(QObject* _obj, QEvent* _event) {
  if (_event->type() == ignition::gui::events::Render::kType) {
    if (nullptr == this->scenePtr) {
      auto engine = ignition::rendering::engine(kEngineName);
      this->scenePtr = engine->SceneByName(this->sceneName);
      if (nullptr == this->scenePtr) {
        ignwarn << "Scene \"" << this->sceneName << "\" not found, "
                << "agent info display plugin won't work until the scene is created." << std::endl;
      } else {
        // Subscribe to agent info once the scene pointer is found
        this->node.Subscribe("agents/state", &AgentInfoDisplay::OnAgentState, this);
      }
    } else if (this->dirty) {
      this->ProcessMsg();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void AgentInfoDisplay::OnAgentState(const ignition::msgs::AgentState_V& _msg) {
  std::lock_guard<std::recursive_mutex> lock(this->mutex);

  this->msg.CopyFrom(_msg);
  this->dirty = true;
}

/////////////////////////////////////////////////
void AgentInfoDisplay::ProcessMsg() {
  std::lock_guard<std::recursive_mutex> lock(this->mutex);

  for (int i = 0; i < this->msg.states_size(); ++i) {
    ignition::msgs::AgentState agent = this->msg.states(i);
    std::shared_ptr<AgentInfoText> agentInfoText;
    const std::string agentName = NameFromAgent(agent);

    // Step 2 from above if necessary
    if (mapAgentInfoText.find(agentName) == mapAgentInfoText.end()) {
      agentInfoText = CreateAgentText(agentName, scenePtr);
    } else {
      agentInfoText = this->mapAgentInfoText[agentName];
    }

    UpdateAgentLabel(agent, agentName, agentInfoText);
  }

  ChangeAgentInfoVisibility();

  this->dirty = false;
}

/////////////////////////////////////////////////
std::string AgentInfoDisplay::NameFromAgent(const ignition::msgs::AgentState& agent) {
  // The names that we get from the agents are of the form:
  //
  // "/agent/0/state"
  //
  // To reduce screen real estate, remove the "/agent/" from the start and
  // "/state" from the rear.
  return agent.name().substr(7, agent.name().length() - 7 - 6);
}

/////////////////////////////////////////////////
std::shared_ptr<AgentInfoText> AgentInfoDisplay::CreateAgentText(const std::string& _agentName,
                                                                 ignition::rendering::ScenePtr _scenePtr) {
  auto agentInfoText = std::make_shared<AgentInfoText>();

  agentInfoText->text = _scenePtr->CreateText();
  agentInfoText->text->SetShowOnTop(true);
  agentInfoText->text->SetCharHeight(charHeight);

  agentInfoText->textVis = _scenePtr->CreateVisual();
  agentInfoText->textVis->SetLocalScale(1.0, 1.0, 1.0);
  agentInfoText->textVis->AddGeometry(agentInfoText->text);

  _scenePtr->RootVisual()->AddChild(agentInfoText->textVis);

  mapAgentInfoText[_agentName] = agentInfoText;

  ChangeAgentInfoVisibility();

  return agentInfoText;
}

/////////////////////////////////////////////////
void AgentInfoDisplay::UpdateAgentLabel(const ignition::msgs::AgentState& _agent, const std::string& _agentName,
                                        std::shared_ptr<AgentInfoText> _agentInfoText) {
  ignition::math::Vector3d pos;
  double roll = 0.0;
  double pitch = 0.0;
  double yaw = 0.0;
  ignition::math::Vector3d linear_velocity;

  if (_agent.has_position()) {
    pos = ignition::msgs::Convert(_agent.position());
  }
  if (_agent.has_orientation()) {
    roll = _agent.orientation().roll();
    pitch = _agent.orientation().pitch();
    yaw = _agent.orientation().yaw();
  }
  if (_agent.has_linear_velocity()) {
    linear_velocity = ignition::msgs::Convert(_agent.linear_velocity());
  }

  std::stringstream ss;
  ss << std::setprecision(2);
  ss << _agentName << ":\n pos:(" << pos << "), yaw:(" << yaw << ")\n vel:(" << linear_velocity << ")";
  _agentInfoText->text->SetTextString(ss.str());
  _agentInfoText->textVis->SetLocalPose(ignition::math::Pose3d(pos.X(), pos.Y(), pos.Z() + 2.6, roll, pitch, yaw));
}

/////////////////////////////////////////////////
void AgentInfoDisplay::ChangeAgentInfoVisibility() {
  const bool newIsVisibleValue = isVisible;
  for (auto& nameToAgentInfoText : mapAgentInfoText) {
    auto name = nameToAgentInfoText.first;
    auto agentInfoText = nameToAgentInfoText.second;
    if (agentInfoText && agentInfoText->textVis) {
      agentInfoText->textVis->SetVisible(newIsVisibleValue);
    }
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::AgentInfoDisplay, ignition::gui::Plugin)
