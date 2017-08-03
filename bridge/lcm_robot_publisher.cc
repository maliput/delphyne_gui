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
  drake::lcmt_viewer_geometry_data geometryMsg;
  geometryMsg.type = geometryMsg.BOX;
  geometryMsg.num_float_data = 3;
  geometryMsg.float_data.resize(geometryMsg.num_float_data);
  geometryMsg.float_data[0] = 3.8808;
  geometryMsg.float_data[1] = 0.75;
  geometryMsg.float_data[2] = 0.030921;

  // Generate link message
  drake::lcmt_viewer_link_data linkMsg;
  linkMsg.name = "chassis";
  linkMsg.robot_num = 0;
  linkMsg.num_geom = 1;
  linkMsg.geom.resize(linkMsg.num_geom);
  linkMsg.geom[0] = geometryMsg;

  // Generate robot message
  drake::lcmt_viewer_load_robot robotMsg;
  robotMsg.num_links = 1;
  robotMsg.link.resize(robotMsg.num_links);
  robotMsg.link[0] = linkMsg;

  // Publish robot messages at 1Hz.
  while (1) {
    lcm.publish("DRAKE_VIEWER_LOAD_ROBOT", &robotMsg);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
