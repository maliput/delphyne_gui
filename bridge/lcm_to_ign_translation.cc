#include <ignition/msgs.hh>

#include "lcm_to_ign_translation.hh"

namespace delphyne {
namespace bridge {

ignition::msgs::Vector3d* TranslatePosition(float position_data[3]);

ignition::msgs::Quaternion* TranslateOrientation(float position_data[4]);

ignition::msgs::Color* TranslateColor(float color_data[4]);

ignition::msgs::Geometry* TranslateGeometryShape(drake::lcmt_viewer_geometry_data geometry_data);

ignition::msgs::Geometry* TranslateBoxGeometry(drake::lcmt_viewer_geometry_data geometry_data);

ignition::msgs::Model* Translate(
    drake::lcmt_viewer_load_robot robot_data) {
  auto modelMsgPtr = new ignition::msgs::Model();
  for (int i = 0; i < robot_data.num_links; i = i + 1) {
    ignition::msgs::Link* linkMsgPtr = Translate(robot_data.link[i]);
    modelMsgPtr->mutable_link()->AddAllocated(linkMsgPtr);
  }
  return modelMsgPtr;
}

ignition::msgs::Link* Translate(
    drake::lcmt_viewer_link_data link_data) {
  auto linkMsgPtr = new ignition::msgs::Link();
  linkMsgPtr->set_name(link_data.name);
  for (int i = 0; i < link_data.num_geom; i = i + 1) {
    ignition::msgs::Visual* visualMsgPtr = Translate(link_data.geom[i]);
    linkMsgPtr->mutable_visual()->AddAllocated(visualMsgPtr);
  }
  return linkMsgPtr;
}

ignition::msgs::Visual* Translate(
    drake::lcmt_viewer_geometry_data geometry_data) {
  auto visualMsgPtr = new ignition::msgs::Visual();
  auto* poseMsg = visualMsgPtr->mutable_pose();
  auto* materialMsg = visualMsgPtr->mutable_material();

  poseMsg->set_allocated_position(
      TranslatePosition(geometry_data.position));
  poseMsg->set_allocated_orientation(
      TranslateOrientation(geometry_data.quaternion));

  materialMsg->set_allocated_diffuse(TranslateColor(geometry_data.color));

  visualMsgPtr->set_allocated_geometry(
      TranslateGeometryShape(geometry_data));

  return visualMsgPtr;
}

ignition::msgs::Vector3d* TranslatePosition(
    float position_data[3]) {
  auto* positionPtr = new ignition::msgs::Vector3d();
  positionPtr->set_x(position_data[0]);
  positionPtr->set_y(position_data[1]);
  positionPtr->set_z(position_data[2]);
  return positionPtr;
}

ignition::msgs::Quaternion* TranslateOrientation(
    float position_data[4]) {
  auto* orientationMsg = new ignition::msgs::Quaternion();
  orientationMsg->set_x(position_data[0]);
  orientationMsg->set_y(position_data[1]);
  orientationMsg->set_z(position_data[2]);
  orientationMsg->set_w(position_data[3]);
  return orientationMsg;
}

ignition::msgs::Color* TranslateColor(float color_data[4]) {
  auto* colorMsg = new ignition::msgs::Color();
  colorMsg->set_r(color_data[0]);
  colorMsg->set_g(color_data[1]);
  colorMsg->set_b(color_data[2]);
  colorMsg->set_a(color_data[3]);
  return colorMsg;
}

ignition::msgs::Geometry* TranslateGeometryShape(
    drake::lcmt_viewer_geometry_data geometry_data) {
  if (geometry_data.type == geometry_data.BOX) {
    return TranslateBoxGeometry(geometry_data);
  } else {
    // TODO(basicNew): Once we have written all the shape translations we will
    // throw an exception here if there is no match in the switch statement
    return NULL;
  }
}

ignition::msgs::Geometry* TranslateBoxGeometry(
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
