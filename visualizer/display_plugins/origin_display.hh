/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef DELPHYNE_GUI_DISPLAYS_ORIGINDISPLAY_HH
#define DELPHYNE_GUI_DISPLAYS_ORIGINDISPLAY_HH

#include <memory>

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
  };
}
}
}

#endif
