#include <ignition/msgs.hh>

#include "lcm_to_ign_translator.h"

namespace delphyne {
namespace bridge {

ignition::msgs::Model* LcmToIgnTranslator::Translate(
    drake::lcmt_viewer_load_robot robot_data) {
  auto* modelMsg = new ignition::msgs::Model();
  for (int i = 0; i < robot_data.num_links; i = i + 1) {
    ::ignition::msgs::Link* linkMsg = this->Translate(robot_data.link[i]);
    modelMsg->mutable_link()->AddAllocated(linkMsg);
  }
  return modelMsg;
}

ignition::msgs::Link* LcmToIgnTranslator::Translate(
    drake::lcmt_viewer_link_data link_data) {
  auto* linkMsg = new ignition::msgs::Link();
  linkMsg->set_name(link_data.name);
  for (int i = 0; i < link_data.num_geom; i = i + 1) {
    ::ignition::msgs::Visual* visualMsg = this->Translate(link_data.geom[i]);
    linkMsg->mutable_visual()->AddAllocated(visualMsg);
  }
  return linkMsg;
}

ignition::msgs::Visual* LcmToIgnTranslator::Translate(
    drake::lcmt_viewer_geometry_data geometry_data) {
  auto* visualMsg = new ignition::msgs::Visual();
  auto* poseMsg = visualMsg->mutable_pose();
  auto* materialMsg = visualMsg->mutable_material();

  poseMsg->set_allocated_position(
      this->TranslatePosition(geometry_data.position));
  poseMsg->set_allocated_orientation(
      this->TranslateOrientation(geometry_data.quaternion));

  materialMsg->set_allocated_diffuse(this->TranslateColor(geometry_data.color));

  visualMsg->set_allocated_geometry(
      this->TranslateGeometryShape(geometry_data));

  return visualMsg;
}

ignition::msgs::Vector3d* LcmToIgnTranslator::TranslatePosition(
    float position_data[3]) {
  auto* positionMsg = new ignition::msgs::Vector3d();
  positionMsg->set_x(position_data[0]);
  positionMsg->set_y(position_data[1]);
  positionMsg->set_z(position_data[2]);
  return positionMsg;
}

ignition::msgs::Quaternion* LcmToIgnTranslator::TranslateOrientation(
    float position_data[4]) {
  auto* orientationMsg = new ignition::msgs::Quaternion();
  orientationMsg->set_x(position_data[0]);
  orientationMsg->set_y(position_data[1]);
  orientationMsg->set_z(position_data[2]);
  orientationMsg->set_w(position_data[3]);
  return orientationMsg;
}

ignition::msgs::Color* LcmToIgnTranslator::TranslateColor(float color_data[4]) {
  auto* colorMsg = new ignition::msgs::Color();
  colorMsg->set_r(color_data[0]);
  colorMsg->set_g(color_data[1]);
  colorMsg->set_b(color_data[2]);
  colorMsg->set_a(color_data[3]);
  return colorMsg;
}

ignition::msgs::Geometry* LcmToIgnTranslator::TranslateGeometryShape(
    drake::lcmt_viewer_geometry_data geometry_data) {
  if (geometry_data.type == geometry_data.BOX) {
    return this->TranslateBoxGeometry(geometry_data);
  } else {
    // TODO(basicNew): Once we have written all the shape translations we will
    // throw an exception here if there is no match in the switch statement
    return NULL;
  }
}

ignition::msgs::Geometry* LcmToIgnTranslator::TranslateBoxGeometry(
    drake::lcmt_viewer_geometry_data geometry_data) {
  auto* geometryMsg = new ignition::msgs::Geometry();
  auto* boxMsg = geometryMsg->mutable_box();
  auto* sizeMsg = boxMsg->mutable_size();

  geometryMsg->set_type(ignition::msgs::Geometry::BOX);

  sizeMsg->set_x(geometry_data.float_data[0]);
  sizeMsg->set_y(geometry_data.float_data[1]);
  sizeMsg->set_z(geometry_data.float_data[2]);

  return geometryMsg;
}
}
}
