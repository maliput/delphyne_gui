// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_DISPLAYS_ORIGINDISPLAY_HH
#define DELPHYNE_GUI_DISPLAYS_ORIGINDISPLAY_HH

#include <memory>
#include <string>

#include <ignition/gui/qt.h>
#include <ignition/gui/DisplayPlugin.hh>
#include <ignition/msgs.hh>

namespace delphyne
{
namespace gui
{
namespace display_plugins
{
  /// \brief Display the realtime factor on an Ignition Rendering scene.
  class OriginDisplay : public ignition::gui::DisplayPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: OriginDisplay();

    /// \brief Destructor
    public: virtual ~OriginDisplay();

    // Documentation inherited
    public: virtual void Initialize(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    public: virtual std::string Type() const override;
  };
}
}
}

#endif
