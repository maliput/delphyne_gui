// Copyright 2021 Toyota Research Institute

#include <chrono>
#include <iostream>
#include <string>

#include <delphyne/macros.h>
#include <delphyne/protobuf/playback_status.pb.h>
#include <maliput/common/maliput_throw.h>

#include <ignition/common/PluginMacros.hh>
#include <ignition/msgs.hh>
#include <ignition/plugin/Register.hh>

#include "playback_plugin.hh"

Q_DECLARE_METATYPE(ignition::msgs::PlaybackStatus)

namespace delphyne {
namespace gui {
namespace {

// @brief Converts from ignition::msgs::Time to std::chrono::duration.
// @param[in] _src Time to be converted
// @returns `_src` converted to std::chrono::duration::nanoseconds.
std::chrono::nanoseconds IgnitionTimeToChrono(const ignition::msgs::Time& _src) {
  return (std::chrono::seconds(_src.sec()) + std::chrono::nanoseconds(_src.nsec()));
}

// @brief Converts from std::chrono::nanoseconds to a QString
//        where the first three digits and the "s" indicating the unit are printed.
// @param[in] _src Time
// @returns String containing the time.
QString ChronoToQString(const std::chrono::nanoseconds& _src) {
  return QString::number(
             static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(_src).count()) / 1000., 'f', 3) +
         " s";
}

// @brief Converts from std::chrono::nanoseconds to ignition::msgs::Duration.
// @param[in] _src Time
// @param[out] dst Duration
// @throws maliput::common::assertion_error When `dst` is nullptr.
void ChronoToDuration(const std::chrono::nanoseconds& _src, ignition::msgs::Duration* dst) {
  MALIPUT_THROW_UNLESS(dst != nullptr);
  const std::chrono::seconds src_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(_src);
  dst->set_sec(std::floor(src_in_seconds.count()));
  dst->set_nsec((_src - src_in_seconds).count());
}

}  // namespace

PlaybackPlugin::PlaybackPlugin() : ignition::gui::Plugin() { qRegisterMetaType<ignition::msgs::PlaybackStatus>(); }

void PlaybackPlugin::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  title = "PlaybackWidget";

  if (!_pluginElem) {
    ignerr << "Error reading plugin XML element " << std::endl;
  }

  if (!node.Subscribe(kStatusTopicName, &PlaybackPlugin::OnStatusMessage, this)) {
    ignerr << "Error subscribing to topic "
           << "[" << kStatusTopicName << "]" << std::endl;
  }
}

QString PlaybackPlugin::SimTime() const { return simTime; }

QString PlaybackPlugin::CurrentTime() const { return currentTime; }

QString PlaybackPlugin::SliderValue() const { return sliderValue; }

void PlaybackPlugin::SetSimTime(const QString& _simTime) {
  simTime = _simTime;
  SimTimeChanged();
}

void PlaybackPlugin::SetCurrentTime(const QString& _currentTime) {
  currentTime = _currentTime;
  CurrentTimeChanged();
}

void PlaybackPlugin::SetSliderValue(const QString& _sliderValue) {
  sliderValue = _sliderValue;
  SliderValueChanged();
}

void PlaybackPlugin::OnStatusMessage(const ignition::msgs::PlaybackStatus& _msg) {
  timeStatus.current_time = IgnitionTimeToChrono(_msg.current_time());
  timeStatus.start_time = IgnitionTimeToChrono(_msg.start_time());
  timeStatus.end_time = IgnitionTimeToChrono(_msg.end_time());
  SetSimTime(ChronoToQString(timeStatus.current_time) + " / " + ChronoToQString(timeStatus.end_time));
  SetCurrentTime(ChronoToQString(timeStatus.current_time));
  SetSliderValue(QString::fromStdString(std::to_string(timeStatus.current_time.count() * 100. /
                                                       (timeStatus.end_time.count() - timeStatus.start_time.count()))));
}

void PlaybackPlugin::OnRewindButtonPush() {
  ignition::msgs::Duration msg;
  msg.set_sec(0);
  msg.set_nsec(0);
  RequestSeek(msg);
}

void PlaybackPlugin::OnPauseButtonPush() { RequestPause(); }

void PlaybackPlugin::OnPlayButtonPush() { RequestResume(); }

void PlaybackPlugin::OnStepButtonPush(const QString& _stepValue) {
  const std::chrono::milliseconds time_step(_stepValue.toInt());
  ignition::msgs::Duration time_step_msg;
  ChronoToDuration(time_step, &time_step_msg);
  RequestStep(time_step_msg);
}

QString PlaybackPlugin::OnSliderDrop(const QString& _sliderValue) {
  const int kNanoToMilliRatio{1000000};
  const double kSliderSpan{100.};
  const std::chrono::milliseconds new_current_time{static_cast<int>(
      ((timeStatus.end_time.count() - timeStatus.start_time.count()) * _sliderValue.toDouble() / kSliderSpan) /
      kNanoToMilliRatio)};
  timeStatus.current_time = new_current_time;
  ignition::msgs::Duration time_seek_msg;
  ChronoToDuration(new_current_time, &time_seek_msg);
  RequestSeek(time_seek_msg);
  return _sliderValue;
}

void PlaybackPlugin::RequestPause() {
  const ignition::msgs::Empty msg;
  if (!node.Request(kPauseServiceName, msg)) {
    ignerr << "Failed to request a playback pause." << std::endl;
  }
}

void PlaybackPlugin::RequestResume() {
  const ignition::msgs::Empty msg;
  if (!node.Request(kResumeServiceName, msg)) {
    ignerr << "Failed to request a playback resume." << std::endl;
  }
}

void PlaybackPlugin::RequestSeek(const ignition::msgs::Duration& _seekOffset) {
  if (!node.Request(kSeekServiceName, _seekOffset)) {
    ignerr << "Failed to request a playback seek." << std::endl;
  }
}

void PlaybackPlugin::RequestStep(const ignition::msgs::Duration& _stepSize) {
  if (!node.Request(kStepServiceName, _stepSize)) {
    ignerr << "Failed to request a playback step." << std::endl;
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::PlaybackPlugin, ignition::gui::Plugin)
