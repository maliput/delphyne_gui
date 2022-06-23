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
#include "origin_display.hh"

#include <cmath>

#include <ignition/common/Console.hh>
#include <ignition/gui/Application.hh>
#include <ignition/gui/GuiEvents.hh>
#include <ignition/gui/MainWindow.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/Visual.hh>

namespace delphyne {
namespace gui {

void OriginDisplay::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  title = "Origin Display";

  if (_pluginElem) {
    // Update the requested scene name even it fails to load.
    if (auto elem = _pluginElem->FirstChildElement("scene")) {
      sceneName = elem->GetText();
    }
    // Similarly with the visibility flag.
    if (auto elem = _pluginElem->FirstChildElement("visible")) {
      elem->QueryBoolText(&isVisible);
      IsVisibleChanged();
    }
  }
  // Install event filter.
  ignition::gui::App()->findChild<ignition::gui::MainWindow*>()->installEventFilter(this);

  // Set timer to get the scene.
  timer.start(kTimerPeriodInMs, this);
}

bool OriginDisplay::eventFilter(QObject* _obj, QEvent* _event) {
  // Hooking to the Render event to safely make rendering calls.
  // See https://github.com/ignitionrobotics/ign-gui/blob/ign-gui3/include/ignition/gui/GuiEvents.hh#L36-L37
  if (_event->type() == ignition::gui::events::Render::kType) {
    if (scene != nullptr) {
      if (!areAxesDrawn) {
        this->DrawAxes(scene);
        areAxesDrawn = true;
      }
      if (isDirty) {
        ChangeAxesVisibility();
        isDirty = false;
      }
    }
  }
  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

void OriginDisplay::timerEvent(QTimerEvent* _event) {
  if (_event->timerId() != timer.timerId()) {
    return;
  }

  // Get the render engine.
  // Note: we don't support other engines than Ogre.
  auto engine = ignition::rendering::engine(kEngineName);
  scene = engine->SceneByName(sceneName);
  if (!scene) {
    ignwarn << "Scene \"" << sceneName << "\" not found yet. Trying again in "
            << " Trying again in " << kTimerPeriodInMs << "ms" << std::endl;
    return;
  }
  timer.stop();
}

void OriginDisplay::SetIsVisible(bool _isVisible) {
  isVisible = _isVisible;
  IsVisibleChanged();
  isDirty = true;
}

void OriginDisplay::DrawAxes(ignition::rendering::ScenePtr scene) {
  constexpr double kAxisRadius{0.02};
  constexpr double kAxisLength{10000.0};
  constexpr double kAxisHalfLength{kAxisLength / 2.0};

  // Construct the visual.
  auto visual = scene->CreateVisual();
  scene->RootVisual()->AddChild(visual);

  // Create the visual axes.
  for (auto& axis : axes) {
    axis = scene->CreateVisual();
    if (!axis) {
      ignerr << "Failed to create axis visual" << std::endl;
      return;
    }
    axis->SetLocalScale(kAxisRadius, kAxisRadius, kAxisLength);
    axis->AddGeometry(scene->CreateCylinder());
  }

  // X axis
  const ignition::math::Pose3d kAxisPoseX(kAxisHalfLength /* x */, 0. /* y */, 0. /* z */, 0. /* roll */,
                                          M_PI_2 /* pitch */, 0. /* yaw */);
  axes[0]->SetLocalPose(kAxisPoseX);
  axes[0]->SetMaterial("Default/TransRed");

  // Y axis
  const ignition::math::Pose3d kAxisPoseY(0. /* x */, kAxisHalfLength /* y */, 0. /* z */, M_PI_2 /* roll */,
                                          0. /* pitch */, 0. /* yaw */);
  axes[1]->SetLocalPose(kAxisPoseY);
  axes[1]->SetMaterial("Default/TransGreen");

  // Z axis
  const ignition::math::Pose3d kAxisPoseZ(0. /* x */, 0. /* y */, kAxisHalfLength /* z */, 0. /* roll */,
                                          0. /* pitch */, 0. /* yaw */);
  axes[2]->SetLocalPose(kAxisPoseZ);
  axes[2]->SetMaterial("Default/TransBlue");

  // Add the three visual axes.
  for (auto& axis : axes) {
    visual->AddChild(axis);
  }

  // Set the visibility of the visuals
  ChangeAxesVisibility();
}

void OriginDisplay::ChangeAxesVisibility() {
  const bool newIsVisibleValue = isVisible;
  if (axes[0] != nullptr) {
    for (auto& axis : axes) {
      axis->SetVisible(newIsVisibleValue);
    }
  }
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::OriginDisplay, ignition::gui::Plugin)
