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

static bool lcmHandlerCalled;

class LCMHandler {
 public:
  ~LCMHandler() {}

  void handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan,
                     const drake::lcmt_viewer_command* msg) {
    lcmHandlerCalled = true;
  }
};

//////////////////////////////////////////////////
/// \brief To be called before a test, clears the handler
void reset() {
  lcmHandlerCalled = false;
}

//////////////////////////////////////////////////
/// \brief Test an end to end service to lcm msg conversion
GTEST_TEST(IgnServiceToLCMChannelTest, TestConversionEndToEnd) {
  // Reset handler flag
  reset();

  // Create an ignition transport node.
  std::shared_ptr<ignition::transport::Node> node =
      std::make_shared<ignition::transport::Node>();
  // Create empty request and response messages for ignition service
  ignition::msgs::Empty req;
  ignition::msgs::StringMsg rep;
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
      ignToLcmRepublisher(node, notifierServiceName, lcm, lcmChannelName);
  ignToLcmRepublisher.Start();

  bool srv_result;
  unsigned int timeout = 500;
  // Request the republisher service.
  node->Request(notifierServiceName, req, timeout, rep, srv_result);

  // Wait for lcm message up to 500 millis before timeout
  lcm->handleTimeout(500);

  // Check handler has been called
  ASSERT_TRUE(lcmHandlerCalled);
}

}  // namespace bridge
}  // namespace delphyne
