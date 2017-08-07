#pragma once

#include <ignition/msgs.hh>
#include <ignition/transport.hh>
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"
#include "lcm/lcm-cpp.hpp"

#include "lcm_to_ign_translator.h"

namespace delphyne {
namespace bridge {

// LcmChannelRepeater listens to all the messages that arrive to
// an LCM channel, translates them into their ign-msgs counterpart
// and re-publishes them into an ign-transport topic of the same
// name.
template <class LCM_TYPE, class IGN_TYPE>
class LcmChannelRepeater {
 public:
  LcmChannelRepeater(lcm::LCM* lcm, const std::string& topic_name)
      : lcm_(lcm), topic_name_(topic_name) {}

  void Start() {
    publisher_ = node_.Advertise<IGN_TYPE>("/" + topic_name_);

    if (!lcm_->good()) {
      throw std::runtime_error("LCM is not ready");
    }

    if (!publisher_) {
      throw std::runtime_error("Error advertising topic: " + topic_name_);
    }

    lcm_->subscribe(topic_name_, &LcmChannelRepeater::handleMessage, this);
  }

 private:
  lcm::LCM* lcm_;
  const std::string topic_name_;
  ignition::transport::Node node_;
  ignition::transport::Node::Publisher publisher_;
  delphyne::bridge::LcmToIgnTranslator translator_;

  void handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan,
                     const LCM_TYPE* msg) {
    ignition::msgs::Model* modelMsg = translator_.Translate(*msg);
    publisher_.Publish(*modelMsg);
  }
};
}
}
