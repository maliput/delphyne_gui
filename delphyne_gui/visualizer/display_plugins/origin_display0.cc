// Copyright 2018 Toyota Research Institute

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering/Text.hh>

#include "origin_display0.hh"

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
  const double kAxisLength = 10000.0;
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
    // X axis
    double xPosition = kAxisHalfLength;
    double yPosition = 0.0;
    double zPosition = 0.0;
    double roll = 0.0;
    double pitch = IGN_PI_2;
    double yaw = 0.0;
    const ignition::math::Pose3d kAxisPoseX(xPosition, yPosition, zPosition, roll, pitch, yaw);
    axes[0]->SetLocalPose(kAxisPoseX);
    axes[0]->SetMaterial("Default/TransRed");

    // Y axis
    xPosition = 0.0;
    yPosition = kAxisHalfLength;
    zPosition = 0.0;
    roll = IGN_PI_2;
    pitch = 0.0;
    yaw = 0.0;
    const ignition::math::Pose3d kAxisPoseY(xPosition, yPosition, zPosition, roll, pitch, yaw);
    axes[1]->SetLocalPose(kAxisPoseY);
    axes[1]->SetMaterial("Default/TransGreen");

    // Z axis
    xPosition = 0.0;
    yPosition = 0.0;
    zPosition = kAxisHalfLength;
    roll = 0.0;
    pitch = 0.0;
    yaw = 0.0;
    const ignition::math::Pose3d kAxisPoseZ(xPosition, yPosition, zPosition, roll, pitch, yaw);
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
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::display_plugins::OriginDisplay, ignition::gui::DisplayPlugin)
