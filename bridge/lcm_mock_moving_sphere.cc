// Copyright 2017 Open Source Robotics Foundation
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <ignition/common/Console.hh>

#include <lcm/lcm-cpp.hpp>
#include "drake/lcmt_viewer_draw.hpp"
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_link_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"

const double pi = std::acos(-1);

int main(int argc, char* argv[]) {
  ignition::common::Console::SetVerbosity(3);
  ignmsg << "Starting up sphere mock publisher" << std::endl;

  lcm::LCM lcm;
  if (!lcm.good()) {
    ignerr << "Failed to initialize LCM" << std::endl;
    return 1;
  }
  // Define sphere geometry message
  drake::lcmt_viewer_geometry_data sphereMsg;
  sphereMsg.type = sphereMsg.SPHERE;
  sphereMsg.num_float_data = 1;
  sphereMsg.float_data.resize(sphereMsg.num_float_data);
  sphereMsg.float_data[0] = 1;
  sphereMsg.position[0] = 3;
  sphereMsg.position[1] = 3;
  sphereMsg.position[2] = 1;
  sphereMsg.quaternion[0] = 1.0;
  sphereMsg.quaternion[1] = 0.0;
  sphereMsg.quaternion[2] = 0.0;
  sphereMsg.quaternion[3] = 0.0;
  sphereMsg.color[0] = 1.0;
  sphereMsg.color[1] = 1.0;
  sphereMsg.color[2] = 1.0;
  sphereMsg.color[3] = 1.0;

  // Define sphere link message
  drake::lcmt_viewer_link_data sphereLinkMsg;
  sphereLinkMsg.name = "sphere";
  sphereLinkMsg.robot_num = 0;
  sphereLinkMsg.num_geom = 1;
  sphereLinkMsg.geom.resize(sphereLinkMsg.num_geom);
  sphereLinkMsg.geom[0] = sphereMsg;

  // Define robot message
  drake::lcmt_viewer_load_robot robotMsg;
  robotMsg.num_links = 1;
  robotMsg.link.resize(robotMsg.num_links);
  robotMsg.link[0] = sphereLinkMsg;

  // Define draw message
  drake::lcmt_viewer_draw drawMsg;
  drawMsg.timestamp = 0.0;
  drawMsg.num_links = robotMsg.num_links;
  drawMsg.link_name.resize(1);
  drawMsg.robot_num.resize(1);
  drawMsg.position.resize(1);
  drawMsg.quaternion.resize(1);
  drawMsg.position[0].resize(3);
  drawMsg.quaternion[0].resize(4);
  drawMsg.link_name[0] = sphereLinkMsg.name;
  drawMsg.position[0][0] = 3;
  drawMsg.position[0][1] = 3;
  drawMsg.position[0][2] = 1;
  drawMsg.quaternion[0][0] = 1.0;
  drawMsg.quaternion[0][1] = 0.0;
  drawMsg.quaternion[0][2] = 0.0;
  drawMsg.quaternion[0][3] = 0.0;

  // Load sphere model into drake
  std::string lcm_channel = "DRAKE_VIEWER_LOAD_ROBOT";
  ignmsg << "Publishing message into " << lcm_channel << std::endl;
  if (lcm.publish(lcm_channel, &robotMsg) == -1) {
    ignerr << "Failed to publish message into " << lcm_channel << std::endl;
    return 1;
  }

  // Update sphere position every 10ms
  int timestep_ms = 10;
  while (1) {
    drawMsg.timestamp = drawMsg.timestamp + timestep_ms;

    // Circumference definition
    float radius = 3.0;
    std::vector<float> center = {0.0, 0.0, radius};

    // Update each position by moving 1 degree along the previously defined
    // circumference
    for (int i = 0; i < 360; i++) {
      drawMsg.position[0][0] = center[0] + radius * std::sin(i * 2 * pi / 360);
      drawMsg.position[0][2] = center[2] + radius * std::cos(i * 2 * pi / 360);

      ignmsg << "Publishing message into DRAKE_VIEWER_DRAW" << std::endl;
      if (lcm.publish("DRAKE_VIEWER_DRAW", &drawMsg) == -1) {
        ignerr << "Failed to publish message into DRAKE_VIEWER_DRAW"
               << std::endl;
        return 1;
      }
      // Wait 10ms
      std::this_thread::sleep_for(std::chrono::milliseconds(timestep_ms));
    }
  }
  return 0;
}
