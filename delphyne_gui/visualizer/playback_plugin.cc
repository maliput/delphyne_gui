// Copyright 2021 Toyota Research Institute

#include <chrono>
#include <iostream>
#include <string>

#include <delphyne/macros.h>
#include <delphyne/protobuf/playback_status.pb.h>

#include <ignition/common/PluginMacros.hh>
#include <ignition/msgs.hh>
#include <ignition/plugin/Register.hh>

#include "playback_plugin.hh"

Q_DECLARE_METATYPE(ignition::msgs::PlaybackStatus)

namespace delphyne {
namespace gui {
namespace {

// @brief Converts from ignition::msgs::Time to std::chrono::duration.
// @param[in] src Time to be converted
// @returns `src` converted to std::chrono::duration::nanoseconds.
std::chrono::nanoseconds TimeToChrono(const ignition::msgs::Time& src) {
  return (std::chrono::seconds(src.sec()) + std::chrono::nanoseconds(src.nsec()));
}

// @brief Converts from std::chrono::nanoseconds to a QString
//        where the first three digits and the "s" indicating the unit are printed.
// @param[in] src Time
// @returns String containing the time.
QString ChronoToQString(const std::chrono::nanoseconds& src) {
  return QString::number(
             static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(src).count()) / 1000., 'f', 3) +
         " s";
}

// @brief Converts from std::chrono::nanoseconds to ignition::msgs::Duration.
// @param[in] src Time
// @param[out] dst Duration
// @throws When `dst` is nullptr.
void ChronoToDuration(const std::chrono::nanoseconds& src, ignition::msgs::Duration* dst) {
  DELPHYNE_DEMAND(dst != nullptr);
  std::chrono::seconds src_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(src);
  dst->set_sec(std::floor(src_in_seconds.count()));
  dst->set_nsec((src - src_in_seconds).count());
}

}  // namespace

PlaybackPlugin::PlaybackPlugin() : ignition::gui::Plugin() { qRegisterMetaType<ignition::msgs::PlaybackStatus>(); }

PlaybackPlugin::~PlaybackPlugin() {}

void PlaybackPlugin::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  title = "PlaybackWidget";

  if (!_pluginElem) {
    ignerr << "Error reading plugin XML element " << std::endl;
    return;
  }

  if (!node_.Subscribe(kStatusTopicName, &PlaybackPlugin::OnStatusMessage, this)) {
    ignerr << "Error subscribing to topic "
           << "[" << kStatusTopicName << "]" << std::endl;
  }
}

QString PlaybackPlugin::SimTime() const { return simTime_; }

QString PlaybackPlugin::CurrentTime() const { return currentTime_; }

QString PlaybackPlugin::SliderValue() const { return sliderValue_; }

void PlaybackPlugin::SetSimTime(const QString& simTime) {
  simTime_ = simTime;
  SimTimeChanged();
}

void PlaybackPlugin::SetCurrentTime(const QString& currentTime) {
  currentTime_ = currentTime;
  CurrentTimeChanged();
}

void PlaybackPlugin::SetSliderValue(const QString& sliderValue) {
  sliderValue_ = sliderValue;
  SliderValueChanged();
}

void PlaybackPlugin::OnStatusMessage(const ignition::msgs::PlaybackStatus& msg) {
  time_status_.current_time = TimeToChrono(msg.current_time());
  time_status_.start_time = TimeToChrono(msg.start_time());
  time_status_.end_time = TimeToChrono(msg.end_time());
  SetSimTime(ChronoToQString(time_status_.current_time) + " / " + ChronoToQString(time_status_.end_time));
  SetCurrentTime(ChronoToQString(time_status_.current_time));
  SetSliderValue(QString::fromStdString(std::to_string(
      time_status_.current_time.count() * 100. / (time_status_.end_time.count() - time_status_.start_time.count()))));
}

void PlaybackPlugin::OnRewindButtonPush() {
  ignition::msgs::Duration msg;
  msg.set_sec(0);
  msg.set_nsec(0);
  RequestSeek(msg);
}

void PlaybackPlugin::OnPauseButtonPush() { RequestPause(); }

void PlaybackPlugin::OnPlayButtonPush() { RequestResume(); }

void PlaybackPlugin::OnStepButtonPush(const QString& stepValue) {
  const std::chrono::milliseconds time_step(stepValue.toInt());
  ignition::msgs::Duration time_step_msg;
  ChronoToDuration(time_step, &time_step_msg);
  RequestStep(time_step_msg);
}

QString PlaybackPlugin::OnSliderDrop(const QString& sliderValue) {
  const int kNanoToMilliRatio{1000000};
  const double kSliderSpan{100.};
  const std::chrono::milliseconds new_current_time{static_cast<int>(
      ((time_status_.end_time.count() - time_status_.start_time.count()) * sliderValue.toDouble() / kSliderSpan) /
      kNanoToMilliRatio)};
  time_status_.current_time = new_current_time;
  ignition::msgs::Duration time_seek_msg;
  ChronoToDuration(new_current_time, &time_seek_msg);
  RequestSeek(time_seek_msg);
  return sliderValue;
}

void PlaybackPlugin::RequestPause() {
  const ignition::msgs::Empty msg;
  if (!node_.Request(kPauseServiceName, msg)) {
    ignerr << "Failed to request a playback pause." << std::endl;
  }
}

void PlaybackPlugin::RequestResume() {
  const ignition::msgs::Empty msg;
  if (!node_.Request(kResumeServiceName, msg)) {
    ignerr << "Failed to request a playback resume." << std::endl;
  }
}

void PlaybackPlugin::RequestSeek(const ignition::msgs::Duration& seek_offset) {
  if (!node_.Request(kSeekServiceName, seek_offset)) {
    ignerr << "Failed to request a playback seek." << std::endl;
  }
}

void PlaybackPlugin::RequestStep(const ignition::msgs::Duration& step_size) {
  if (!node_.Request(kStepServiceName, step_size)) {
    ignerr << "Failed to request a playback step." << std::endl;
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::PlaybackPlugin, ignition::gui::Plugin)
