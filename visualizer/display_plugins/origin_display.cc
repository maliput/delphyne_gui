// Copyright 2018 Toyota Research Institute

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering/Text.hh>

#include "origin_display.hh"

using namespace delphyne;
using namespace gui;
using namespace display_plugins;

/////////////////////////////////////////////////
OriginDisplay::OriginDisplay() : DisplayPlugin() { this->title = "Origin"; }

/////////////////////////////////////////////////
OriginDisplay::~OriginDisplay() {}

/////////////////////////////////////////////////
void OriginDisplay::Initialize(const tinyxml2::XMLElement* /*_pluginElem*/) {
  const double kAxisRadius = 0.02;
  const double kAxisLength = 10000;
  const double kAxisHalfLength = kAxisLength / 2.0;
  if (auto scenePtr = this->Scene().lock()) {
    // Create the visual axes.
    std::array<ignition::rendering::VisualPtr, 3> axes;
    for (auto& axis : axes) {
      axis = scenePtr->CreateVisual();
      if (!axis) {
        ignerr << "Failed to create axis visual" << std::endl;
        return;
      }
      axis->SetLocalScale(kAxisRadius, kAxisRadius, kAxisLength);
      axis->AddGeometry(scenePtr->CreateCylinder());
    }

    const ignition::math::Pose3d kAxisPoseX(kAxisHalfLength, 0, 0, 0, IGN_PI_2,
                                            0);
    axes[0]->SetLocalPose(kAxisPoseX);
    axes[0]->SetMaterial("Default/TransRed");
    const ignition::math::Pose3d kAxisPoseY(0, kAxisHalfLength, 0, IGN_PI_2, 0,
                                            0);
    axes[1]->SetLocalPose(kAxisPoseY);
    axes[1]->SetMaterial("Default/TransGreen");
    const ignition::math::Pose3d kAxisPoseZ(0, 0, kAxisHalfLength, 0, 0, 0);
    axes[2]->SetLocalPose(kAxisPoseZ);
    axes[2]->SetMaterial("Default/TransBlue");

    for (auto& axis : axes) {
      this->Visual()->AddChild(axis);
    }
  } else {
    ignerr << "Scene invalid. Origin display not initialized." << std::endl;
    return;
  }
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::display_plugins::OriginDisplay,
                                  ignition::gui::DisplayPlugin)
