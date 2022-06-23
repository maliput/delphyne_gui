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

#include <cmath>
#include <memory>
#include <string>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport.hh>

namespace delphyne {
namespace gui {

/// @brief Implements a teleoperation plugin.
/// @details See TeleopPlugin.qml to complete understand how the view is
///          connected to each QProperty.
///          The plugin has a text input in which users must place the topic
///          name of the agent to teleoperate. Topic name should be: <teleop/X>
///          where X is the agent number. The plugin appends an extra "/" at the
///          beginning to match topic names.
///          When "Start Driving" is clicked, use:
///          - ↑ to throttle.
///          - ↓ to brake.
///          - ← to steer left.
///          - → to steer right.
///          Note that when either throttling or braking, if ↑ or ↓ are released
///          both values will return to zero. However, that does not happen with
///          the steering control.
///          To "Stop Driving", click again the button.
class TeleopPlugin : public ignition::gui::Plugin {
  Q_OBJECT

  Q_PROPERTY(QString carNumber READ CarNumber WRITE SetCarNumber NOTIFY CarNumberChanged)

  Q_PROPERTY(
      QString steeringAngleValue READ SteeringAngleValue WRITE SetSteeringAngleValue NOTIFY SteeringAngleValueChanged)

  Q_PROPERTY(QString throttleValue READ ThrottleValue WRITE SetThrottleValue NOTIFY ThrottleValueChanged)

  Q_PROPERTY(QString brakeValue READ BrakeValue WRITE SetBrakeValue NOTIFY BrakeValueChanged)

  Q_PROPERTY(bool newSteeringAngle READ NewSteeringAngle WRITE SetNewSteeringAngle NOTIFY NewSteeringAngleChanged)

  Q_PROPERTY(
      bool throttleKeyPressed READ ThrottleKeyPressed WRITE SetThrottleKeyPressed NOTIFY ThrottleKeyPressedChanged)

  Q_PROPERTY(
      bool keepCurrentThrottle READ KeepCurrentThrottle WRITE SetKeepCurrentThrottle NOTIFY KeepCurrentThrottleChanged)

  Q_PROPERTY(bool brakeKeyPressed READ BrakeKeyPressed WRITE SetBrakeKeyPressed NOTIFY BrakeKeyPressedChanged)

  Q_PROPERTY(bool keepCurrentBrake READ KeepCurrentBrake WRITE SetKeepCurrentBrake NOTIFY KeepCurrentBrakeChanged)

  Q_PROPERTY(bool isDriving READ IsDriving WRITE SetIsDriving NOTIFY IsDrivingChanged)

 public:
  /// @brief Constructor.
  /// @details Starts the timer and connects it to `timerEvent()`.
  TeleopPlugin();

  /// @brief Loads the plugin configuration.
  /// @details Expects to find `car_number` only which is translated to
  //           `teleop/<car_number>` as topic name.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  Q_INVOKABLE QString BrakeValue() const { return QString::number(brakeValue, /* format */ 'f', /* precision */ 2); }

  Q_INVOKABLE QString CarNumber() const { return QString::fromStdString(carNumber); }

  Q_INVOKABLE QString SteeringAngleValue() const {
    return QString::number(steeringAngleValue, /* format */ 'f', /* precision */ 2);
  }

  Q_INVOKABLE QString ThrottleValue() const {
    return QString::number(throttleValue, /* format */ 'f', /* precision */ 2);
  }

  Q_INVOKABLE bool NewSteeringAngle() const { return newSteeringAngle; }

  Q_INVOKABLE bool ThrottleKeyPressed() const { return throttleKeyPressed; }

  Q_INVOKABLE bool KeepCurrentThrottle() const { return keepCurrentThrottle; }

  Q_INVOKABLE bool BrakeKeyPressed() const { return brakeKeyPressed; }

  Q_INVOKABLE bool KeepCurrentBrake() const { return keepCurrentBrake; }

  Q_INVOKABLE bool IsDriving() const { return isDriving; }

  Q_INVOKABLE void SetNewSteeringAngle(bool _newSteeringAngle) {
    newSteeringAngle = _newSteeringAngle;
    NewSteeringAngleChanged();
  }

  Q_INVOKABLE void SetThrottleKeyPressed(bool _throttleKeyPressed) {
    throttleKeyPressed = _throttleKeyPressed;
    ThrottleKeyPressedChanged();
  }

