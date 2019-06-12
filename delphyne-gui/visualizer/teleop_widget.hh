// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_TELEOPWIDGET_HH
#define DELPHYNE_GUI_TELEOPWIDGET_HH

#include <memory>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport.hh>

namespace delphyne {
namespace gui {

/// \class TeleopWidget
/// \brief This is a class that implements a simple ign-gui widget for
/// teleop-ing.
class TeleopWidget : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  /// \brief Default constructor.
  explicit TeleopWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~TeleopWidget();

  /// \brief Callback to start the driving setup process
 public slots:
  void StartDriving();

  /// \brief Notify that the response to the asynchronous service call
  /// to the bridge returned.
 signals:
  void RepeatingDriveTopic(const ignition::msgs::Boolean& response,
                           const bool result);

  /// \brief Callback to finish start driving
 public slots:
  void DriveTopicComplete(const ignition::msgs::Boolean& response,
                          const bool result);

 protected:
  virtual void keyPressEvent(QKeyEvent* _event) override;
  virtual void keyReleaseEvent(QKeyEvent* _event) override;
  void mousePressEvent(QMouseEvent* _event) override;
  void timerEvent(QTimerEvent* event) override;
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

 private:
  /// \internal
  /// \brief A transport node.
  ignition::transport::Node node_;

  /// \internal
  /// \brief The ignition publisher used to send the updates
  std::unique_ptr<ignition::transport::Node::Publisher> publisher_;

  /// \internal
  /// \brief The current amount of throttle
  double currentThrottle;

  /// \internal
  /// \brief The current amount of brake
  double currentBrake;

  /// \internal
  /// \brief True if any valid key is pressed
  bool throttleKeyPressed = false;

  /// \internal
  /// \brief Whether the brake key is currently pressed
  bool brakeKeyPressed = false;

  /// \internal
  /// \brief True if keeping current throttle value
  /// without reseting to zero is enabled.
  bool keepCurrentThrottle = false;

  /// \internal
  /// \brief True if keeping current brake value
  /// without reseting to zero is enabled.
  bool keepCurrentBrake = false;

  /// \internal
  /// \brief Whether a new steering angle has been
  /// computed since the last time the timer ran.
  bool newSteeringAngle = false;

  /// \internal
  /// \brief The current steering angle
  double currentSteeringAngle;

  /// \internal
  /// \brief The current state of the widget
  bool driving;

  QLineEdit* lineedit;
  QPushButton* button;
  QLabel* steeringAngleLabel;
  QLabel* throttleValueLabel;
  QLabel* brakeValueLabel;
  QBasicTimer timer;

  /// \internal
  /// \brief Compute and set a valid throttle value
  void computeClampAndSetThrottle(double throttleGradient);

  /// \internal
  /// \brief Compute and set a valid brake value
  void computeClampAndSetBrake(double brakeGradient);

  /// \internal
  /// \brief Compute and set a valid steering angle value
  void computeClampAndSetSteeringAngle(double sign);
};
}
}

#endif
