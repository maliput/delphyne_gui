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

#ifndef DELPHYNE_BRIDGE_LCMCHANNELREPEATER_HH_
#define DELPHYNE_BRIDGE_LCMCHANNELREPEATER_HH_

#include <string>
#include <ignition/msgs.hh>
#include <ignition/transport.hh>
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"
#include "lcm/lcm-cpp.hpp"

#include "lcm_to_ign_translation.hh"

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

  void handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan,
                     const LCM_TYPE* lcm_msg) {
    IGN_TYPE ign_msg;
    Translate(*lcm_msg, &ign_msg);
    publisher_.Publish(ign_msg);
  }
};

}  // namespace bridge
}  // namespace delphyne

#endif
