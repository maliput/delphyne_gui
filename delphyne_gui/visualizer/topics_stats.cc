// Copyright 2021 Toyota Research Institute
#include "topics_stats.hh"

#include <ignition/plugin/Register.hh>

namespace delphyne {
namespace gui {

TopicsStats::TopicsStats() : Plugin() {}

void TopicsStats::LoadConfig(const tinyxml2::XMLElement * /*_pluginElem*/) {
  if (this->title.empty()) this->title = "Topic stats";
}

} // namespace gui
} // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::TopicsStats, ignition::gui::Plugin)
