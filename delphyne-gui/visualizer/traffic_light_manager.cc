#include "traffic_light_manager.hh"

#include "delphyne/macros.h"

#include <ignition/common/Console.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Helpers.hh>
#include <maliput/api/lane.h>
#include <maliput/api/lane_data.h>
#include <maliput/api/rules/phase.h>
#include <maliput/api/rules/traffic_lights.h>

namespace delphyne {
namespace gui {

const std::string TrafficLightManager::kGreenMaterialName{"GreenBulb"};
const std::string TrafficLightManager::kGreenBrightMaterialName{"GreenBulbBright"};
const std::string TrafficLightManager::kRedMaterialName{"RedBulb"};
const std::string TrafficLightManager::kRedBrightMaterialName{"RedBulbBright"};
const std::string TrafficLightManager::kYellowMaterialName{"YellowBulb"};
const std::string TrafficLightManager::kYellowBrightMaterialName{"YellowBulbBright"};

TrafficLightManager::TrafficLightManager(ignition::rendering::ScenePtr& _scene) {
  bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kRed)] = _scene->CreateMaterial(kRedMaterialName);
  bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kYellow)] =
      _scene->CreateMaterial(kYellowMaterialName);
  bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kGreen)] =
      _scene->CreateMaterial(kGreenMaterialName);

  bright_bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kRed)] =
      _scene->CreateMaterial(kRedBrightMaterialName);
  bright_bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kYellow)] =
      _scene->CreateMaterial(kYellowBrightMaterialName);
  bright_bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kGreen)] =
      _scene->CreateMaterial(kGreenBrightMaterialName);

  SetBulbMaterialColors();
  ignition::common::MeshManager* meshManager = ignition::common::MeshManager::Instance();
  DELPHYNE_DEMAND(meshManager);
  const ignition::common::Mesh* unit_sphere_mesh = meshManager->MeshByName("unit_sphere");
  const ignition::common::Mesh* unit_box_mesh = meshManager->MeshByName("unit_box");
  DELPHYNE_DEMAND(unit_sphere_mesh);
  DELPHYNE_DEMAND(unit_box_mesh);
  sphere_bulb_aabb_min = unit_sphere_mesh->Min();
  sphere_bulb_aabb_max = unit_sphere_mesh->Max();
  unit_box_aabb_min = unit_box_mesh->Min();
  unit_box_aabb_max = unit_box_mesh->Max();
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
      for (const auto& bulb_mesh : bulb_group.second.bulbs) {
        _scene->RootVisual()->RemoveChild(bulb_mesh.second);
      }
      _scene->RootVisual()->RemoveChild(bulb_group.second.visual);
    }
  }
  traffic_lights.clear();
}

void TrafficLightManager::BlinkBulbs(bool on) {
  for (const auto& bulb : blinking_bulbs) {
    const maliput::api::rules::BulbColor bulb_color = GetBulbColor(bulb.second);
    const size_t color_index = static_cast<size_t>(bulb_color);
    if (on) {
      bulb.second->SetMaterial(bright_bulb_materials[color_index], false);
    } else {
      bulb.second->SetMaterial(bulb_materials[color_index], false);
    }
  }
}

void TrafficLightManager::ChangeBulbState(const maliput::api::rules::BulbStates& bulb_states) {
  for (const auto& state : bulb_states) {
    ignition::rendering::VisualPtr bulb_mesh = GetBulbMesh(state.first);
    if (bulb_mesh) {
      SetBulbMaterial(state.first, bulb_mesh, GetBulbColor(bulb_mesh), state.second);
    }
  }
}

ignition::rendering::VisualPtr TrafficLightManager::GetBulbMesh(
    const maliput::api::rules::UniqueBulbId& bulb_id) const {
  auto traffic_light = traffic_lights.find(bulb_id.traffic_light_id);
  if (traffic_light != traffic_lights.end()) {
    auto bulb_group = traffic_light->second.bulb_groups.find(bulb_id.bulb_group_id);
    if (bulb_group != traffic_light->second.bulb_groups.end()) {
      auto bulb = bulb_group->second.bulbs.find(bulb_id.bulb_id);
      if (bulb != bulb_group->second.bulbs.end()) {
        return bulb->second;
      }
    }
  }
  return ignition::rendering::VisualPtr();
}

