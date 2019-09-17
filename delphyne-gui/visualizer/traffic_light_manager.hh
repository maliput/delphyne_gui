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

/// \brief Class that creates a representation for traffic light and controls them.
class TrafficLightManager final {
 public:
  TrafficLightManager(ignition::rendering::ScenePtr _scene);
  /// \brief Destructor. All mesh destruction will be in charge of the scene's destructor.
  ~TrafficLightManager() = default;

  /// \brief Renders all bulbs and box covering them passed by parameter with their corresponding colors
  /// \param[in] _traffic_lights Vector containing all the required traffic lights to render.
  void CreateTrafficLights(const std::vector<maliput::api::rules::TrafficLight>& _traffic_lights);

  /// \brief Destroy all meshes created by the manager explictly.
  void Clear();

  /// \brief Change the material of the bulbs when a blink ocurrs.
  /// \param[in] on If on, the bulb will turn on, otherwise it will be turned off.
  void Tick(bool on);

  /// \brief Set the state of all the bulbs.
  /// \param[in] bulb_states Unordered map containing the new state of each bulb.
  void SetBulbStates(const maliput::api::rules::BulbStates& _bulb_states);

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
    std::unordered_map<maliput::api::rules::BulbGroup::Id, BulbMeshes> bulbGroups;
  };

  ignition::rendering::MaterialPtr& GetRedMaterial() {
    return bulbMaterials[maliput::api::rules::BulbColor::kRed];
  }
  ignition::rendering::MaterialPtr& GetGreenMaterial() {
    return bulbMaterials[maliput::api::rules::BulbColor::kGreen];
  }
  ignition::rendering::MaterialPtr& GetYellowMaterial() {
    return bulbMaterials[maliput::api::rules::BulbColor::kYellow];
  }

  ignition::rendering::MaterialPtr& GetBrightRedMaterial() {
    return brightBulbMaterials[maliput::api::rules::BulbColor::kRed];
  }
  ignition::rendering::MaterialPtr& GetBrightGreenMaterial() {
    return brightBulbMaterials[maliput::api::rules::BulbColor::kGreen];
  }
  ignition::rendering::MaterialPtr& GetBrightYellowMaterial() {
    return brightBulbMaterials[maliput::api::rules::BulbColor::kYellow];
  }

  /// \brief Gets the bulb mesh related with a given id.
  /// \param[in] bulb_id Unique id of a bulb.
  /// \returns A pointer that contains the bulb's mesh or nullptr if not found.
  ignition::rendering::VisualPtr GetBulbMesh(const maliput::api::rules::UniqueBulbId& _bulb_id) const;

  /// \brief Gets the color of the bulb based on its material name.
  /// \param[in] bulb Pointer containing the bulb's mesh.
  /// \returns The color of a given bulb.
  maliput::api::rules::BulbColor GetBulbColor(const ignition::rendering::VisualPtr& _bulb) const;

  /// \brief Initializes the bulb materials (red, yellow and green along with their brighter versions). This is called
  /// just once per instance.
  void InitializeBulbMaterials();

  /// \brief Sets the proper material for a bulb for a new state.
  /// \param[in] unique_bulb_id Unique id of a bulb.
  /// \param[in] bulb Pointer containing the mesh of a given bulb.
  /// \param[in] color Color used by the bulb.
  /// \param[in] new_bulb_state The new state the bulb will be.
  void SetBulbMaterial(const maliput::api::rules::UniqueBulbId& _unique_bulb_id, ignition::rendering::VisualPtr& _bulb,
                       maliput::api::rules::BulbColor _color, maliput::api::rules::BulbState _new_bulb_state);

  /// \brief Removes a given bulb from a list of blinking bulbs to not tick them anymore.
  /// \param[in] _unique_bulb_id Bulb's unique id.
  void RemoveBlinkingLight(const maliput::api::rules::UniqueBulbId& _unique_bulb_id);

  /// \brief Create the mesh for a given traffic light.
  /// \param[in] _traffic_light Traffic light that will be rendered.
  void CreateSingleTrafficLight(const maliput::api::rules::TrafficLight& _traffic_light);

  /// \brief Create the mesh for a given bulb group.
  /// \param[in] _traffic_light_mesh Structure that holds the visual of the traffic light and the unordered map of bulb
  /// groups to be part of.
  /// \param[in] _bulb_group Bulb group information to render the mesh.
  /// \param[in] _traffic_light_world_position World position of the traffic light where this group lives in.
  /// \param[in] _traffic_light_world_rotation World rotation of the traffic light where this group lives in.
  void CreateBulbGroup(TrafficLightMesh* _traffic_light_mesh,
                       const maliput::api::rules::TrafficLight::Id& _traffic_light_id,
                       const maliput::api::rules::BulbGroup& _bulb_group,
                       const maliput::api::GeoPosition& _traffic_light_world_position,
                       const maliput::api::Rotation& _traffic_light_world_rotation);

  /// \brief Create the mesh for a given bulb that is within a bulb group.
  /// \param[in] _unique_bulb_id Unique id that belongs to the bulb to be rendered.
  /// \param[in] _single_bulb Bulb information to render the mesh.
  /// \param[in] _bulb_group_world_position World position of the bulb group where this bulb lives in.
  /// \param[in] _bulb_group_world_rotation World rotation of the bulb group where this bulb lives in.
  maliput::api::rules::Bulb::BoundingBox CreateSingleBulb(BulbMeshes* _bulb_group,
                                                          const maliput::api::rules::UniqueBulbId& _unique_bulb_id,
                                                          const maliput::api::rules::Bulb& _single_bulb,
                                                          const maliput::api::GeoPosition& _bulb_group_world_position,
                                                          const maliput::api::Rotation& _bulb_group_world_rotation);

  /// \brief Unordered map containing the green, yellow and red material for the turned off bulb state.
  std::unordered_map<maliput::api::rules::BulbColor, ignition::rendering::MaterialPtr> bulbMaterials;
  /// \brief Unordered map containing the green, yellow and red material for the turned on bulb state.
  std::unordered_map<maliput::api::rules::BulbColor, ignition::rendering::MaterialPtr> brightBulbMaterials;
  /// \brief Unordered map containing all the traffic light visual hierarchy.
  std::unordered_map<maliput::api::rules::TrafficLight::Id, TrafficLightMesh> trafficLights;
  /// \brief Unordered map containing all the bulbs that are in the blinking state to facilitate the change
  /// of materials.
  std::unordered_map<maliput::api::rules::UniqueBulbId, ignition::rendering::VisualPtr> blinkingBulbs;
  /// \brief Pointer to the scene where the traffic lights will be rendered.
  ignition::rendering::ScenePtr scene;
  /// \brief Min point of the sphere's bounding box used for the bulbs.
  ignition::math::Vector3d sphereBulbAABBMax;
  /// \brief Max point of the sphere's bounding box used for the bulbs.
  ignition::math::Vector3d sphereBulbAABBMin;
  /// \brief Min point of the bulb group's bounding box.
  ignition::math::Vector3d unitBoxAABBMin;
  /// \brief Max point of the bulb group's bounding box.
  ignition::math::Vector3d unitBoxAABBMax;
};
}  // namespace gui
}  // namespace delphyne

#endif  // TRAFFIC_LIGHT_MANAGER_HH
