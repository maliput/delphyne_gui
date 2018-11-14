// Copyright 2018 Toyota Research Institute

#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering/Text.hh>

#include "ignition/gui/NumberWidget.hh"

#include "delphyne/protobuf/agent_state_v.pb.h"

#include "agent_info_display.hh"

namespace delphyne
{
namespace gui
{
namespace display_plugins
{
  struct AgentInfoText
  {
    /// \brief The text display
    ignition::rendering::TextPtr text;
    ignition::rendering::VisualPtr textVis;
    bool visible{true};
  };

  class AgentInfoDisplayPrivate
  {
    /// \brief Message holding latest agent states
    public: ignition::msgs::AgentState_V msg;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    public: QStackedLayout *stackedLayout;

    public: QWidget *widget;

    public: QSignalMapper *signalMapper;

    /// \brief A transport node.
    public: ignition::transport::Node node;

    public: std::map<std::string, std::shared_ptr<AgentInfoText>> agentInfoText;

    /// \brief Text size in pixels
    public: unsigned int textSize = 15;

    /// \brief Horizontal padding away from the image border
    public: int horizontalPadding = 20;

    /// \brief Vertical padding away from the image border
    public: int verticalPadding = 20;

    /// \brief Color of the text
    public: ignition::math::Color textColor = ignition::math::Color::White;
  };
}
}
}

static constexpr double charHeight = 0.3;

using namespace delphyne;
using namespace gui;
using namespace display_plugins;

/////////////////////////////////////////////////
AgentInfoDisplay::AgentInfoDisplay() : DisplayPlugin(), dataPtr(new AgentInfoDisplayPrivate) {
  this->title = "Agent Info";
  this->dataPtr->node.Subscribe("agents/state", &AgentInfoDisplay::OnAgentState,
                                this);

  this->dataPtr->signalMapper = new QSignalMapper(this);
  connect(this->dataPtr->signalMapper, SIGNAL(mapped(QString)), this, SLOT(ToggleText(QString)));
}

/////////////////////////////////////////////////
AgentInfoDisplay::~AgentInfoDisplay() {}

std::string AgentInfoDisplay::NameFromAgent(const ignition::msgs::AgentState& agent)
{
  return agent.name().substr(0, agent.name().length() - 6);
}

/////////////////////////////////////////////////
void AgentInfoDisplay::ToggleText(const QString& _agentName) {
  auto agentInfoText = this->dataPtr->agentInfoText[_agentName.toStdString()];

  agentInfoText->visible = !agentInfoText->visible;

  if (agentInfoText->visible) {
    agentInfoText->text->SetCharHeight(charHeight);
  } else {
    agentInfoText->text->SetCharHeight(0.0);
  }
}

/////////////////////////////////////////////////
QWidget *AgentInfoDisplay::CreateCustomProperties() const {
  // This is more complicated than one might hope, mostly due to types.
  // We really want a "stacked" layout so that we can dynamically add in a new
  // layout once we get data from the ignition subscription.  To acheive that,
  // and keep the objects all attached to the window, we have to:
  //
  // 1.  Create a "top-level" widget; we will hold a reference to this and return
  //     the pointer to it from this method.
  // 2.  Create a QStackedLayout that is the layout for the top-level widget.
  //     A QStackedLayout allows switching between layouts in a "stack", so that
  //     once we get the first message, we can overlay that new layout on the
  //     original, empty layout.
  //
  // Now when we want to attach something new to the stacked layout later, we
  // have to:
  // 1.  Create a layout that contains those widgets.
  // 2.  Create the widgets that we actually want to see in the end.
  // 3.  Create a "dummy" top-level widget, and attach the above layout to
  //     that widget.
  // 4.  Attach the top-level widget to the stackedLayout.
  // 5.  Update the stackedLayout to point to the newest widget.

  this->dataPtr->widget = new QWidget();
  this->dataPtr->stackedLayout = new QStackedLayout();
  this->dataPtr->widget->setLayout(this->dataPtr->stackedLayout);

  return this->dataPtr->widget;
}

