// Copyright 2021 Toyota Research Institute
#include "teleop_plugin.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>

#include <delphyne/protobuf/automotive_driving_command.pb.h>
#include <ignition/common/Console.hh>
#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {
namespace {

// @brief Gets seconds and nanoseconds from the current time.
// @param[out] sec The seconds component of the current time.
// @param[out] nsec The nanoseconds component of the current time.
void GetCurrentTime(int64_t* sec, int32_t* nsec) {
  const auto now = std::chrono::system_clock::now();
  const auto epoch = now.time_since_epoch();
  const int64_t count = epoch.count();
  *nsec = static_cast<int32_t>(count % 1000000000l);
  *sec = (count - static_cast<int64_t>(*nsec)) / 1000000000l;
}

}  // namespace

TeleopPlugin::TeleopPlugin() { timer.start(kTimerPeriodInMs, this); }

void TeleopPlugin::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  if (title.empty()) {
    title = "Teleop";
  }

  if (_pluginElem) {
    if (auto agentChannel = _pluginElem->FirstChildElement("car_number")) {
      SetCarNumber(QString::fromStdString("teleop/" + std::string(agentChannel->GetText())));
    }
  } else {
    SetCarNumber("teleop/0");
  }
}

void TeleopPlugin::OnStartDriving() {
  publisher = std::make_unique<ignition::transport::Node::Publisher>();
  *publisher = node.Advertise<ignition::msgs::AutomotiveDrivingCommand>("/" + carNumber);
}

void TeleopPlugin::UpdateBrakeValue(double brakeGradient) {
  brakeValue = std::clamp(brakeValue + kBrakeScale * brakeGradient, kMinBrakeValue, kMaxBrakeValue);
  BrakeValueChanged();
}

void TeleopPlugin::UpdateThrottleValue(double throttleGradient) {
  throttleValue = std::clamp(throttleValue + kThrottleScale * throttleGradient, kMinThrottleValue, kMaxBrakeValue);
  ThrottleValueChanged();
}

void TeleopPlugin::UpdateSteeringAngle(double sign) {
  steeringAngleValue = std::clamp(steeringAngleValue + kStepSteeringAngle * sign, kMinSteeringAngle, kMaxSteeringAngle);
  SteeringAngleValueChanged();
}

void TeleopPlugin::timerEvent(QTimerEvent* _event) {
  if (_event->timerId() != timer.timerId()) {
    return;
  }
  if (!isDriving) {
    return;
  }

  const double oldThrottleValue = throttleValue;
  const double oldBrakeValue = brakeValue;

  if (!keepCurrentThrottle) {
    UpdateThrottleValue(throttleKeyPressed ? 1.0 : -6.0);
  }

  if (!keepCurrentBrake) {
    UpdateBrakeValue(brakeKeyPressed ? 1.0 : -6.0);
  }

  if (oldThrottleValue != throttleValue /* new throttle value */ || oldBrakeValue != brakeValue /* new brake value */ ||
      newSteeringAngle /* new steering angle */) {
    ignition::msgs::AutomotiveDrivingCommand ignMsg;

    // Note: we don't set the header here since the bridge completely
    // ignores it.
    int32_t nsec;
    int64_t sec;
    GetCurrentTime(&sec, &nsec);
    ignMsg.mutable_time()->set_sec(sec);
    ignMsg.mutable_time()->set_nsec(nsec);

    ignMsg.set_acceleration(throttleValue - brakeValue);
    ignMsg.set_theta(steeringAngleValue);

    publisher->Publish(ignMsg);

    SteeringAngleValueChanged();
    ThrottleValueChanged();
    BrakeValueChanged();
    SetNewSteeringAngle(false);
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TeleopPlugin, ignition::gui::Plugin)
