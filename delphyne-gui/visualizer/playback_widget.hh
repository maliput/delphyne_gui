// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_PLAYBACKWIDGET_HH
#define DELPHYNE_GUI_PLAYBACKWIDGET_HH

#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <ignition/gui/Plugin.hh>

#include <ignition/transport.hh>

// Forward declarations.
namespace tinyxml2 {
class XMLElement;
}
namespace ignition {
namespace msgs {
class Duration;
class PlaybackStatus;
}  // namespace msgs
}  // namespace ignition

namespace delphyne {
namespace gui {

/// This is a class that implements a simple ign-gui widget for
/// rendering a scene, using the ign-rendering functionality.
class PlaybackWidget : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  /// Default constructor.
  explicit PlaybackWidget(QWidget* parent = 0);

  /// Default destructor.
  virtual ~PlaybackWidget();

  /// @brief Overridden method to load user configuration.
  /// @param[in] _pluginElem The data containing the configuration.
  virtual void LoadConfig(const tinyxml2::XMLElement* _pluginElem);

 public slots:
  /// Updates widget using the given playback @p status message.
  void Update(const ignition::msgs::PlaybackStatus& status);

 signals:
  /// Notify that playback status has changed.
  /// @param[in] status New playback status message.
  void OnStatusChange(const ignition::msgs::PlaybackStatus& status);

 protected:
  // Documentation inherited.
  void timerEvent(QTimerEvent* event) override;

 private slots:
  // A slot to react to a rewind button push.
  void OnRewindButtonPush();

  // A slot to react to a pause button push.
  void OnPauseButtonPush();

  // A slot to react to a play button push.
  void OnPlayButtonPush();

  // A slot to react to a step button push.
  void OnStepButtonPush();

  // A slot to detect the start of a timeline interaction.
  void OnTimelinePress();

  // A slot to react to timeline interactions (i.e. the user moving
  // the timeline slider).
  // @param[in] location Timeline slider location (i.e. its value).
  void OnTimelineMove(int location);

  // A slot to detect the end of a timeline interaction.
  void OnTimelineRelease();

 private:
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

  // A button to rewind a playback (i.e. back to the start).
  QPushButton* rewind_button_{nullptr};

  // A button to pause a playback.
  QPushButton* pause_button_{nullptr};

  // A button to resume a playback.
  QPushButton* play_button_{nullptr};

  // A button to step a playback.
  QPushButton* step_button_{nullptr};

  // A spinbox to adjust time step sizes.
  QSpinBox* time_step_spinbox_{nullptr};

  // A label to track playback time.
  QLabel* time_label_{nullptr};

  // A slider to act as interactive playback timeline.
  QSlider* timeline_slider_{nullptr};

  // Timeline scale i.e. tick duration.
  std::chrono::nanoseconds timeline_scale_{0};

  // Whether the user is interacting with the
  // timeline or not.
  bool timeline_interaction_{false};

  // A label to track playback duration.
  QLabel* duration_label_{nullptr};

  // Last playback status update time.
  std::chrono::nanoseconds last_update_time_{0};

  // Maximum playback status delay to withstand before
  // assuming no playback is taking place.
  static constexpr std::chrono::milliseconds kStatusUpdateMaxDelay{200};

  // Playback's ignition transport services and topics.
  static constexpr const char* const kStatusTopicName{"/replayer/status"};
  static constexpr const char* const kPauseServiceName{"/replayer/pause"};
  static constexpr const char* const kResumeServiceName{"/replayer/resume"};
  static constexpr const char* const kStepServiceName{"/replayer/step"};
  static constexpr const char* const kSeekServiceName{"/replayer/seek"};

  // An ignition transport node.
  ignition::transport::Node node_;
};
}  // namespace gui
}  // namespace delphyne

#endif
