#ifndef DELPHYNE_BRIDGE_LCMTOIGNTRANSLATOR_HH_
#define DELPHYNE_BRIDGE_LCMTOIGNTRANSLATOR_HH_

#include <ignition/msgs.hh>
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"

namespace delphyne {
namespace bridge {

void Translate(drake::lcmt_viewer_load_robot robot_data,
               ignition::msgs::Model* robot_model);

void Translate(drake::lcmt_viewer_link_data link_data,
               ignition::msgs::Link* link_model);

void Translate(drake::lcmt_viewer_geometry_data geometry_data,
               ignition::msgs::Visual* visual_model);

void Translate(float position_data[3],
               ignition::msgs::Vector3d* position_model);

void Translate(float quaternion_data[4],
               ignition::msgs::Quaternion* quaternion_model);

void Translate(float color_data[4], ignition::msgs::Color* color_model);

void Translate(drake::lcmt_viewer_geometry_data geometry_data,
               ignition::msgs::Geometry* geometry_model);

}  // namespace bridge
}  // namespace delphyne

#endif
