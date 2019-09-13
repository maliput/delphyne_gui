// Copyright 2019 Toyota Research Institute

#ifndef TRAFFIC_LIGHT_MANAGER_HH
#define TRAFFIC_LIGHT_MANAGER_HH

#include <ignition/math/Vector3.hh>
#include <ignition/rendering/Scene.hh>
#include <maliput/api/lane.h>
#include <maliput/api/lane_data.h>
#include <maliput/api/rules/phase.h>
#include <maliput/api/rules/traffic_lights.h>

#include <unordered_map>
#include <vector>

namespace delphyne {
namespace gui {

/// \brief Class that creates a representation for each traffic light and controls them.
class TrafficLightManager final {
 public:
  TrafficLightManager(ignition::rendering::ScenePtr _scene);
  /// \brief Destructor. All mesh destruction will be in charge of the scene's destructor.
  ~TrafficLightManager() = default;

  /// \brief Renders all bulbs and box covering them passed by parameter with their corresponding colors
  /// \param[in] _scene Pointer containing the scene where all meshes will be rendered.
  /// \param[in] _traffic_lights Vector containing all the required traffic lights to render.
  void CreateTrafficLights(const std::vector<maliput::api::rules::TrafficLight>& _traffic_lights);

  /// \brief Destroy all meshes created by the manager explictly.
  /// \param[in] _scene Pointer containing the scene where all meshes were rendered.
  void Clear();

  /// \brief Change the material of the bulbs when a blink ocurrs.
  /// \param[in] on If on, the bulb will turn on, otherwise it will be turned off.
  void BlinkBulbs(bool on);

  /// \brief Set the state of all the bulbs.
  /// \param[in] bulb_states Unordered map containing the new state of each bulb.
  void SetBulbsState(const maliput::api::rules::BulbStates& bulb_states);

 private:
  /// \brief Name of the material used for the green bulb turned off.
  static const std::string kGreenMaterialName;
  /// \brief Name of the material used for the green bulb turned on.
  static const std::string kGreenBrightMaterialName;
  /// \brief Name of the material used for the red bulb turned off.
  static const std::string kRedMaterialName;
  /// \brief Name of the material used for the red bulb turned on.
  static const std::string kRedBrightMaterialName;
  /// \brief Name of the material used for the yellow bulb turned off.
  static const std::string kYellowMaterialName;
  /// \brief Name of the material used for the yellow bulb turned on.
  static const std::string kYellowBrightMaterialName;

  /// \brief Structure that contains the mesh related with the bulb group and connects it to a set of bulbs
  struct BulbMeshes {
    ignition::rendering::VisualPtr visual;
    std::unordered_map<maliput::api::rules::Bulb::Id, ignition::rendering::VisualPtr> bulbs;
  };

  /// \brief Structure that contains the mesh related with the traffic light (unused at the moment) and connects
  /// it to a set of bulb groups.
  struct TrafficLightMesh {
    ignition::rendering::VisualPtr visual;
    std::unordered_map<maliput::api::rules::BulbGroup::Id, BulbMeshes> bulb_groups;
  };

  inline ignition::rendering::MaterialPtr& GetRedMaterial() {
    return bulb_materials[maliput::api::rules::BulbColor::kRed];
  }
  inline ignition::rendering::MaterialPtr& GetGreenMaterial() {
    return bulb_materials[maliput::api::rules::BulbColor::kGreen];
  }
  inline ignition::rendering::MaterialPtr& GetYellowMaterial() {
    return bulb_materials[maliput::api::rules::BulbColor::kYellow];
  }

  inline ignition::rendering::MaterialPtr& GetBrightRedMaterial() {
    return bright_bulb_materials[maliput::api::rules::BulbColor::kRed];
  }
  inline ignition::rendering::MaterialPtr& GetBrightGreenMaterial() {
    return bright_bulb_materials[maliput::api::rules::BulbColor::kGreen];
  }
  inline ignition::rendering::MaterialPtr& GetBrightYellowMaterial() {
    return bright_bulb_materials[maliput::api::rules::BulbColor::kYellow];
  }

  /// \brief Gets the bulb mesh related with a given id.
  /// \param[in] bulb_id Unique id of a bulb.
  /// \returns A pointer that contains the bulb's mesh or nullptr if not found.
  ignition::rendering::VisualPtr GetBulbMesh(const maliput::api::rules::UniqueBulbId& bulb_id) const;

