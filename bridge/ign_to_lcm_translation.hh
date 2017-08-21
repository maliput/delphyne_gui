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

#ifndef DELPHYNE_BRIDGE_IGNTOLCMTRANSLATION_HH_
#define DELPHYNE_BRIDGE_IGNTOLCMTRANSLATION_HH_

#include <ignition/msgs.hh>
#include "drake/lcmt_driving_command_t.hpp"

namespace delphyne {
namespace bridge {

class TranslateException : public std::runtime_error {
 public:
  TranslateException(std::string message) : std::runtime_error(message) {}
};

/// \brief Translate a driving command from LCM to ignition
/// \param[in]  driving_command  An LCM message containing the driving command
/// \param[out] cmd_vel The resulting ignition message of the cmd_vel type
void ign_to_lcm(drake::lcmt_driving_command_t driving_command,
               ignition::msgs::CmdVel2D* cmd_vel);

}  // namespace bridge
}  // namespace delphyne

#endif
