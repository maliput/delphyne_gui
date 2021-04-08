// Copyright 2021 Toyota Research Institute
#include "topics_stats.hh"

#include <iomanip>
#include <iostream>
#include <sstream>

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

// \Returns True when @p _subStr is contained in _mainStr.
bool IsSubStr(const std::string& _mainStr, const std::string& _subStr) {
  const std::string::size_type n = _mainStr.find(_subStr);
  return n == std::string::npos ? false : true;
}

// Get a string containing the bandwith and its correspondant unit.
// \param _bytesLastSec Number of bytes in the a second.
std::string GetBandwidth(uint64_t _bytesInASec) {
  std::string units;
  if (_bytesInASec < 1000) {
    units = "B/s";
  } else if (_bytesInASec < 1000000) {
    _bytesInASec /= 1000.0;
    units = "KB/s";
  } else {
    _bytesInASec /= 1000000.0;
    units = "MB/s";
  }
  return ToStringWithPrecision(_bytesInASec, 2, units);
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
    std::cerr << "Unable to find stats for [" << topic << "]" << std::endl;
    return;
  }

  // Update the total number of messages received.
  statsPair->second.numMessages++;

  // Update the number of messages received during the last second.
  statsPair->second.numMessagesLastSec++;

  // Update the number of bytes received during the last second.
  statsPair->second.numBytesLastSec += _size;
}

QStringList TopicsStats::Data() const { return data; }

void TopicsStats::SetData(const QStringList& _Data) {
  data = _Data;
  DataChanged();
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
    auto topic = topics.at(i);
    if (std::find(prevTopics.begin(), prevTopics.end(), topic) == prevTopics.end()) {
      // Subscribe to the topic.
      auto cb =
          std::bind(&TopicsStats::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      if (!node.SubscribeRaw(topic, cb)) {
        std::cerr << "Error subscribing to [" << topic << "]" << std::endl;
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
  std::for_each(
      rawData.cbegin(), rawData.cend(),
      [&afterFilterData, &topicFilter = this->topicFilter](const std::pair<std::string, BasicStats> _topicStats) {
        if (IsSubStr(_topicStats.first, topicFilter)) {
          afterFilterData[_topicStats.first] = _topicStats.second;
        }
      });

  data.clear();
  for (const auto& topicData : afterFilterData) {
    data.append(QString::fromStdString(topicData.first));
    data.append(QString::number(topicData.second.numMessages));
    data.append(QString::fromStdString(ToStringWithPrecision(topicData.second.numMessagesLastSec, 0, "Hz")));
    data.append(QString::fromStdString(GetBandwidth(topicData.second.numBytesLastSec)));
  }
  DataChanged();
}

void TopicsStats::ResetStats() {
  for (auto& topicPair : rawData) {
    topicPair.second.numMessagesLastSec = 0;
    topicPair.second.numBytesLastSec = 0;
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicsStats, ignition::gui::Plugin)