  /// \brief Gets the color of the bulb based on its material name.
  /// \param[in] bulb Pointer containing the bulb's mesh.
  /// \returns The color of a given bulb.
  maliput::api::rules::BulbColor GetBulbColor(const ignition::rendering::VisualPtr& bulb) const;

  /// \brief Initializes the bulb materials (red, yellow and green along with their brighter versions).
  /// \param[in] _scene Scene pointer where the traffic lights will be rendered.
  void InitializeBulbMaterials();

  /// \brief Sets the proper material for a bulb for a new state.
  /// \param[in] unique_bulb_id Unique id of a bulb.
  /// \param[in] bulb Pointer containing the mesh of a given bulb.
  /// \param[in] color Color used by the bulb.
  /// \param[in] new_bulb_state The new state the bulb will be.
  void SetBulbMaterial(const maliput::api::rules::UniqueBulbId& unique_bulb_id, ignition::rendering::VisualPtr& bulb,
                       maliput::api::rules::BulbColor color, maliput::api::rules::BulbState new_bulb_state);

  /// \brief Removes a given bulb from a list of blinking bulbs to not tick them anymore.
  /// \param[in] id Bulb's unique id.
  void RemoveBlinkingLight(const maliput::api::rules::UniqueBulbId& id);

  /// \brief Create the mesh for a given traffic light.
  /// \param[in] _scene Scene pointer where the traffic lights will be rendered.
  /// \param[in] _traffic_light Traffic light that will be rendered.
  void CreateSingleTrafficLight(const maliput::api::rules::TrafficLight& _traffic_light);

  /// \brief Create the mesh for a given bulb group.
  /// \param[in] traffic_light_mesh Structure that holds the visual of the traffic light and the unordered map of bulb
  /// groups to be part of.
  /// \param[in] _bulb_group Bulb group information to render the mesh.
  /// \param[in] traffic_light_world_position World position of the traffic light where this group lives in.
  /// \param[in] traffic_light_world_rotation World rotation of the traffic light where this group lives in.
  void CreateBulbGroup(TrafficLightMesh* traffic_light_mesh,
                       const maliput::api::rules::TrafficLight::Id& traffic_light_id,
                       const maliput::api::rules::BulbGroup& _bulb_group,
                       const maliput::api::GeoPosition& traffic_light_world_position,
                       const maliput::api::Rotation& traffic_light_world_rotation);

  /// \brief Create the mesh for a given bulb.
  /// \param[in] unique_bulb_id Unique id that belongs to the bulb to be rendered.
  /// \param[in] _single_bulb Bulb information to render the mesh.
  /// \param[in] bulb_group_world_position World position of the bulb group where this bulb lives in.
  /// \param[in] bulb_group_world_rotation World rotation of the bulb group where this bulb lives in.
  maliput::api::rules::Bulb::BoundingBox CreateSingleBulb(BulbMeshes* bulb_group,
                                                          const maliput::api::rules::UniqueBulbId& unique_bulb_id,
                                                          const maliput::api::rules::Bulb& _single_bulb,
                                                          const maliput::api::GeoPosition& bulb_group_world_position,
                                                          const maliput::api::Rotation& bulb_group_world_rotation);

  std::unordered_map<maliput::api::rules::BulbColor, ignition::rendering::MaterialPtr> bulb_materials;
  std::unordered_map<maliput::api::rules::BulbColor, ignition::rendering::MaterialPtr> bright_bulb_materials;

  std::unordered_map<maliput::api::rules::TrafficLight::Id, TrafficLightMesh> traffic_lights;
  /// \brief Unordered map containing all the bulbs that are in the blinking state to facilitate the change
  /// of materials.
  std::unordered_map<maliput::api::rules::UniqueBulbId, ignition::rendering::VisualPtr> blinking_bulbs;
  ignition::rendering::ScenePtr scene;
  ignition::math::Vector3d sphere_bulb_aabb_min;
  ignition::math::Vector3d sphere_bulb_aabb_max;
  ignition::math::Vector3d unit_box_aabb_min;
  ignition::math::Vector3d unit_box_aabb_max;
};
}  // namespace gui
}  // namespace delphyne

#endif  // TRAFFIC_LIGHT_MANAGER_HH
