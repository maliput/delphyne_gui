// Copyright 2019 Toyota Research Institute

#ifndef TRAFFIC_LIGHT_MANAGER_HH
#define TRAFFIC_LIGHT_MANAGER_HH

#include <ignition/math/Vector3.hh>
#include <ignition/rendering/Scene.hh>
#include <maliput/api/lane.h>
#include <maliput/api/lane_data.h>
#include <maliput/api/rules/phase.h>
#include <maliput/api/rules/traffic_lights.h>

#include <array>
#include <unordered_map>
#include <vector>

namespace delphyne {
namespace gui {

/// \brief
class TrafficLightManager final {
 public:
  TrafficLightManager(ignition::rendering::ScenePtr& _scene);
  /// \brief Destructor. Cube's destruction will be in charge of the scene's destructor.
  ~TrafficLightManager() = default;

  void CreateTrafficLights(ignition::rendering::ScenePtr& _scene,
                           const std::vector<maliput::api::rules::TrafficLight>& _traffic_lights);

  void Clear(ignition::rendering::ScenePtr& _scene);

  void BlinkBulbs(bool on);

  void ChangeBulbState(const maliput::api::rules::BulbStates& bulb_states);

 private:
  static constexpr size_t kAmountOfColors = static_cast<size_t>(maliput::api::rules::BulbColor::kGreen) + 1;
  static const std::string kGreenMaterialName;
  static const std::string kGreenBrightMaterialName;
  static const std::string kRedMaterialName;
  static const std::string kRedBrightMaterialName;
  static const std::string kYellowMaterialName;
  static const std::string kYellowBrightMaterialName;

  struct BulbMeshes {
    ignition::rendering::VisualPtr visual;
    std::unordered_map<maliput::api::rules::Bulb::Id, ignition::rendering::VisualPtr> bulbs;
  };

  // TODO: Instead of having ton of meshes doing a drawcall each one, we could try to merge every mesh onto one.
  // The problem is that we need to turn on the material of one particular bulb. We could have a RGB texture
  // where each channel represents a bulb and a shader that turns on the color material of a given bulb and turn off
  // the other ones.
  struct TrafficLightMesh {
    ignition::rendering::VisualPtr visual;
    std::unordered_map<maliput::api::rules::BulbGroup::Id, BulbMeshes> bulb_groups;
  };

  inline ignition::rendering::MaterialPtr& GetRedMaterial() {
    return bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kRed)];
  }
  inline ignition::rendering::MaterialPtr& GetGreenMaterial() {
    return bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kGreen)];
  }
  inline ignition::rendering::MaterialPtr& GetYellowMaterial() {
    return bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kYellow)];
  }

  inline ignition::rendering::MaterialPtr& GetBrightRedMaterial() {
    return bright_bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kRed)];
  }
  inline ignition::rendering::MaterialPtr& GetBrightGreenMaterial() {
    return bright_bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kGreen)];
  }
  inline ignition::rendering::MaterialPtr& GetBrightYellowMaterial() {
    return bright_bulb_materials[static_cast<size_t>(maliput::api::rules::BulbColor::kYellow)];
  }

  ignition::rendering::VisualPtr GetBulbMesh(const maliput::api::rules::UniqueBulbId& bulb_id) const;

  maliput::api::rules::BulbColor GetBulbColor(const ignition::rendering::VisualPtr& bulb) const;

  void SetBulbMaterialColors();

  void SetBulbMaterial(const maliput::api::rules::UniqueBulbId& unique_bulb_id, ignition::rendering::VisualPtr& bulb,
                       maliput::api::rules::BulbColor color, maliput::api::rules::BulbState new_bulb_state);

  void RemoveBlinkingLight(const maliput::api::rules::UniqueBulbId& id);

  void CreateSingleTrafficLight(ignition::rendering::ScenePtr& _scene,
                                const maliput::api::rules::TrafficLight& _traffic_light);

  void CreateBulbGroup(ignition::rendering::ScenePtr& _scene, TrafficLightMesh* traffic_light_mesh,
                       const maliput::api::rules::TrafficLight::Id& traffic_light_id,
                       const maliput::api::rules::BulbGroup& _bulb_group,
                       const maliput::api::GeoPosition& traffic_light_world_position,
                       const maliput::api::Rotation& traffic_light_world_rotation);

  maliput::api::rules::Bulb::BoundingBox CreateSingleBulb(ignition::rendering::ScenePtr& _scene, BulbMeshes* bulb_group,
                                                          const maliput::api::rules::UniqueBulbId& unique_bulb_id,
                                                          const maliput::api::rules::Bulb& _single_bulb,
                                                          const maliput::api::GeoPosition& bulb_group_world_position,
                                                          const maliput::api::Rotation& bulb_group_world_rotation);

  std::array<ignition::rendering::MaterialPtr, kAmountOfColors> bulb_materials;
  std::array<ignition::rendering::MaterialPtr, kAmountOfColors> bright_bulb_materials;

  std::unordered_map<maliput::api::rules::TrafficLight::Id, TrafficLightMesh> traffic_lights;
  std::unordered_map<maliput::api::rules::UniqueBulbId, ignition::rendering::VisualPtr> blinking_bulbs;
  ignition::math::Vector3d sphere_bulb_aabb_min;
  ignition::math::Vector3d sphere_bulb_aabb_max;
  ignition::math::Vector3d unit_box_aabb_min;
  ignition::math::Vector3d unit_box_aabb_max;
};
}  // namespace gui
}  // namespace delphyne

#endif  // TRAFFIC_LIGHT_MANAGER_HH