// Copyright 2021 Toyota Research Institute
#include "topics_stats.hh"

#include <iostream>

#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {

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


void TopicsStats::timerEvent(QTimerEvent* _event) {
  std::cout << "Timer callback executed: " << std::endl;

  data.clear();
  data.append(QString::fromStdString("/topic1/from/cpp"));
  data.append(QString::number(counter));
  data.append(QString::number(counter*2));
  data.append(QString::number(counter*0.5));
  data.append(QString::fromStdString("/topic2/from/cpp"));
  data.append(QString::number(counter));
  data.append(QString::number(counter*2));
  data.append(QString::number(counter*0.5));
  data.append(QString::fromStdString("/topic3/from/cpp"));
  data.append(QString::number(counter));
  data.append(QString::number(counter*2));
  data.append(QString::number(counter*0.5));
  counter++;
  DataChanged();
}

} // namespace gui
} // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicsStats, ignition::gui::Plugin)
