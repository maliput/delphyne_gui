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
/// \brief To be called before a test, clears the handlers
/// called flags
void reset() { lcmHandlerCalled = false; }

GTEST_TEST(ignServiceToLCMChannelTest, TestRepublishingEndToEnd) {
  // Reset handler flag
  reset();

  // Create an ignition transport node.
  std::shared_ptr<ignition::transport::Node> node =
      std::make_shared<ignition::transport::Node>();
  // Create empty request and response messages for ignition service
  ignition::msgs::Empty req;
  ignition::msgs::StringMsg rep;
  // Ignition service name
  std::string notifierServiceName = "/visualizer_start_notifier";
  std::string lcmChannelName = "DRAKE_VIEWER_STATUS";

  // Create an lcm instance
  std::shared_ptr<lcm::LCM> lcm = std::make_shared<lcm::LCM>();
  // Subscribe lcm handler to channel
  LCMHandler handlerObject;
  lcm->subscribe(lcmChannelName, &LCMHandler::handleMessage, &handlerObject);

  // Start republishing into LCM
  auto ignToLcmRepublisher = delphyne::bridge::IgnitionToLCMRepublisher(
      notifierServiceName, node, lcmChannelName, lcm);
  ignToLcmRepublisher.start();

  bool srv_result;
  unsigned int timeout = 5000;
  // Request the republisher service.
  node->Request(notifierServiceName, req, timeout, rep, srv_result);

  // Wait up to 500 milis before timeout
  lcm->handleTimeout(500);

  // Check handler has been called
  ASSERT_TRUE(lcmHandlerCalled);
}

GTEST_TEST(ignServiceToLCMChannelTest, tRepublishingEndToEnd) {}

}  // namespace bridge
}  // namespace delphyne
