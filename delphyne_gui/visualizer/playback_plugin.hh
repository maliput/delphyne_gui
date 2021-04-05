// Copyright 2021 Toyota Research Institute
#pragma once

#include <ignition/common/Console.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/gui/qt.h>
#include <ignition/transport.hh>

namespace ignition {
namespace msgs {
class PlaybackStatus;
}  // namespace msgs
}  // namespace ignition

namespace delphyne {
namespace gui {

/// @brief Implements a playback plugin.
/// @details See PlaybackPlugin.qml to complete understand how the view is
///          connected to each QProperty.
class PlaybackPlugin : public ignition::gui::Plugin {
  Q_OBJECT

  Q_PROPERTY(QString simTime READ SimTime WRITE SetSimTime NOTIFY SimTimeChanged)

  Q_PROPERTY(QString currentTime READ CurrentTime WRITE SetCurrentTime NOTIFY CurrentTimeChanged)

  Q_PROPERTY(QString sliderValue READ SliderValue WRITE SetSliderValue NOTIFY SliderValueChanged)

  Q_PROPERTY(bool isSliderPressed READ IsSliderPressed WRITE SetIsSliderPressed NOTIFY IsSliderPressedChanged)

 public:
  /// Constructor
  PlaybackPlugin();

  /// Destructor
  virtual ~PlaybackPlugin();

  /// Called by Ignition GUI when plugin is instantiated.
  /// \param[in] _pluginElem XML configuration for this plugin.
  virtual void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  Q_INVOKABLE QString SimTime() const;

  Q_INVOKABLE QString CurrentTime() const;

  Q_INVOKABLE QString SliderValue() const;

  Q_INVOKABLE bool IsSliderPressed() const;

  Q_INVOKABLE void SetSimTime(const QString& simTime);

  Q_INVOKABLE void SetCurrentTime(const QString& currentTime);

  Q_INVOKABLE void SetSliderValue(const QString& sliderValue);

  Q_INVOKABLE void SetIsSliderPressed(bool isSliderPressed);

 signals:
  /// @{ Signals to notify that properties changed.
  void SimTimeChanged();
  void CurrentTimeChanged();
  void SliderValueChanged();
  void IsSliderPressedChanged();
  /// @} Signals to notify that properties changed.

 protected slots:

  // A slot to react to a rewind button push.
  void OnRewindButtonPush();

  // A slot to react to a pause button push.
  void OnPauseButtonPush();

  // A slot to react to a play button push.
  void OnPlayButtonPush();

  // A slot to react to a step button push.
  // @param[in] stepValue Value to step.
  void OnStepButtonPush(const QString& stepValue);

  // A slot to react to a slider interaction.
  // @param[in] sliderValue Value of the slider.
  // @return The value of the slider.
  QString OnSliderDrop(const QString& sliderValue);

 private:
  // Holds the start, current and end time of the simulation.
  struct SimTimes {
    std::chrono::nanoseconds start_time;
    std::chrono::nanoseconds end_time;
    std::chrono::nanoseconds current_time;
  };

  // Playback's ignition transport services and topics.
  static constexpr const char* const kStatusTopicName{"/replayer/status"};
  static constexpr const char* const kPauseServiceName{"/replayer/pause"};
  static constexpr const char* const kResumeServiceName{"/replayer/resume"};
  static constexpr const char* const kStepServiceName{"/replayer/step"};
  static constexpr const char* const kSeekServiceName{"/replayer/seek"};

  // Playback status topic subscription callback.
  // @param[in] status The playback status message received.
  void OnStatusMessage(const ignition::msgs::PlaybackStatus& status);

  // Issues a playback pause request.
  void RequestPause();

  // Issues a playback resume request.
  void RequestResume();

  // Issues a playback step request, using the given @p step_size.
  void RequestStep(const ignition::msgs::Duration& step_size);

  // Issues a playback seek request, to the given @p seek_offset
  // from playback start.
  void RequestSeek(const ignition::msgs::Duration& seek_offset);

  /// \brief Holds sim time
  QString simTime_;
  /// \brief Holds current time
  QString currentTime_;
  /// \brief Holds slider value
  QString sliderValue_;
  /// \brief Holds the status of the slider
  bool isSliderPressed_{false};

  // An ignition transport node.
  ignition::transport::Node node_;

  SimTimes time_status_;
};

}  // namespace gui
}  // namespace delphyne
