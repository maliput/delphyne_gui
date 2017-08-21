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

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include <ignition/msgs.hh>

#include "lcm_to_ign_translation.hh"

namespace delphyne {
namespace bridge {

void translateBoxGeometry(drake::lcmt_viewer_geometry_data geometryData,
                          ignition::msgs::Geometry* geometryModel);

void translateSphereGeometry(drake::lcmt_viewer_geometry_data geometryData,
                             ignition::msgs::Geometry* geometryModel);

void translateCylinderGeometry(drake::lcmt_viewer_geometry_data geometryData,
                               ignition::msgs::Geometry* geometryModel);

void translateMeshGeometry(drake::lcmt_viewer_geometry_data geometryData,
                           ignition::msgs::Geometry* geometryModel);

void checkVectorSize(int vectorSize, int expectedSize, std::string fieldName);

//////////////////////////////////////////////////
void lcm_to_ign(drake::lcmt_viewer_draw robotViewerData,
               ignition::msgs::PosesStamped* posesStampedModel) {
  // Check the size of each vector on an lcm_viewer_draw message
  // num_links represents the ammount of links declarated and
  // should be matched by the size of each of the following vectors
  checkVectorSize(robotViewerData.link_name.size(),
                  robotViewerData.num_links, "link_name");
  checkVectorSize(robotViewerData.robot_num.size(),
                  robotViewerData.num_links, "robot_num");
  checkVectorSize(robotViewerData.position.size(),
                  robotViewerData.num_links, "position");
  checkVectorSize(robotViewerData.quaternion.size(),
                  robotViewerData.num_links, "quaternion");

  // Convert from milliseconds to seconds
  int64_t sec = robotViewerData.timestamp / 1000;
  // Convert the remainder of division above to nanoseconds
  int64_t nsec = robotViewerData.timestamp % 1000 * 1000000;
  // Set timestamp
  posesStampedModel->mutable_time()->set_sec(sec);
  posesStampedModel->mutable_time()->set_nsec(nsec);

  // Add one pose per link
  for (int i = 0; i < robotViewerData.num_links; ++i) {
    ignition::msgs::Pose* currentPose = posesStampedModel->add_pose();

    currentPose->set_name(robotViewerData.link_name[i]);
    currentPose->set_id(robotViewerData.robot_num[i]);
    // Check position size and translate
    checkVectorSize(robotViewerData.position[i].size(), 3,
                    "position[" + std::to_string(i) + "]");
    lcm_to_ign(robotViewerData.position[i].data(), currentPose->mutable_position());
    // Check orientation size and translate
    checkVectorSize(robotViewerData.quaternion[i].size(), 4,
                    "quaternion[" + std::to_string(i) + "]");
    lcm_to_ign(robotViewerData.quaternion[i].data(),
               currentPose->mutable_orientation());
  }
}

//////////////////////////////////////////////////
void lcm_to_ign(drake::lcmt_viewer_load_robot robotData,
               ignition::msgs::Model* robotModel) {
  for (int i = 0; i < robotData.num_links; ++i) {
    lcm_to_ign(robotData.link[i], robotModel->add_link());
  }
}

//////////////////////////////////////////////////
void lcm_to_ign(drake::lcmt_viewer_link_data linkData,
               ignition::msgs::Link* linkModel) {
  linkModel->set_name(linkData.name);
  linkModel->set_id(linkData.robot_num);
  for (int i = 0; i < linkData.num_geom; ++i) {
    lcm_to_ign(linkData.geom[i], linkModel->add_visual());
  }
}

//////////////////////////////////////////////////
void lcm_to_ign(drake::lcmt_viewer_geometry_data geometryData,
               ignition::msgs::Visual* visualModel) {
  auto* poseMsg = visualModel->mutable_pose();
  auto* materialMsg = visualModel->mutable_material();

  lcm_to_ign(geometryData.position, poseMsg->mutable_position());
  lcm_to_ign(geometryData.quaternion, poseMsg->mutable_orientation());
  lcm_to_ign(geometryData.color, materialMsg->mutable_diffuse());
  lcm_to_ign(geometryData, visualModel->mutable_geometry());
}

//////////////////////////////////////////////////
void lcm_to_ign(float positionData[3],
               ignition::msgs::Vector3d* positionModel) {
  positionModel->set_x(positionData[0]);
  positionModel->set_y(positionData[1]);
  positionModel->set_z(positionData[2]);
}

//////////////////////////////////////////////////
void lcm_to_ign(float quaternionData[4],
               ignition::msgs::Quaternion* quaternionModel) {
  quaternionModel->set_x(quaternionData[0]);
  quaternionModel->set_y(quaternionData[1]);
  quaternionModel->set_z(quaternionData[2]);
  quaternionModel->set_w(quaternionData[3]);
}

//////////////////////////////////////////////////
void lcm_to_ign(float colorData[4], ignition::msgs::Color* colorModel) {
  colorModel->set_r(colorData[0]);
  colorModel->set_g(colorData[1]);
  colorModel->set_b(colorData[2]);
  colorModel->set_a(colorData[3]);
}

//////////////////////////////////////////////////
void lcm_to_ign(drake::lcmt_viewer_geometry_data geometryData,
               ignition::msgs::Geometry* geometryModel) {
  switch (geometryData.type) {
    case geometryData.BOX:
      translateBoxGeometry(geometryData, geometryModel);
      break;
    case geometryData.SPHERE:
      translateSphereGeometry(geometryData, geometryModel);
      break;
    case geometryData.CYLINDER:
      translateCylinderGeometry(geometryData, geometryModel);
      break;
    case geometryData.MESH:
      translateMeshGeometry(geometryData, geometryModel);
      break;
    default:
      std::map<int, std::string> unsupportedGeometries;
      unsupportedGeometries[geometryData.CAPSULE] = "CAPSULE";
      unsupportedGeometries[geometryData.ELLIPSOID] = "ELLIPSOID";
      std::string type = std::to_string(geometryData.type);
      if (unsupportedGeometries.find(geometryData.type) !=
          unsupportedGeometries.end()) {
        type = unsupportedGeometries[geometryData.type];
      }
      throw TranslateException("Geometry of type: " + type +
                               " is currently unsupported");
      break;
  }
}

//////////////////////////////////////////////////
void translateBoxGeometry(drake::lcmt_viewer_geometry_data geometryData,
                          ignition::msgs::Geometry* geometryModel) {
  if (geometryData.num_float_data != 3) {
    std::stringstream message;
    message << "Wrong float_data information for box: expecting 3 elements but "
            << geometryData.num_float_data << " given.";
    throw TranslateException(message.str());
  }

  auto* boxMsg = geometryModel->mutable_box();
  auto* sizeMsg = boxMsg->mutable_size();

  geometryModel->set_type(ignition::msgs::Geometry::BOX);
  sizeMsg->set_x(geometryData.float_data[0]);
  sizeMsg->set_y(geometryData.float_data[1]);
  sizeMsg->set_z(geometryData.float_data[2]);
}

//////////////////////////////////////////////////
void translateSphereGeometry(drake::lcmt_viewer_geometry_data geometryData,
                             ignition::msgs::Geometry* geometryModel) {
  if (geometryData.num_float_data != 1) {
    std::stringstream message;
    message << "Wrong float_data information for sphere: "
            << "expecting 1 element but " << geometryData.num_float_data
            << " given.";
    throw TranslateException(message.str());
  }

  auto* sphereMsg = geometryModel->mutable_sphere();

  geometryModel->set_type(ignition::msgs::Geometry::SPHERE);
  sphereMsg->set_radius(geometryData.float_data[0]);
}

//////////////////////////////////////////////////
void translateCylinderGeometry(drake::lcmt_viewer_geometry_data geometryData,
                               ignition::msgs::Geometry* geometryModel) {
  if (geometryData.num_float_data != 2) {
    std::stringstream message;
    message << "Wrong float_data information for cylinder: "
            << "expecting 2 elements but " << geometryData.num_float_data
            << " given.";
    throw TranslateException(message.str());
  }
  auto* cylinderMsg = geometryModel->mutable_cylinder();

  geometryModel->set_type(ignition::msgs::Geometry::CYLINDER);
  cylinderMsg->set_radius(geometryData.float_data[0]);
  cylinderMsg->set_length(geometryData.float_data[1]);
}

//////////////////////////////////////////////////
void translateMeshGeometry(drake::lcmt_viewer_geometry_data geometryData,
                           ignition::msgs::Geometry* geometryModel) {
  // If no string_data, we assume MeshMessage, which is unsupported
  if (geometryData.string_data.empty()) {
    throw TranslateException(
        "Meshes generated from array are currently unsupported");
  } else {
    auto* meshMsh = geometryModel->mutable_mesh();

    geometryModel->set_type(ignition::msgs::Geometry::MESH);
    meshMsh->set_filename(geometryData.string_data);

    if (geometryData.num_float_data == 3) {
      auto* scale_msg = meshMsh->mutable_scale();
      scale_msg->set_x(geometryData.float_data[0]);
      scale_msg->set_y(geometryData.float_data[1]);
      scale_msg->set_z(geometryData.float_data[2]);
    }
  }
}

void checkVectorSize(int vectorSize, int expectedSize,
                     std::string fieldName) {
  if (vectorSize != expectedSize) {
    std::stringstream message;
    message << "Wrong size for " << fieldName << ": expecting "
            << expectedSize << " elements but " << vectorSize << " given.";
    throw TranslateException(message.str());
  }
}

}  // namespace bridge
}  // namespace delphyne