/////////////////////////////////////////////////
void AgentInfoDisplay::ProcessMsg() {
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  auto scenePtr = this->Scene().lock();
  if (!scenePtr) {
    ignerr << "Scene invalid. Agent Info display not initialized." << std::endl;
    return;
  }

  QVBoxLayout *layout{nullptr};

  if (this->dataPtr->stackedLayout->count() == 0) {
    // This is the first message; create the widgets and the hovering text that we'll use

    // Step 1 from above
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Step 3 from above
    auto widget = new QWidget();
    widget->setLayout(layout);

    // Step 4 from above
    this->dataPtr->stackedLayout->addWidget(widget);

    // Step 5 from above
    this->dataPtr->stackedLayout->setCurrentIndex(this->dataPtr->stackedLayout->count() - 1);

    // We delay doing step 2 until the loop below so we can just iterate over
    // the agents once.
  }

  for (int i = 0; i < this->dataPtr->msg.states_size(); ++i) {
    ignition::msgs::AgentState agent = this->dataPtr->msg.states(i);
    std::shared_ptr<AgentInfoText> agentInfoText;
    std::string agentName = NameFromAgent(agent);

    // Step 2 from above if necessary
    if (layout != nullptr) {
      auto visibleCheck = new QCheckBox(QString::fromStdString(agentName), this);
      visibleCheck->setChecked(true);
      layout->addWidget(visibleCheck);

      this->dataPtr->signalMapper->setMapping(visibleCheck, QString::fromStdString(agentName));
      connect(visibleCheck, SIGNAL(clicked()), this->dataPtr->signalMapper, SLOT(map()));

      // Now that we've created the widgets, create the hovering text
      agentInfoText = std::make_shared<AgentInfoText>();

      agentInfoText->visible = true;

      agentInfoText->text = scenePtr->CreateText();
      agentInfoText->text->SetShowOnTop(true);
      agentInfoText->text->SetCharHeight(charHeight);

      agentInfoText->textVis = scenePtr->CreateVisual();
      agentInfoText->textVis->SetLocalScale(1.0, 1.0, 1.0);
      agentInfoText->textVis->AddGeometry(agentInfoText->text);

      this->Visual()->AddChild(agentInfoText->textVis);

      this->dataPtr->agentInfoText[agentName] = agentInfoText;
    } else {
      agentInfoText = this->dataPtr->agentInfoText[agentName];
    }

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;
    double vx = 0.0;
    double vy = 0.0;
    double vz = 0.0;
    double vroll = 0.0;
    double vpitch = 0.0;
    double vyaw = 0.0;

    if (agent.has_position()) {
      x = agent.position().x();
      y = agent.position().y();
      z = agent.position().z();
    }
    if (agent.has_orientation()) {
      roll = agent.orientation().roll();
      pitch = agent.orientation().pitch();
      yaw = agent.orientation().yaw();
    }
    if (agent.has_linear_velocity()) {
      vx = agent.linear_velocity().x();
      vy = agent.linear_velocity().y();
      vz = agent.linear_velocity().z();
    }
    if (agent.has_angular_velocity()) {
      vroll = agent.angular_velocity().x();
      vpitch = agent.angular_velocity().y();
      vyaw = agent.angular_velocity().z();
    }

    std::stringstream ss;
    ss << agentName << std::setprecision(2) << " pos:(x:" << x << ",y:" << y << ",z:" << z << ",r:" << roll << ",p:" << pitch << ",y:" << yaw << ")" << " vel:(x:" << vx << ",y:" << vy << ",z:" << vz << ",r:" << vroll << ",p:" << vpitch << ",y" << vyaw << ")";
    agentInfoText->text->SetTextString(ss.str());
    agentInfoText->textVis->SetLocalPose(ignition::math::Pose3d(x, y, z, roll, pitch, yaw));
  }
}

/////////////////////////////////////////////////
void AgentInfoDisplay::OnAgentState(const ignition::msgs::AgentState_V& _msg) {
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::display_plugins::AgentInfoDisplay,
                                  ignition::gui::DisplayPlugin)
