#pragma once

#include <ignition/msgs.hh>
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"

namespace delphyne {
namespace bridge {

/// LcmToIgnTranslator implements the methods required to convert LCM messages
/// into ign-message ones. As the bridge implementation grows and we need to
/// accommodate more channels, new translation methods will be added.
///
class LcmToIgnTranslator {
 public:
  ignition::msgs::Model* Translate(drake::lcmt_viewer_load_robot robot_data);

  ignition::msgs::Link* Translate(drake::lcmt_viewer_link_data link_data);

  ignition::msgs::Visual* Translate(
      drake::lcmt_viewer_geometry_data geometry_data);

 private:
  ignition::msgs::Color* TranslateColor(float color_data[4]);

  ignition::msgs::Vector3d* TranslatePosition(float position_data[3]);

  ignition::msgs::Quaternion* TranslateOrientation(float position_data[4]);

  ignition::msgs::Geometry* TranslateGeometryShape(
      drake::lcmt_viewer_geometry_data geometry_data);

  ignition::msgs::Geometry* TranslateBoxGeometry(
      drake::lcmt_viewer_geometry_data geometry_data);
};
}
}
