#include "bridge/ign_to_lcm_translation.hh"
#include <iostream>
#include <gtest/gtest.h>
#include <ignition/msgs.hh>

namespace delphyne {
namespace bridge {

//////////////////////////////////////////////////
/// \brief Test that the ignition AutomotiveDrivingCommand
/// message is properly translated to its LCM counterpart
GTEST_TEST(ignToLcm, TestAutomotiveDrivingCommand) {
  // Define the ignition command
  ignition::msgs::AutomotiveDrivingCommand ign_driving_msg;
  // Define LCM expected message
  drake::lcmt_driving_command_t lcm_driving_msg;
  // Fill LCM data
  ign_driving_msg.mutable_time()->set_sec(123);
  ign_driving_msg.mutable_time()->set_nsec(987222111);
  ign_driving_msg.set_theta(0.12);
  ign_driving_msg.set_acceleration(15.7);

  // Translate from ignition to LCM
  ignToLcm(ign_driving_msg, &lcm_driving_msg);

  // Verify generated LCM message
  ASSERT_EQ(lcm_driving_msg.timestamp, 123987);
  ASSERT_EQ(lcm_driving_msg.steering_angle, 0.12);
  ASSERT_EQ(lcm_driving_msg.acceleration, 15.7);
}

}  // namespace bridge
}  // namespace delphyne