maliput::api::rules::BulbColor TrafficLightManager::GetBulbColor(const ignition::rendering::VisualPtr& bulb) const {
  maliput::api::rules::BulbColor color;
  const std::string& material_name = bulb->Material()->Name();
  if (material_name.find(kGreenMaterialName) != std::string::npos) {
    color = maliput::api::rules::BulbColor::kGreen;
  } else if (material_name.find(kYellowMaterialName) != std::string::npos) {
    color = maliput::api::rules::BulbColor::kYellow;
  } else {
    color = maliput::api::rules::BulbColor::kRed;
  }
  return color;
}

void TrafficLightManager::SetBulbMaterialColors() {
  ignition::rendering::MaterialPtr& red_material = GetRedMaterial();
  ignition::rendering::MaterialPtr& green_material = GetGreenMaterial();
  ignition::rendering::MaterialPtr& yellow_material = GetYellowMaterial();

  ignition::rendering::MaterialPtr& red_bright_material = GetBrightRedMaterial();
  ignition::rendering::MaterialPtr& green_bright_material = GetBrightGreenMaterial();
  ignition::rendering::MaterialPtr& yellow_bright_material = GetBrightYellowMaterial();

  red_material->SetDiffuse(60.0, 0.0, 0.0, 1.0);
  red_material->SetAmbient(60.0, 0.0, 0.0, 1.0);
  red_bright_material->SetDiffuse(255.0, 0.0, 0.0, 1.0);
  red_bright_material->SetAmbient(255.0, 0.0, 0.0, 1.0);

  green_material->SetDiffuse(0.0, 60.0, 0.0, 1.0);
  green_material->SetAmbient(0.0, 60.0, 0.0, 1.0);
  green_bright_material->SetDiffuse(0.0, 255.0, 0.0, 1.0);
  green_bright_material->SetAmbient(0.0, 255.0, 0.0, 1.0);

  yellow_material->SetDiffuse(60.0, 60.0, 0.0, 1.0);
  yellow_material->SetAmbient(60.0, 60.0, 0.0, 1.0);
  yellow_bright_material->SetDiffuse(255.0, 255.0, 0.0, 1.0);
  yellow_bright_material->SetAmbient(255.0, 255.0, 0.0, 1.0);
}

void TrafficLightManager::SetBulbMaterial(const maliput::api::rules::UniqueBulbId& unique_bulb_id,
                                          ignition::rendering::VisualPtr& bulb, maliput::api::rules::BulbColor color,
                                          maliput::api::rules::BulbState new_bulb_state) {
  const size_t color_index = static_cast<size_t>(color);
  switch (new_bulb_state) {
    case maliput::api::rules::BulbState::kOff:
      bulb->SetMaterial(bulb_materials[color_index], false);
      RemoveBlinkingLight(unique_bulb_id);
      break;
    case maliput::api::rules::BulbState::kOn:
      bulb->SetMaterial(bright_bulb_materials[color_index], false);
      RemoveBlinkingLight(unique_bulb_id);
      break;
    case maliput::api::rules::BulbState::kBlinking:
      blinking_bulbs[unique_bulb_id] = bulb;
      break;
    default:
      break;
  }
}

void TrafficLightManager::RemoveBlinkingLight(const maliput::api::rules::UniqueBulbId& id) {
  if (blinking_bulbs.find(id) != blinking_bulbs.end()) {
    blinking_bulbs.erase(id);
  }
}

