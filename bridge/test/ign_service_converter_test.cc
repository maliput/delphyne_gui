#include <iostream>
#include <gtest/gtest.h>
#include <ignition/msgs.hh>
#include <ignition/transport.hh>

#include "bridge/drake/lcmt_viewer_command.hpp"
#include "bridge/ign_service_converter.hh"

// LCM entry point
#include "lcm/lcm-cpp.hpp"


namespace delphyne {
namespace bridge {


class LCMHandler {
 public:
  ~LCMHandler() {}
  int handlerCounter = 0;
  drake::lcmt_viewer_command responseMsg;

  void handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan,
                     const drake::lcmt_viewer_command* msg) {
    handlerCounter++;
    responseMsg = *msg;
  }
};

//////////////////////////////////////////////////
/// \brief Test an end to end service to lcm msg conversion
GTEST_TEST(IgnServiceToLCMChannelTest, TestConversionEndToEnd) {
  // Reset handler flag

  // Create an ignition transport node.
  ignition::transport::Node node;
  // Create empty request and response messages for ignition service
  ignition::msgs::Empty request;
  ignition::msgs::Boolean response;
  // Ignition service name
  std::string notifierServiceName = "/test_service";
  std::string lcmChannelName = "DRAKE_VIEWER_STATUS";

  // Create an lcm instance
  std::shared_ptr<lcm::LCM> lcm = std::make_shared<lcm::LCM>();
  // Subscribe lcm handler to channel
  LCMHandler handlerObject;
  lcm->subscribe(lcmChannelName, &LCMHandler::handleMessage, &handlerObject);

  // Start ignition service to lcm channel converter
  delphyne::bridge::IgnitionServiceConverter<ignition::msgs::Empty,
                                             drake::lcmt_viewer_command>
      ignToLcmRepublisher(lcm, notifierServiceName, lcmChannelName);
  ignToLcmRepublisher.Start();

  bool serviceResult;
  unsigned int timeout = 500;
  // Request the republisher service.
  node.Request(notifierServiceName, request, timeout, response, serviceResult);

  // Wait for lcm message up to 500 millis before timeout
  lcm->handleTimeout(500);

  // Check handler has been called once
  EXPECT_EQ(1, handlerObject.handlerCounter);
  // Check msg content
  EXPECT_EQ(0, handlerObject.responseMsg.command_type);
  EXPECT_EQ("successfully loaded robot", handlerObject.responseMsg.command_data);
}

}  // namespace bridge
}  // namespace delphyne
