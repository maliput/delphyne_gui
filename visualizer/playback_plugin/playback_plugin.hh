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
  virtual ~PlaybackPlugin() = default;

  /// Called by Ignition GUI when plugin is instantiated.
  /// \param[in] _pluginElem XML configuration for this plugin.
  virtual void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  Q_INVOKABLE QString SimTime() const;

  Q_INVOKABLE QString CurrentTime() const;

  Q_INVOKABLE QString SliderValue() const;

  Q_INVOKABLE bool IsSliderPressed() const;

  Q_INVOKABLE void SetSimTime(const QString& _simTime);

  Q_INVOKABLE void SetCurrentTime(const QString& _currentTime);

  Q_INVOKABLE void SetSliderValue(const QString& _sliderValue);

  Q_INVOKABLE void SetIsSliderPressed(bool _isSliderPressed);

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
  // @param[in] _stepValue Value to step.
  void OnStepButtonPush(const QString& _stepValue);

  // A slot to react to a slider interaction.
  // @param[in] _sliderValue Value of the slider.
  // @return The value of the slider.
  QString OnSliderDrop(const QString& _sliderValue);

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
  // @param[in] _msg The playback status message received.
  void OnStatusMessage(const ignition::msgs::PlaybackStatus& _msg);

  // Issues a playback pause request.
  void RequestPause();

  // Issues a playback resume request.
  void RequestResume();

  // Issues a playback step request, using the given @p _stepSize.
  void RequestStep(const ignition::msgs::Duration& _stepSize);

  // Issues a playback seek request, to the given @p _seekOffset
  // from playback start.
  void RequestSeek(const ignition::msgs::Duration& _seekOffset);

  /// \brief Holds sim time
  QString simTime;
  /// \brief Holds current time
  QString currentTime;
  /// \brief Holds slider value
  QString sliderValue;
  /// \brief Holds the status of the slider
  bool isSliderPressed{false};

  // An ignition transport node.
  ignition::transport::Node node;

  SimTimes timeStatus;
};

}  // namespace gui
}  // namespace delphyne
