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
  drake::lcmt_driving_command_t lcm_driving_msg;
  // Define the expected ignition geometry
  ignition::msgs::AutomotiveDrivingCommand ign_driving_msg;
  // Fill LCM data
  lcm_driving_msg.steering_angle = 2.54;
  lcm_driving_msg.acceleration = 5.26;

  // Translate from LCM to ignition
  ignToLcm(lcm_driving_msg, &ign_driving_msg);

  // Verify generated ignition geometry
  ASSERT_TRUE(ign_driving_msg.has_theta());
}


}  // namespace bridge
}  // namespace delphyne

