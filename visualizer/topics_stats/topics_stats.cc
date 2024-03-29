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
#include "topics_stats.hh"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <ignition/common/Console.hh>
#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {
namespace {

// \brief Convert @p _value into an string selecting the @p _precision and adding the @p _unit to the
//        value
std::string ToStringWithPrecision(double _value, int _precision, const std::string& _unit = "") {
  std::stringstream sstr;
  sstr << std::fixed << std::setprecision(_precision) << _value;
  if (!_unit.empty()) {
    sstr << " " << _unit;
  }
  return sstr.str();
}

// Get a string containing the bandwith and its correspondant unit.
// \param _bytesLastSec Number of bytes in the a second.
std::string GetBandwidth(uint64_t _bytesInASec) {
  std::string units;
  if (_bytesInASec < 1000) {
    units = "B/s";
  } else if (_bytesInASec < 1000000) {
    _bytesInASec /= 1000u;
    units = "KB/s";
  } else {
    _bytesInASec /= 1000000u;
    units = "MB/s";
  }
  return ToStringWithPrecision(_bytesInASec, 2 /* decimal places */, units);
}

}  // namespace

TopicsStats::TopicsStats() : Plugin() { timer.start(kTimerPeriodInMs, this); }

void TopicsStats::LoadConfig(const tinyxml2::XMLElement* /*_pluginElem*/) {
  if (this->title.empty()) this->title = "Topics Stats";
}

void TopicsStats::OnMessage(const char* /*_msgData*/, const size_t _size,
                            const ignition::transport::MessageInfo& _info) {
  const auto topic = _info.Topic();

  const auto& statsPair = rawData.find(topic);
  if (statsPair == rawData.end()) {
    ignerr << "Unable to find stats for [" << topic << "]" << std::endl;
    return;
  }

  // Update the total number of messages received.
  statsPair->second.numMessages++;

  // Update the number of messages received during the last second.
  statsPair->second.numMessagesInLastSec++;

  // Update the number of bytes received during the last second.
  statsPair->second.numBytesInLastSec += _size;
}

QStringList TopicsStats::DisplayedTopicData() const { return displayedTopicData; }

void TopicsStats::SetDisplayedTopicData(const QStringList& _data) {
  displayedTopicData = _data;
  DisplayedTopicDataChanged();
}

void TopicsStats::SearchTopic(const QString& _topic) { topicFilter = _topic.toStdString(); }

void TopicsStats::timerEvent(QTimerEvent*) {
  // Get all the unique topics.
  std::vector<std::string> topics;
  node.TopicList(topics);

  // Remove expired topics.
  for (auto i = 0u; i < prevTopics.size(); ++i) {
    auto topic = prevTopics.at(i);
    if (std::find(topics.begin(), topics.end(), topic) == topics.end()) {
      // Unsubscribe from the topic.
      node.Unsubscribe(topic);
      // Do not track stats for this topic anymore.
      rawData.erase(topic);
    }
  }

  // Add new topics.
  for (auto i = 0u; i < topics.size(); ++i) {
    const std::string& topic = topics.at(i);
    if (std::find(prevTopics.begin(), prevTopics.end(), topic) == prevTopics.end()) {
      // Subscribe to the topic.
      auto cb =
          std::bind(&TopicsStats::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      if (!node.SubscribeRaw(topic, cb)) {
        ignerr << "Error subscribing to [" << topic << "]" << std::endl;
        continue;
      }
      // Start tracking stats for this topic.
      rawData[topic] = BasicStats();
    }
  }

  UpdateGUIStats();
  ResetStats();
  prevTopics = topics;
}

void TopicsStats::UpdateGUIStats() {
  std::map<std::string, BasicStats> afterFilterData;
  std::for_each(rawData.cbegin(), rawData.cend(), [&](const std::pair<std::string, BasicStats> _topicStats) {
    if (_topicStats.first.find(topicFilter) != std::string::npos) {
      afterFilterData[_topicStats.first] = _topicStats.second;
    }
  });

  displayedTopicData.clear();
  for (const auto& topicData : afterFilterData) {
    displayedTopicData.append(QString::fromStdString(topicData.first));
    displayedTopicData.append(QString::number(topicData.second.numMessages));
    displayedTopicData.append(
        QString::fromStdString(ToStringWithPrecision(topicData.second.numMessagesInLastSec, 0, "Hz")));
    displayedTopicData.append(QString::fromStdString(GetBandwidth(topicData.second.numBytesInLastSec)));
  }
  DisplayedTopicDataChanged();
}

void TopicsStats::ResetStats() {
  for (auto& topicPair : rawData) {
    topicPair.second.numMessagesInLastSec = 0;
    topicPair.second.numBytesInLastSec = 0;
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicsStats, ignition::gui::Plugin)
