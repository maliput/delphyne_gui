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
