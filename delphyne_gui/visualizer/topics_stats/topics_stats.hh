// Copyright 2021 Toyota Research Institute
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport/Node.hh>

namespace delphyne {
namespace gui {

/// \brief Show stats of all topics.
class TopicsStats : public ignition::gui::Plugin {
  Q_OBJECT

  Q_PROPERTY(QStringList displayedTopicData READ DisplayedTopicData WRITE SetDisplayedTopicData NOTIFY
                 DisplayedTopicDataChanged)

 public:
  Q_INVOKABLE QStringList DisplayedTopicData() const;

  Q_INVOKABLE void SetDisplayedTopicData(const QStringList& _data);

 public:
  /// \brief Constructor.
  TopicsStats();

  // Documentation inherited
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

 signals:
  /// Signals to notify that the property has changed.
  void DisplayedTopicDataChanged();

 protected slots:

  void SearchTopic(const QString& _topic);

 protected:
  /// \brief Timer event callback which handles the publication of new data in the table.
  void timerEvent(QTimerEvent* _event) override;

 private:
  /// \brief Timer period.
  static constexpr int kTimerPeriodInMs{1000};

  class BasicStats {
   public:
    /// \brief Total number of messages received.
    uint64_t numMessages = 0;

    /// \brief Number of messages received during the last second.
    uint64_t numMessagesInLastSec = 0;

    /// \brief Number of bytes received during the last second.
    uint64_t numBytesInLastSec = 0;
  };

  /// \brief Function called each time a topic update is received.
  /// Note that this callback uses the generic signature, hence it may receive
  /// messages with different types.
  /// \param[in] _msgData string of a serialized protobuf message.
  /// \param[in] _size Number of bytes in the serialized message data.
  /// \param[in] _info Meta-information about the message received.
  void OnMessage(const char* _msgData, const size_t _size, const ignition::transport::MessageInfo& _info);

  /// \brief Update the stats of the GUI.
  void UpdateGUIStats();

  /// \brief Reset all the stats.
  void ResetStats();

  /// @brief Triggers an event every `kTimerPeriodInMs`.
  QBasicTimer timer;

  /// \brief Contains the stats of the topics.
  /// The key of `rawData` is the topic name, and the value is a BasicStats instance.
  /// This way the table will be automatically ordered by topic name.
  std::map<std::string, BasicStats> rawData;

  /// \brief Table data to be passed to the table.
  ///  The list is expected to be comformed using blocks of
  ///  [`topic`, `messages`, `frequency`, `bandwidth`]. In the QML file
  ///   this is parsed to get the four values for each row.
  QStringList displayedTopicData;

  /// \brief Holds a user search by topic.
  std::string topicFilter{""};

  /// \brief Topics displayed in the last update.
  std::vector<std::string> prevTopics;

  /// \brief Transport node to obtain information of the topics.
  ignition::transport::Node node;
};

}  // namespace gui
}  // namespace delphyne