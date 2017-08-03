#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <thread>

#include "lcmtypes/lcmt_viewer_geometry_data.hpp"
#include "lcmtypes/lcmt_viewer_link_data.hpp"
#include "lcmtypes/lcmt_viewer_load_robot.hpp"

// Publishes a defined lcmt_viewer_load_robot
// message into the DRAKE_VIEWER_LOAD_ROBOT
// channel every one second
int main(int argc, char *argv[]) {
  lcm::LCM lcm;
  if (!lcm.good())
    return 1;

  // Generate geometry message
  drake::lcmt_viewer_geometry_data geometry;
  geometry.type = geometry.BOX;
  geometry.num_float_data = 3;
  geometry.float_data.resize(geometry.num_float_data);
  geometry.float_data[0] = 3.8808;
  geometry.float_data[1] = 0.75;
  geometry.float_data[2] = 0.030921;

  // Generate link message
  drake::lcmt_viewer_link_data link;
  link.name = "chassis";
  link.robot_num = 0;
  link.num_geom = 1;
  link.geom.resize(link.num_geom);
  link.geom[0] = geometry;

  // Generate robot message
  drake::lcmt_viewer_load_robot robot_data;
  robot_data.num_links = 1;
  robot_data.link.resize(robot_data.num_links);
  robot_data.link[0] = link;

  // Publish robot messages at 1Hz.
  while (1) {
    lcm.publish("DRAKE_VIEWER_LOAD_ROBOT", &robot_data);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
