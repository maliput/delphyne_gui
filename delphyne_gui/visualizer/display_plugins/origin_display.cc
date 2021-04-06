// Copyright 2021 Toyota Research Institute
#include "origin_display.hh"

#include <cmath>

#include <ignition/common/Console.hh>
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

  // Get the render engine.
  // Note: we don't support other engines than Ogre.
  auto engine = ignition::rendering::engine(kEngineName);
  if (!engine) {
    ignerr << "Engine \"" << kEngineName << "\" not supported, origin display plugin won't work." << std::endl;
    return;
  }
  // Get the scene.
  auto scene = engine->SceneByName(sceneName);
  if (!scene) {
    ignwarn << "Scene \"" << sceneName << "\" not found, origin display plugin won't work until the scene is created."
            << " Trying again in " << kTimerPeriodInMs << "ms" << std::endl;
    timer.start(kTimerPeriodInMs, this);
    return;
  }
  DrawAxes(scene);
}

void OriginDisplay::timerEvent(QTimerEvent* _event) {
  if (_event->timerId() != timer.timerId()) {
    return;
  }

  // Get the render engine.
  // Note: we don't support other engines than Ogre.
  auto engine = ignition::rendering::engine(kEngineName);
  auto scene = engine->SceneByName(sceneName);
  if (!scene) {
    ignwarn << "Scene \"" << sceneName << "\" not found yet. Trying again in "
            << " Trying again in " << kTimerPeriodInMs << "ms" << std::endl;
    return;
  }
  timer.stop();
  DrawAxes(scene);
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
