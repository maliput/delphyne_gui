#include <iostream>
#include <gtest/gtest.h>
#include <ignition/msgs.hh>
#include <ignition/transport.hh>

#include "bridge/drake/lcmt_viewer_command.hpp"
#include "bridge/service_to_channel_translation.hh"

// LCM entry point
#include "lcm/lcm-cpp.hpp"
#include "lcm/lcm_coretypes.h"


namespace delphyne {
namespace bridge {

//////////////////////////////////////////////////
/// \brief Test an end to end service to lcm msg conversion
GTEST_TEST(ServiceToChannelTranslationTest, TestConversionToLCMViewerCommand) {
    ignition::msgs::Empty req;
    drake::lcmt_viewer_command msg = delphyne::bridge::convertServiceToMsg(req);
    ASSERT_EQ(msg.command_type, 0);
    ASSERT_EQ(msg.command_data, "successfully loaded robot");
}


}  // namespace bridge
}  // namespace delphyne

