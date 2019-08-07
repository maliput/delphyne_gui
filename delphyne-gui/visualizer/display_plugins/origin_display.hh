// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_DISPLAYS_ORIGINDISPLAY_HH
#define DELPHYNE_GUI_DISPLAYS_ORIGINDISPLAY_HH

#include <memory>

#include <ignition/gui/qt.h>
#include <ignition/gui/DisplayPlugin.hh>

namespace delphyne {
namespace gui {
namespace display_plugins {
/// \brief Display the origin on an Ignition Rendering scene.
class OriginDisplay : public ignition::gui::DisplayPlugin {
  Q_OBJECT

  /// \brief Constructor
 public:
  OriginDisplay();

  /// \brief Destructor
 public:
  virtual ~OriginDisplay();

  // Documentation inherited
 public:
  virtual void Initialize(const tinyxml2::XMLElement* _pluginElem) override;
};
}  // namespace display_plugins
}  // namespace gui
}  // namespace delphyne

#endif
