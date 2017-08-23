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

#include <ignition/common/Console.hh>

// Drake LCM message headers
#include "drake/lcmt_driving_command_t.hpp"
#include "drake/lcmt_viewer_draw.hpp"
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"

// Custom ignition message headers
#include "protobuf/headers/automotive_driving_command.pb.h"

// Repeater classes
#include "ign_topic_repeater.hh"
#include "lcm_channel_repeater.hh"

// LCM entry point
#include "lcm/lcm-cpp.hpp"

// Runs a program that listens for new messages in a set
// of Drake LCM channels and converts them to ign-messages
// to be consumed by the front end.
int main(int argc, char* argv[]) {
  ignition::common::Console::SetVerbosity(3);
  ignmsg << "LCM to ignition-transport bridge 0.1.0" << std::endl;

  std::shared_ptr<lcm::LCM> lcm = std::make_shared<lcm::LCM>();

  // Create a repeater on DRAKE_VIEWER_LOAD_ROBOT channel, translating
  // from drake::lcmt_viewer_load_robot to ignition::msgs::Model
  delphyne::bridge::LcmChannelRepeater<drake::lcmt_viewer_load_robot,
                                       ignition::msgs::Model>
      viewerLoadRobotRepeater(lcm, "DRAKE_VIEWER_LOAD_ROBOT");

  // Create a repeater on DRAKE_VIEWER_DRAW channel, translating
  // from drake::lcmt_viewer_draw to ignition::msgs::PosesStamped
  delphyne::bridge::LcmChannelRepeater<drake::lcmt_viewer_draw,
                                       ignition::msgs::PosesStamped>
      viewerDrawRepeater(lcm, "DRAKE_VIEWER_DRAW");

  // Start DRAKE_VIEWER_LOAD_ROBOT repeater
  try {
    viewerLoadRobotRepeater.Start();
  } catch (const std::runtime_error& error) {
    ignerr << "Failed to start LCM channel repeater for "
           << "DRAKE_VIEWER_LOAD_ROBOT" << std::endl;
    ignerr << "Details: " << error.what() << std::endl;
    exit(1);
  }
  // Start DRAKE_VIEWER_DRAW repeater
  try {
    viewerDrawRepeater.Start();
  } catch (const std::runtime_error& error) {
    ignerr << "Failed to start LCM channel repeater for "
           << "DRAKE_VIEWER_DRAW" << std::endl;
    ignerr << "Details: " << error.what() << std::endl;
    exit(1);
  }

  // Create a repeater on ignition DRIVING_COMMAND_0 topic, translating
  // from ignition::msgs::AutomotiveDrivingCommand to
  // drake::lcmt_driving_command_t
  delphyne::bridge::IgnTopicRepeater<ignition::msgs::AutomotiveDrivingCommand,
                                     drake::lcmt_driving_command_t>
      drivingCommandRepeater(lcm, "DRIVING_COMMAND_0");

  // Start DRIVING_COMMAND_0 repeater
  try {
    drivingCommandRepeater.Start();
  } catch (const std::runtime_error& error) {
    ignerr << "Failed to start ignition channel repeater for "
           << "DRIVING_COMMAND_0" << std::endl;
    ignerr << "Details: " << error.what() << std::endl;
    exit(1);
  }

  while (true) {
    lcm->handle();
  }

  return 0;
}
