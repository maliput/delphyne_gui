// Copyright 2021 Toyota Research Institute
#include "topics_stats.hh"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {
namespace {

template <class T>
std::string ToStringWithPrecision(T _value, int _precision){
  std::stringstream sstr;
  sstr << std::fixed << std::setprecision(_precision) << _value;
  return sstr.str();
}

bool IsSubStr(const std::string & _mainStr, const std::string &_subStr){
  const std::string::size_type n = _mainStr.find(_subStr);
  return n == std::string::npos ? false : true;
}

} // namespace

TopicsStats::TopicsStats() : Plugin() { timer.start(kTimerPeriodInMs, this); }

void TopicsStats::LoadConfig(const tinyxml2::XMLElement * /*_pluginElem*/) {
  if (this->title.empty()) this->title = "Topic stats";
}

QStringList TopicsStats::Data() const {
  return data;
}

void TopicsStats::SetData(const QStringList& _Data){
  data = _Data;
  DataChanged();
}

void TopicsStats::SearchTopic(const QString& _topic){
  topicFilter = _topic.toStdString();
}


void TopicsStats::timerEvent(QTimerEvent* _event) {
  rawData = std::map<std::string, std::map<std::string, std::string>> {
    {"/topic4/from/cpp", {{"messages" , std::to_string(counter)}, {"frequency", ToStringWithPrecision(counter*2., 2)}, {"bandwidth", ToStringWithPrecision(counter*0.5, 2)}}},
    {"/topic2/from/cpp", {{"messages" , std::to_string(counter)}, {"frequency", ToStringWithPrecision(counter*2., 2)}, {"bandwidth", ToStringWithPrecision(counter*0.5, 2)}}},
    {"/topic1/from/cpp", {{"messages" , std::to_string(counter)}, {"frequency", ToStringWithPrecision(counter*2., 2)}, {"bandwidth", ToStringWithPrecision(counter*0.5, 2)}}},
    {"/topic3/from/cpp", {{"messages" , std::to_string(counter)}, {"frequency", ToStringWithPrecision(counter*2., 2)}, {"bandwidth", ToStringWithPrecision(counter*0.5, 2)}}},
  };

  std::map<std::string, std::map<std::string, std::string>> afterFilterData;
  std::for_each(rawData.cbegin(), rawData.cend(), [&afterFilterData, &topicFilter = this->topicFilter](const std::pair<std::string, std::map<std::string, std::string>> _topicStats){
    if(IsSubStr(_topicStats.first, topicFilter)){
      afterFilterData[_topicStats.first] = _topicStats.second;
    }
  });

  data.clear();
  for (const auto& row : afterFilterData) {
    data.append(QString::fromStdString(row.first));
    data.append(QString::fromStdString(row.second.at("messages")));
    data.append(QString::fromStdString(row.second.at("frequency")));
    data.append(QString::fromStdString(row.second.at("bandwidth")));
  }

  counter++;
  DataChanged();
}

} // namespace gui
} // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicsStats, ignition::gui::Plugin)
