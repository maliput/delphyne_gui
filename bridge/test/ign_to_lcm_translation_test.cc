#include "bridge/ign_to_lcm_translation.hh"
#include <iostream>
#include <gtest/gtest.h>
#include <ignition/msgs.hh>

namespace delphyne {
namespace bridge {

//////////////////////////////////////////////////
/// \brief Test 
GTEST_TEST(DrivingCommandTest, TestSteering) {
  // Define LCM mesh geometry message
  drake::lcmt_driving_command_t driving_msg;
  // Define the expected ignition geometry
  ignition::msgs::CmdVel2D cmd_vel_msg;
  // Fill LCM data
  driving_msg.steering_angle = 2.54;
  driving_msg.acceleration = 5.26;

  // Translate from LCM to ignition
  ign_to_lcm(driving_msg, &cmd_vel_msg);

  // Verify generated ignition geometry
  ASSERT_TRUE(cmd_vel_msg.has_theta());
}


}  // namespace bridge
}  // namespace delphyne