void TrafficLightManager::CreateSingleTrafficLight(ignition::rendering::ScenePtr& _scene,
                                                   const maliput::api::rules::TrafficLight& _traffic_light) {
  const maliput::api::GeoPosition& traffic_light_world_position = _traffic_light.position_road_network();
  const maliput::api::Rotation& traffic_light_world_rotation = _traffic_light.orientation_road_network();

  TrafficLightManager::TrafficLightMesh traffic_light_mesh;
  traffic_light_mesh.bulb_groups.reserve(_traffic_light.bulb_groups().size());

  for (const maliput::api::rules::BulbGroup& bulb_group : _traffic_light.bulb_groups()) {
    CreateBulbGroup(_scene, &traffic_light_mesh, _traffic_light.id(), bulb_group, traffic_light_world_position,
                    traffic_light_world_rotation);
  }

  traffic_lights[_traffic_light.id()] = std::move(traffic_light_mesh);
}

void TrafficLightManager::CreateBulbGroup(ignition::rendering::ScenePtr& _scene,
                                          TrafficLightManager::TrafficLightMesh* traffic_light_mesh,
                                          const maliput::api::rules::TrafficLight::Id& traffic_light_id,
                                          const maliput::api::rules::BulbGroup& _bulb_group,
                                          const maliput::api::GeoPosition& traffic_light_world_position,
                                          const maliput::api::Rotation& traffic_light_world_rotation) {
  const maliput::api::GeoPosition bulb_group_world_position = maliput::api::GeoPosition::FromXyz(
      traffic_light_world_position.xyz() + _bulb_group.position_traffic_light().xyz());
  const maliput::api::Rotation bulb_group_world_rotation = maliput::api::Rotation::FromQuat(
      traffic_light_world_rotation.quat() * (_bulb_group.orientation_traffic_light().quat()));

  TrafficLightManager::BulbMeshes bulb_meshes;
  bulb_meshes.bulbs.reserve(_bulb_group.bulbs().size());
  bulb_meshes.visual = _scene->CreateVisual();
  bulb_meshes.visual->AddGeometry(_scene->CreateBox());
  _scene->RootVisual()->AddChild(bulb_meshes.visual);

  ignition::math::Vector3d box_aabb_min =
      unit_box_aabb_min + ignition::math::Vector3d(bulb_group_world_position.x(), bulb_group_world_position.y(),
                                                   bulb_group_world_position.z());
  ignition::math::Vector3d box_aabb_max =
      unit_box_aabb_max + ignition::math::Vector3d(bulb_group_world_position.x(), bulb_group_world_position.y(),
                                                   bulb_group_world_position.z());

  ignition::math::Vector3d bulb_group_aabb_max(std::numeric_limits<double>::lowest(),
                                               std::numeric_limits<double>::lowest(),
                                               std::numeric_limits<double>::lowest());
  ignition::math::Vector3d bulb_group_aabb_min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                                               std::numeric_limits<double>::max());

  for (const maliput::api::rules::Bulb bulb : _bulb_group.bulbs()) {
    maliput::api::rules::UniqueBulbId unique_bulb_id(traffic_light_id, _bulb_group.id(), bulb.id());
    const maliput::api::rules::Bulb::BoundingBox bulb_bb_world_pos = CreateSingleBulb(
        _scene, &bulb_meshes, unique_bulb_id, bulb, bulb_group_world_position, bulb_group_world_rotation);
    bulb_group_aabb_max = ignition::math::Vector3d(std::max(bulb_group_aabb_max.X(), bulb_bb_world_pos.p_BMax.x()),
                                                   std::max(bulb_group_aabb_max.Y(), bulb_bb_world_pos.p_BMax.y()),
                                                   std::max(bulb_group_aabb_max.Z(), bulb_bb_world_pos.p_BMax.z()));
    bulb_group_aabb_min = ignition::math::Vector3d(std::min(bulb_group_aabb_min.X(), bulb_bb_world_pos.p_BMin.x()),
                                                   std::min(bulb_group_aabb_min.Y(), bulb_bb_world_pos.p_BMin.y()),
                                                   std::min(bulb_group_aabb_min.Z(), bulb_bb_world_pos.p_BMin.z()));
  }

  ignition::math::Vector3d mid_point = (bulb_group_aabb_max + bulb_group_aabb_min) / 2.0;

  bulb_meshes.visual->SetWorldScale(1.9 * std::abs(bulb_group_aabb_max.X() - mid_point.X()),
                                    1.9 * std::abs(bulb_group_aabb_max.Y() - mid_point.Y()),
                                    1.9 * std::abs(bulb_group_aabb_max.Z() - mid_point.Z()));
  bulb_meshes.visual->SetWorldRotation(bulb_group_world_rotation.roll(), bulb_group_world_rotation.pitch(),
                                       bulb_group_world_rotation.yaw());
  bulb_meshes.visual->SetWorldPosition(mid_point);
  bulb_meshes.visual->SetVisible(false);
  traffic_light_mesh->bulb_groups[_bulb_group.id()] = std::move(bulb_meshes);
}

