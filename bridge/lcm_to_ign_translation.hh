#ifndef DELPHYNE_BRIDGE_LCMTOIGNTRANSLATOR_HH_
#define DELPHYNE_BRIDGE_LCMTOIGNTRANSLATOR_HH_

#include <ignition/msgs.hh>
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"

namespace delphyne {
namespace bridge {

ignition::msgs::Model* Translate(drake::lcmt_viewer_load_robot robot_data);

ignition::msgs::Link* Translate(drake::lcmt_viewer_link_data link_data);

ignition::msgs::Visual* Translate(drake::lcmt_viewer_geometry_data geometry_data);

} // namespace bridge
} // namespace delphyne

#endif

