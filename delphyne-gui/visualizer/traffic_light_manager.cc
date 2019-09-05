#include "traffic_light_manager.hh"

#include <ignition/common/Console.hh>
#include <ignition/math/Color.hh>
#include <maliput/api/lane.h>
#include <maliput/api/lane_data.h>
#include <maliput/api/rules/traffic_lights.h>

namespace delphyne {
namespace gui {

TrafficLightManager::TrafficLightManager(ignition::rendering::ScenePtr& _scene) {
  for (unsigned int bulb_index = 0; bulb_index < kAmountOfColors; ++bulb_index) {
    bulb_materials[bulb_index] = _scene->CreateMaterial();
  }
  SetBulbMaterialColors();
}

void TrafficLightManager::CreateTrafficLights(ignition::rendering::ScenePtr& _scene,
                                              const std::vector<maliput::api::rules::TrafficLight>& _traffic_lights) {
  traffic_lights.reserve(_traffic_lights.size());
  for (const maliput::api::rules::TrafficLight& traffic_light : _traffic_lights) {
    CreateSingleTrafficLight(_scene, traffic_light);
  }
}

void TrafficLightManager::Clear(ignition::rendering::ScenePtr& _scene) {
  blinking_bulbs.clear();
  for (const auto& mesh : traffic_lights) {
    for (const auto& bulb_group : mesh.second.bulb_groups) {
      for (const auto& bulb_mesh : bulb_group.second) {
        _scene->RootVisual()->RemoveChild(bulb_mesh.second);
      }
    }
    _scene->RootVisual()->RemoveChild(mesh.second.visual);
  }
  traffic_lights.clear();
}

void TrafficLightManager::BlinkBulbs(bool on) {
  const double alpha = on ? 1.0 : 0.5;
  for (ignition::rendering::VisualPtr& bulb : blinking_bulbs) {
    ignition::rendering::MaterialPtr mat = bulb->Material();
    ignition::math::Color ambient = mat->Ambient();
    ambient.A() = alpha;
    mat->SetAmbient(ambient);
    // TODO: Figure out why we need to set (and clone) the same material again for ignition rendering to pick up
    // new shader's attributes.
    bulb->SetMaterial(mat);
  }
}

void TrafficLightManager::SetBulbMaterialColors() {
  ignition::rendering::MaterialPtr& red_material = GetRedMaterial();
  ignition::rendering::MaterialPtr& green_material = GetGreenMaterial();
  ignition::rendering::MaterialPtr& yellow_material = GetYellowMaterial();

  red_material->SetDiffuse(255.0, 0.0, 0.0, 1.0);
  red_material->SetAmbient(255.0, 0.0, 0.0, 1.0);

  green_material->SetDiffuse(0.0, 255.0, 0.0, 1.0);
  green_material->SetAmbient(0.0, 255.0, 0.0, 1.0);

  yellow_material->SetDiffuse(255.0, 255.0, 0.0, 1.0);
  yellow_material->SetAmbient(255.0, 255.0, 0.0, 1.0);
}

void TrafficLightManager::CreateSingleTrafficLight(ignition::rendering::ScenePtr& _scene,
                                                   const maliput::api::rules::TrafficLight& _traffic_light) {
  const maliput::api::GeoPosition& traffic_light_world_position = _traffic_light.position_road_network();
  const maliput::api::Rotation& traffic_light_world_rotation = _traffic_light.orientation_road_network();

  TrafficLightManager::TrafficLightMesh traffic_light_mesh;
  traffic_light_mesh.bulb_groups.reserve(_traffic_light.bulb_groups().size());
  traffic_light_mesh.visual = _scene->CreateVisual();
  traffic_light_mesh.visual->AddGeometry(_scene->CreateBox());
  traffic_light_mesh.visual->SetVisible(false);
  traffic_light_mesh.visual->SetWorldPosition(traffic_light_world_position.x(), traffic_light_world_position.y(),
                                              traffic_light_world_position.z());
  traffic_light_mesh.visual->SetWorldRotation(traffic_light_world_rotation.roll(), traffic_light_world_rotation.pitch(),
                                              traffic_light_world_rotation.yaw());
  _scene->RootVisual()->AddChild(traffic_light_mesh.visual);

  for (const maliput::api::rules::BulbGroup& bulb_group : _traffic_light.bulb_groups()) {
    CreateBulbGroup(_scene, &traffic_light_mesh, bulb_group, traffic_light_world_position,
                    traffic_light_world_rotation);
  }

  traffic_lights[_traffic_light.id()] = std::move(traffic_light_mesh);
}

void TrafficLightManager::CreateBulbGroup(ignition::rendering::ScenePtr& _scene,
                                          TrafficLightManager::TrafficLightMesh* traffic_light_mesh,
                                          const maliput::api::rules::BulbGroup& _bulb_group,
                                          const maliput::api::GeoPosition& traffic_light_world_position,
                                          const maliput::api::Rotation& traffic_light_world_rotation) {
  const maliput::api::GeoPosition& bulb_group_world_position = maliput::api::GeoPosition::FromXyz(
      traffic_light_world_position.xyz() + _bulb_group.position_traffic_light().xyz());
  const maliput::api::Rotation& bulb_group_world_rotation = maliput::api::Rotation::FromQuat(
      traffic_light_world_rotation.quat() * (_bulb_group.orientation_traffic_light().quat()));

  TrafficLightManager::BulbMeshes bulb_meshes;
  bulb_meshes.reserve(_bulb_group.bulbs().size());

  for (const maliput::api::rules::Bulb bulb : _bulb_group.bulbs()) {
    CreateSingleBulb(_scene, &bulb_meshes, bulb, bulb_group_world_position, bulb_group_world_rotation);
  }
  traffic_light_mesh->bulb_groups[_bulb_group.id()] = std::move(bulb_meshes);
}

void TrafficLightManager::CreateSingleBulb(ignition::rendering::ScenePtr& _scene, BulbMeshes* bulb_group,
                                           const maliput::api::rules::Bulb& _single_bulb,
                                           const maliput::api::GeoPosition& bulb_group_world_position,
                                           const maliput::api::Rotation& bulb_group_world_rotation) {
  const maliput::api::Rotation& bulb_rotation = maliput::api::Rotation::FromQuat(
      bulb_group_world_rotation.quat() * (_single_bulb.orientation_bulb_group().quat()));
  ignition::rendering::VisualPtr visual = _scene->CreateVisual();
  if (_single_bulb.type() == maliput::api::rules::BulbType::kRound) {
    visual->AddGeometry(_scene->CreateSphere());
  } else {
    // TODO: Set the proper orientation for the "arrow" bulb.
    visual->AddGeometry(_scene->CreateCone());
  }
  const maliput::api::rules::Bulb::BoundingBox& bb = _single_bulb.bounding_box();
  visual->SetWorldScale(bb.p_BMax.x(), bb.p_BMax.y(), bb.p_BMax.z());
  const maliput::api::GeoPosition bulb_world_position =
      maliput::api::GeoPosition::FromXyz(bulb_group_world_position.xyz() + _single_bulb.position_bulb_group().xyz());
  visual->SetWorldPosition(bulb_world_position.x(), bulb_world_position.y(), bulb_world_position.z());
  visual->SetWorldRotation(bulb_rotation.roll(), bulb_rotation.pitch(), bulb_rotation.yaw());
  const size_t bulb_color_index = static_cast<size_t>(_single_bulb.color());
  visual->SetMaterial(bulb_materials[bulb_color_index]);
  _scene->RootVisual()->AddChild(visual);
  if (_single_bulb.GetDefaultState() == maliput::api::rules::BulbState::kBlinking) {
    blinking_bulbs.push_back(visual);
  }
  (*bulb_group)[_single_bulb.id()] = visual;
}

}  // namespace gui
}  // namespace delphyne