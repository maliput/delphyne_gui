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

#ifndef DELPHYNE_BRIDGE_LCMTOIGNTRANSLATION_HH_
#define DELPHYNE_BRIDGE_LCMTOIGNTRANSLATION_HH_

#include <ignition/msgs.hh>
#include "drake/lcmt_viewer_geometry_data.hpp"
#include "drake/lcmt_viewer_load_robot.hpp"

namespace delphyne {
namespace bridge {

class TranslateException : public std::runtime_error {
  public: TranslateException(std::string message) :std::runtime_error(message) {}
};

/// \brief Translate a whole robot model definition from LCM to ignition
/// \param[in]  robot_data  An LCM message containing the robot data
/// \param[out] robot_model The resulting ignition message with the robot model
void translate(drake::lcmt_viewer_load_robot robot_data,
               ignition::msgs::Model* robot_model);

/// \brief Translate a link definition from LCM to ignition
/// \param[in]  link_data  An LCM message containing the link data
/// \param[out] link_model The resulting ignition message with the link model
void translate(drake::lcmt_viewer_link_data link_data,
               ignition::msgs::Link* link_model);

/// \brief Translate a geometry and visual definition from LCM to ignition
/// \param[in]  geometry_data  An LCM message containing the geometry data
/// \param[out] visual_model The resulting ignition message including the
/// geometry and visual properties
void translate(drake::lcmt_viewer_geometry_data geometry_data,
               ignition::msgs::Visual* visual_model);

/// \brief Translate a geometry definition from LCM to ignition
/// \param[in]  geometry_data  An LCM message containing the geometry data
/// \param[out] visual_model The resulting ignition message including the
/// geometry model
void translate(drake::lcmt_viewer_geometry_data geometry_data,
               ignition::msgs::Geometry* geometry_model);

/// \brief Translate a position definition from LCM to ignition
/// \param[in]  position_data  An LCM message containing the position data
/// \param[out] position_model The resulting ignition message with the position
/// model
void translate(float position_data[3],
               ignition::msgs::Vector3d* position_model);

/// \brief Translate an orientation definition from LCM to ignition
/// \param[in]  quaternion_data  An LCM message containing the orientation data
/// \param[out] quaternion_model The resulting ignition message with the
/// orientation model
void translate(float quaternion_data[4],
               ignition::msgs::Quaternion* quaternion_model);

/// \brief Translate an color definition from LCM to ignition
/// \param[in]  color_data  An LCM message containing the color data
/// \param[out] color_model The resulting ignition message with the color model
void translate(float color_data[4], ignition::msgs::Color* color_model);

}  // namespace bridge
}  // namespace delphyne

#endif
