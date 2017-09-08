#include <gtest/gtest.h>
#include <ignition/msgs.hh>
#include <ignition/transport.hh>
#include <lcm/lcm-cpp.hpp>

#include "bridge/drake/lcmt_driving_command_t.hpp"
#include "bridge/protobuf/headers/automotive_driving_command.pb.h"
#include "bridge/repeater_factory.hh"
#include "bridge/repeater_manager.hh"

namespace delphyne {
namespace bridge {

std::shared_ptr<delphyne::bridge::AbstractRepeater> fakeFactoryFunction(
    std::shared_ptr<lcm::LCM> lcm, const std::string& topicName) {
  return std::make_shared<delphyne::bridge::IgnTopicRepeater<
      ignition::msgs::AutomotiveDrivingCommand, drake::lcmt_driving_command_t>>(
      lcm, topicName);
}

//////////////////////////////////////////////////
/// \brief Test that false is returned if the manager can't start the service
/// because there is no registered type
GTEST_TEST(RepeaterManager, TestReturnFalseOnFailure) {
  // Setup LCM
  std::shared_ptr<lcm::LCM> lcm = std::make_shared<lcm::LCM>();

  // Service name
  std::string serviceName = "/repeat_topic";

  // Define and start the manager
  delphyne::bridge::RepeaterManager manager(lcm, serviceName);

  manager.Start();

  // Perform the service request and check the response
  ignition::transport::Node node;

  ignition::msgs::StringMsg_V request;
  request.add_data("nonExistentType");
  request.add_data("SomeTopic");

  ignition::msgs::Boolean response;
  bool result;
  unsigned int timeout = 500;

  node.Request(serviceName, request, timeout, response, result);

  ASSERT_TRUE(result);
  ASSERT_FALSE(response.data());
}

//////////////////////////////////////////////////
/// \brief Test that true is returned if the manager can successfully  start
/// the repeater
GTEST_TEST(RepeaterManager, TestReturnTrueOnRegistration) {
  // Setup LCM
  std::shared_ptr<lcm::LCM> lcm = std::make_shared<lcm::LCM>();

  // Register a fake factory
  RepeaterFactory::Register("fakeType", fakeFactoryFunction);

  // Service name
  std::string serviceName = "/repeat_topic";

  // Define and start the manager
  delphyne::bridge::RepeaterManager manager(lcm, serviceName);

  manager.Start();

  // Perform the service request and check the response
  ignition::transport::Node node;

  ignition::msgs::StringMsg_V request;
  request.add_data("SomeTopic");
  request.add_data("fakeType");

  ignition::msgs::Boolean response;
  bool result;
  unsigned int timeout = 500;

  node.Request(serviceName, request, timeout, response, result);

  ASSERT_TRUE(result);
  ASSERT_TRUE(response.data());
}

}  // namespace bridge
}  // namespace delphyne
