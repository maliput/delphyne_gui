// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_TIMEWIDGET_HH_
#define DELPHYNE_GUI_TIMEWIDGET_HH_

#include <memory>

#include <ignition/msgs.hh>
#include <ignition/transport.hh>

#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace msgs
{
  class WorldStatisticsD;
}
}

namespace delphyne
{
namespace gui
{
  class TimeWidgetPrivate;

  /// \brief This plugin provides a time panel which may:
  /// * Display simulation time
  /// * Display real time
  /// * Have a play / pause and step buttons
  ///
  /// ## Configuration
  ///
  /// \<world_control\> : Configuration for controlling the world, optional.
  /// * \<play_pause\> : Set to true to see a play/pause and step buttons,
  ///                    false by default.
  /// * \<start_paused\> : Set to false to start playing, false by default.
  /// * \<service\> : Service for world control, required.
  /// \<world_stats\> : Configuration for displaying stats, optional.
  /// * \<sim_time\> : Set to true to display a sim time widget, false by
  ///                  default.
  /// * \<real_time\> : True to display a real time widget, false by default.
  /// * \<topic\> : Topic to receive world statistics, required.
  class TimeWidget: public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TimeWidget();

    /// \brief Destructor
    public: virtual ~TimeWidget();

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback in main thread when diagnostics come in
    public slots: void ProcessMsg();

    /// \brief Callback in Qt thread when play button is clicked.
    public slots: void OnPlay();

    /// \brief Callback in Qt thread when pause button is clicked.
    public slots: void OnPause();

    /// \brief Callback in Qt thread when step button is clicked.
    public slots: void OnStep();

    /// \brief Notify that it's now playing.
    signals: void Playing();

    /// \brief Notify that it's now paused.
    signals: void Paused();

    /// \brief Update simulation time.
    signals: void SetSimTime(QString _time);

    /// \brief Update real time.
    signals: void SetRealTime(QString _time);

    /// \brief Update real time factor.
    signals: void SetRealTimeFactor(QString _rtf);

    /// \brief Subscriber callback when new world statistics are received
    private: void OnWorldStatsMsg(const ignition::msgs::WorldStatisticsD &_msg);

    // Private data
    private: std::unique_ptr<TimeWidgetPrivate> dataPtr;
  };
}
}

#endif