  Q_INVOKABLE void SetKeepCurrentThrottle(bool _keepCurrentThrottle) {
    keepCurrentThrottle = _keepCurrentThrottle;
    KeepCurrentThrottleChanged();
  }

  Q_INVOKABLE void SetBrakeKeyPressed(bool _brakeKeyPressed) {
    brakeKeyPressed = _brakeKeyPressed;
    BrakeKeyPressedChanged();
  }

  Q_INVOKABLE void SetKeepCurrentBrake(bool _keepCurrentBrake) {
    keepCurrentBrake = _keepCurrentBrake;
    KeepCurrentBrakeChanged();
  }

  Q_INVOKABLE void SetIsDriving(bool _isDriving) {
    isDriving = _isDriving;
    IsDrivingChanged();
  }

  Q_INVOKABLE void SetCarNumber(const QString& _carNumber) {
    carNumber = _carNumber.toStdString();
    CarNumberChanged();
  }

  Q_INVOKABLE void SetSteeringAngleValue(const QString& _steeringAngleValue) {
    steeringAngleValue = _steeringAngleValue.toDouble();
    SteeringAngleValueChanged();
  }

  Q_INVOKABLE void SetThrottleValue(const QString& _throttleValue) {
    throttleValue = _throttleValue.toDouble();
    ThrottleValueChanged();
  }

  Q_INVOKABLE void SetBrakeValue(const QString& _brakeValue) {
    brakeValue = _brakeValue.toDouble();
    BrakeValueChanged();
  }

 public slots:

  /// @brief Callback in Qt thread when start driving is clicked.
  void OnStartDriving();

  /// @brief Updates the steering angle from the plugin UI.
  /// @param sign It should be either 1.0 or -1.0 to move the handle wheel.
  void UpdateSteeringAngle(double sign);

 signals:

  /// @{ Signals to notify that properties changed.
  void CarNumberChanged();
  void SteeringAngleValueChanged();
  void ThrottleValueChanged();
  void BrakeValueChanged();
  void NewSteeringAngleChanged();
  void ThrottleKeyPressedChanged();
  void KeepCurrentThrottleChanged();
  void BrakeKeyPressedChanged();
  void KeepCurrentBrakeChanged();
  void IsDrivingChanged();
  /// @} Signals to notify that properties changed.

 protected:
  /// @brief Timer event callback which handles the logic of publishing driving
  ///        commands.
  void timerEvent(QTimerEvent* event) override;

 private:
  /// @{ Constants used in the implementation.
  static constexpr double kStepSteeringAngle{45. / 180. * M_PI / 100.};
  static constexpr double kMinSteeringAngle{-45. / 180. * M_PI};
  static constexpr double kMaxSteeringAngle{45. / 180. * M_PI};

  // Target velocity 60mph, i.e. ~26.8224 m/sec
  static constexpr double kMaxVelocity{26.8224};

  static constexpr double kThrottleScale{kMaxVelocity / 300.0};
  static constexpr double kMinThrottleValue{0.};
  static constexpr double kMaxThrottleValue{kMaxVelocity};

  static constexpr double kBrakeScale{kMaxVelocity / 300.0};
  static constexpr double kMinBrakeValue{0.};
  static constexpr double kMaxBrakeValue{kMaxVelocity};

  static constexpr int kTimerPeriodInMs{10};
  /// @} Constants used in the implementation.

  /// @brief Updates the throttle value.
  /// @param throttleGradient The value to increase the throttle.
  void UpdateThrottleValue(double throttleGradient);

  /// @brief Updates the brake value.
  /// @param brakeGradient The value to increase the brake.
  void UpdateBrakeValue(double brakeGradient);

  /// @{ QProperties.
  std::string carNumber;

  double steeringAngleValue{0.0};

  double throttleValue{0.0};

  double brakeValue{0.0};

  bool newSteeringAngle{false};

  bool throttleKeyPressed{false};

  bool keepCurrentThrottle{false};

  bool brakeKeyPressed{false};

  bool keepCurrentBrake{false};

  bool isDriving{false};
  /// @} QProperties.

  /// @brief Triggers an event every `kTimerPeriodInMs` to process the command
  ///        to send to the agent.
  QBasicTimer timer;

  /// @brief Transport node.
  ignition::transport::Node node;

  /// @brief Transport publisher.
  std::unique_ptr<ignition::transport::Node::Publisher> publisher;
};

}  // namespace gui
}  // namespace delphyne
