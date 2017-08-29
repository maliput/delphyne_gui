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

#ifndef DELPHYNE_BRIDGE_IGNSERVICELCMCHANNEL_HH_
#define DELPHYNE_BRIDGE_IGNSERVICELCMCHANNEL_HH_

#include <functional>
#include <string>
#include <ignition/msgs.hh>
#include <ignition/transport.hh>
#include <lcm/lcm-cpp.hpp>
#include "drake/lcmt_viewer_command.hpp"
#include "service_to_channel_translation.hh"

namespace delphyne {
namespace bridge {

template <class IGN_REQ_TYPE, class LCM_TYPE>
class IgnitionServiceConverter {
 public:
  //////////////////////////////////////////////////
  /// \brief Creates an ignition service converter
  IgnitionServiceConverter(std::shared_ptr<ignition::transport::Node> ignNode,
                           const std::string& notifierServiceName,
                           std::shared_ptr<lcm::LCM> lcm,
                           const std::string& channelName)
      : ignNode_(ignNode),
        ignServiceName_(notifierServiceName),
        lcm_(lcm),
        lcmChannelName_(channelName) {}

  //////////////////////////////////////////////////
  /// \brief Start advertising the ignition service call
  void Start() {
    if (!lcm_->good()) {
      throw std::runtime_error("LCM is not ready");
    }

    // Advertise a service call.
    if (!ignNode_->Advertise(ignServiceName_,
                             &IgnitionServiceConverter::srvConverterHandler,
                             this)) {
      std::stringstream errorMsg;
      errorMsg << "Error advertising service [" << ignServiceName_ << "]" << std::endl;
      throw std::runtime_error(errorMsg.str());
    }
  }

 private:
  /// \internal
  /// \brief The ignition node
  std::shared_ptr<ignition::transport::Node> ignNode_;

  /// \internal
  /// \brief The ignition service listened
  const std::string ignServiceName_;

  /// \internal
  /// \brief The LCM manager
  std::shared_ptr<lcm::LCM> lcm_;

  /// \internal
  /// \brief The topic this converter publishes to
  const std::string lcmChannelName_;

  //////////////////////////////////////////////////
  /// \brief Service handler function, this will call a
  /// convertServiceToMsg overloaded function and publish the
  /// result into an lcm channel
  void srvConverterHandler(const IGN_REQ_TYPE& _req,
                           ignition::msgs::StringMsg& _rep, bool& _result) {
    // Using an Empty msg as _rep won't work, although it works as a _req
    // So we are using an empty string as a response.
    _rep.set_data("");

    LCM_TYPE msg = delphyne::bridge::convertServiceToMsg(_req);
    if (lcm_->publish(lcmChannelName_, &msg) == -1) {
      // The response failed
      _result = false;
    } else {
      // The response succeed
      _result = true;
    }
  }
};

}  // namespace bridge
}  // namespace delphyne #endif

#endif