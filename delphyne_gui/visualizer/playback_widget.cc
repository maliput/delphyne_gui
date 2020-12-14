// Copyright 2018 Toyota Research Institute

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <tinyxml2.h>

#include <delphyne/macros.h>
#include <delphyne/protobuf/playback_status.pb.h>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/gui/qt.h>
#include <ignition/msgs.hh>

#include "playback_widget.hh"

Q_DECLARE_METATYPE(ignition::msgs::PlaybackStatus)

namespace delphyne {
namespace gui {

namespace {

std::chrono::nanoseconds TimeToChrono(const ignition::msgs::Time& src) {
  return (std::chrono::seconds(src.has_sec() ? src.sec() : 0) +
          std::chrono::nanoseconds(src.has_nsec() ? src.nsec() : 0));
}

void ChronoToDuration(const std::chrono::nanoseconds& src, ignition::msgs::Duration* dst) {
  DELPHYNE_DEMAND(dst != nullptr);
  std::chrono::seconds src_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(src);
  dst->set_sec(std::floor(src_in_seconds.count()));
  dst->set_nsec((src - src_in_seconds).count());
}

}  // namespace

/////////////////////////////////////////////////
PlaybackWidget::PlaybackWidget(QWidget*) : Plugin() { qRegisterMetaType<ignition::msgs::PlaybackStatus>(); }

/////////////////////////////////////////////////
PlaybackWidget::~PlaybackWidget() {}

/////////////////////////////////////////////////
void PlaybackWidget::LoadConfig(const tinyxml2::XMLElement*) {
  this->title = "PlaybackWidget";
  this->setEnabled(false);

  // Creates all child widgets.
  rewind_button_ = new QPushButton(QIcon(":/icons/rewind.svg"), "");
  rewind_button_->setToolTip("Rewind");
  this->connect(rewind_button_, SIGNAL(clicked()), this, SLOT(OnRewindButtonPush()));

  pause_button_ = new QPushButton(QIcon(":/icons/pause.svg"), "");
  pause_button_->setToolTip("Pause");
  pause_button_->setCheckable(true);
  pause_button_->setAutoExclusive(true);
  this->connect(pause_button_, SIGNAL(clicked()), this, SLOT(OnPauseButtonPush()));

  play_button_ = new QPushButton(QIcon(":/icons/play.svg"), "");
  play_button_->setToolTip("Play");
  play_button_->setCheckable(true);
  play_button_->setAutoExclusive(true);
  this->connect(play_button_, SIGNAL(clicked()), this, SLOT(OnPlayButtonPush()));

  step_button_ = new QPushButton(QIcon(":/icons/step.svg"), "");
  step_button_->setToolTip("Step forward");
  this->connect(step_button_, SIGNAL(clicked()), this, SLOT(OnStepButtonPush()));

  time_step_spinbox_ = new QSpinBox();
  // Max value gets overwritten as soon as the time range is known.
  time_step_spinbox_->setRange(10, INT32_MAX);
  time_step_spinbox_->setSuffix(" ms");
  time_step_spinbox_->setSingleStep(1);
  time_step_spinbox_->setToolTip("Time step");

  time_label_ = new QLabel;

  timeline_slider_ = new QSlider;
  timeline_slider_->setMinimum(0);
  timeline_slider_->setTracking(true);
  timeline_slider_->setOrientation(Qt::Horizontal);
  this->connect(timeline_slider_, SIGNAL(sliderPressed()), this, SLOT(OnTimelinePress()));
  this->connect(timeline_slider_, SIGNAL(sliderMoved(int)), this, SLOT(OnTimelineMove(int)));
  this->connect(timeline_slider_, SIGNAL(sliderReleased()), this, SLOT(OnTimelineRelease()));

  duration_label_ = new QLabel;

  // Creates the layout to hold children widgets.
  auto layout = new QHBoxLayout;
  layout->addWidget(rewind_button_);
  layout->addWidget(pause_button_);
  layout->addWidget(play_button_);
  layout->addWidget(step_button_);
  layout->addWidget(time_step_spinbox_);
  layout->addWidget(time_label_);
  layout->addWidget(timeline_slider_);
  layout->addWidget(duration_label_);

  // Uses the layout.
  this->setLayout(layout);

  // Connects update slot with status subscription through an
  // auxiliary signal.
  this->connect(this, SIGNAL(OnStatusChange(const ignition::msgs::PlaybackStatus&)), this,
                SLOT(Update(const ignition::msgs::PlaybackStatus&)));

  if (!node_.Subscribe(kStatusTopicName, &PlaybackWidget::OnStatusMessage, this)) {
    ignerr << "Error subscribing to topic "
           << "[" << kStatusTopicName << "]" << std::endl;
  }

  // Spins up a timer to monitor backend responsiveness.
  this->startTimer((kStatusUpdateMaxDelay / 10).count());
}

/////////////////////////////////////////////////
void PlaybackWidget::OnRewindButtonPush() {
  ignition::msgs::Duration msg;
  msg.set_sec(0);
  msg.set_nsec(0);
  this->RequestSeek(msg);
}

/////////////////////////////////////////////////
void PlaybackWidget::OnPauseButtonPush() { this->RequestPause(); }

/////////////////////////////////////////////////
void PlaybackWidget::OnPlayButtonPush() { this->RequestResume(); }

/////////////////////////////////////////////////
void PlaybackWidget::OnStepButtonPush() {
  const std::chrono::milliseconds time_step(time_step_spinbox_->value());
  ignition::msgs::Duration time_step_msg;
  ChronoToDuration(time_step, &time_step_msg);
  this->RequestStep(time_step_msg);
}

/////////////////////////////////////////////////
void PlaybackWidget::OnTimelinePress() { timeline_interaction_ = true; }

/////////////////////////////////////////////////
void PlaybackWidget::OnTimelineMove(int slider_location) {
  ignition::msgs::Duration msg;
  ChronoToDuration(slider_location * timeline_scale_, &msg);
  this->RequestSeek(msg);
}

/////////////////////////////////////////////////
void PlaybackWidget::OnTimelineRelease() { timeline_interaction_ = false; }

/////////////////////////////////////////////////
void PlaybackWidget::timerEvent(QTimerEvent*) {
  const std::chrono::nanoseconds current_time = std::chrono::steady_clock::now().time_since_epoch();
  // Disables the entire widget if playback status updates have stopped coming.
  this->setEnabled(current_time - last_update_time_ < kStatusUpdateMaxDelay);
}

/////////////////////////////////////////////////
void PlaybackWidget::OnStatusMessage(const ignition::msgs::PlaybackStatus& msg) { emit this->OnStatusChange(msg); }

/////////////////////////////////////////////////
void PlaybackWidget::Update(const ignition::msgs::PlaybackStatus& status) {
  // Enables step button if playback is paused.
  step_button_->setEnabled(status.paused());
  // Sets play and pause buttons accordingly.
  pause_button_->setChecked(status.paused());
  play_button_->setChecked(!status.paused());
  // Computes playback time stats.
  const std::chrono::nanoseconds start_time = TimeToChrono(status.start_time());
  const std::chrono::nanoseconds end_time = TimeToChrono(status.end_time());
  const std::chrono::nanoseconds time_range = end_time - start_time;
  const std::chrono::nanoseconds current_time = TimeToChrono(status.current_time());
  const std::chrono::nanoseconds elapsed_time = current_time - start_time;
  const std::chrono::milliseconds time_step(time_step_spinbox_->value());
  if (!timeline_interaction_) {
    // Sets timeline slider range and value.
    timeline_slider_->setMaximum(time_range / time_step);
    timeline_slider_->setValue(elapsed_time / time_step);
    // Sets timeline scale (or in other words, its tick duration).
    const int slider_range = timeline_slider_->maximum() - timeline_slider_->minimum();
    timeline_scale_ = time_range / slider_range;
  }
  using seconds = std::chrono::duration<double>;
  constexpr int kFieldWidth{0};      // Default.
  constexpr char kFieldFormat{'f'};  // No scientific notation.
  constexpr int kFieldPrecision{3};  // Up to three (3) places after
                                     // the decimal dot.
  // Uses time label to track playback time, in seconds
  // since the Epoch..
  time_label_->setText(
      QString("Time: %1")
          .arg(std::chrono::duration_cast<seconds>(current_time).count(), kFieldWidth, kFieldFormat, kFieldPrecision));
  // Uses duration label to track playback duration, in seconds.
  duration_label_->setText(
      QString("%1 s / %2 s")
          .arg(std::chrono::duration_cast<seconds>(elapsed_time).count(), kFieldWidth, kFieldFormat, kFieldPrecision)
          .arg(std::chrono::duration_cast<seconds>(time_range).count(), kFieldWidth, kFieldFormat, kFieldPrecision));
  // Updates maximum value in time step spinbox with the actual lenght of the
  // playback. This runs only once since the widget gets enabled afterwards.
  if (!this->isEnabled()) {
    time_step_spinbox_->setMaximum(std::chrono::duration_cast<std::chrono::milliseconds>(time_range).count());
  }

  // Enables the entire widget.
  this->setEnabled(true);
  // Tracks time of update.
  last_update_time_ = std::chrono::steady_clock::now().time_since_epoch();
}

/////////////////////////////////////////////////
void PlaybackWidget::RequestPause() {
  const ignition::msgs::Empty msg;
  if (!node_.Request(kPauseServiceName, msg)) {
    ignerr << "Failed to request a playback pause." << std::endl;
  }
}

/////////////////////////////////////////////////
void PlaybackWidget::RequestResume() {
  const ignition::msgs::Empty msg;
  if (!node_.Request(kResumeServiceName, msg)) {
    ignerr << "Failed to request a playback resume." << std::endl;
  }
}

/////////////////////////////////////////////////
void PlaybackWidget::RequestStep(const ignition::msgs::Duration& step_size) {
  if (!node_.Request(kStepServiceName, step_size)) {
    ignerr << "Failed to request a playback step." << std::endl;
  }
}

/////////////////////////////////////////////////
void PlaybackWidget::RequestSeek(const ignition::msgs::Duration& seek_offset) {
  if (!node_.Request(kSeekServiceName, seek_offset)) {
    ignerr << "Failed to request a playback seek." << std::endl;
  }
}

}  // namespace gui
}  // namespace delphyne

IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::PlaybackWidget, ignition::gui::Plugin)