maliput::api::rules::Bulb::BoundingBox TrafficLightManager::CreateSingleBulb(
    ignition::rendering::ScenePtr& _scene, BulbMeshes* bulb_group,
    const maliput::api::rules::UniqueBulbId& unique_bulb_id, const maliput::api::rules::Bulb& _single_bulb,
    const maliput::api::GeoPosition& bulb_group_world_position,
    const maliput::api::Rotation& bulb_group_world_rotation) {
  const maliput::api::rules::Bulb::BoundingBox& bb = _single_bulb.bounding_box();

  ignition::math::Vector3d world_bounding_box_max = sphere_bulb_aabb_max;
  ignition::math::Vector3d world_bounding_box_min = sphere_bulb_aabb_min;

  maliput::api::rules::Bulb::BoundingBox bulb_world_bounding_box;

  bulb_world_bounding_box.p_BMin = Eigen::Vector3d(world_bounding_box_min.X() * std::abs(bb.p_BMin.x()),
                                                   world_bounding_box_min.Y() * std::abs(bb.p_BMin.y()),
                                                   world_bounding_box_min.Z() * std::abs(bb.p_BMin.z()));
  bulb_world_bounding_box.p_BMax =
      Eigen::Vector3d(world_bounding_box_max.X() * bb.p_BMax.x(), world_bounding_box_max.Y() * bb.p_BMax.y(),
                      world_bounding_box_max.Z() * bb.p_BMax.z());

  const maliput::api::Rotation bulb_rotation = maliput::api::Rotation::FromQuat(
      bulb_group_world_rotation.quat() * (_single_bulb.orientation_bulb_group().quat()));
  ignition::rendering::VisualPtr visual = _scene->CreateVisual();
  if (_single_bulb.type() == maliput::api::rules::BulbType::kRound) {
    visual->AddGeometry(_scene->CreateSphere());
  } else {
    // TODO: Set the proper orientation for the "arrow" bulb.
    visual->AddGeometry(_scene->CreateCone());
  }
  visual->SetWorldScale(bb.p_BMax.x(), bb.p_BMax.y(), bb.p_BMax.z());
  const maliput::api::GeoPosition bulb_world_position =
      maliput::api::GeoPosition::FromXyz(bulb_group_world_position.xyz() + _single_bulb.position_bulb_group().xyz());
  visual->SetWorldRotation(bulb_rotation.roll(), bulb_rotation.pitch(), bulb_rotation.yaw());
  visual->SetWorldPosition(bulb_world_position.x(), bulb_world_position.y(), bulb_world_position.z());
  SetBulbMaterial(unique_bulb_id, visual, _single_bulb.color(), _single_bulb.GetDefaultState());
  _scene->RootVisual()->AddChild(visual);
  bulb_group->bulbs[_single_bulb.id()] = visual;

  bulb_world_bounding_box.p_BMax += bulb_world_position.xyz();
  bulb_world_bounding_box.p_BMin += bulb_world_position.xyz();

  return bulb_world_bounding_box;
}

}  // namespace gui
}  // namespace delphyne