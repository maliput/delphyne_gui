// Copyright 2021 Toyota Research Institute
#pragma once

#include "ignition/gui/Plugin.hh"

namespace delphyne {
namespace gui {

/// \brief Show stats of all the topics.
class TopicsStats : public ignition::gui::Plugin {
  Q_OBJECT

  /// \brief Constructor
 public:
  TopicsStats();

  /// \brief Destructor
  virtual ~TopicsStats() = default;

  // Documentation inherited
  virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

};

}  // namespace gui
}  // namespace delphyne
