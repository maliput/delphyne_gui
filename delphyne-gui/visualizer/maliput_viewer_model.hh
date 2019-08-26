// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERMODEL_HH
#define DELPHYNE_GUI_MALIPUTVIEWERMODEL_HH

#include <map>
#include <memory>
#include <ostream>
#include <string>

#include <delphyne/macros.h>
#include <ignition/math/Vector3.hh>

#include <maliput-utilities/generate_obj.h>
#include <maliput-utilities/mesh.h>
#include <maliput/api/lane.h>
#include <maliput/api/road_geometry.h>
#include <maliput/api/road_network.h>

#include <maliput-utilities/generate_obj.h>
#include <maliput-utilities/mesh.h>
#include <maliput/api/rules/regions.h>
#include <maliput/api/rules/right_of_way_rule.h>

#include <ignition/common/Mesh.hh>

// Code copied from malidrive's repository. You can find it in
// src/applications/malidrive_query.cc.
//
// Returns a vector of all possible direction usage values. Item order
// matches maliput::api::rules::DirectionUsageRule::Type enumeration.
const std::vector<std::string> DirectionUsageRuleNames();

// Serializes `road_position` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::RoadPosition& road_position);

// Serializes `road_position_result` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::RoadPositionResult& road_position_result);

// Serializes `state_type` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::rules::RightOfWayRule::State::Type& state_type);

// Serializes `state` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::rules::RightOfWayRule::State& state);

// Serializes `s_range` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::rules::SRange& s_range);

// Serializes `lane_s_range` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::rules::LaneSRange& lane_s_range);

// Serializes `lane_s_route` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::rules::LaneSRoute& lane_s_route);

// Serializes `zone_type` into `out`.
std::ostream& operator<<(std::ostream& out, const maliput::api::rules::RightOfWayRule::ZoneType& zone_type);

namespace delphyne {
namespace gui {

/// Code copied from malidrive's repository. You can find it in
/// src/applications/malidrive_query.cc.
///
/// Query and logs results to RoadGeometry or RoadRulebook minimizing the
/// overhead of getting the right calls / asserting conditions.
class RoadNetworkQuery {
 public:
  DELPHYNE_NO_COPY_NO_MOVE_NO_ASSIGN(RoadNetworkQuery)

  /// Constructs a RoadNetworkQuery.
  ///
  /// @param out A pointer to an output stream where results will be logged.
  ///            It must not be nullptr.
  /// @param rn A pointer to a RoadNetwork. It must not be nullptr.
  /// @throws std::runtime_error When `out` or `rn` are nullptr.
  RoadNetworkQuery(std::ostream* out, const maliput::api::RoadNetwork* rn) : out_(out), rn_(rn) {
    DELPHYNE_DEMAND(out_ != nullptr);
    DELPHYNE_DEMAND(rn_ != nullptr);
  }

  /// Redirects `geo_position` and `radius` to RoadGeometry::FindRoadPosition().
  void FindRoadPositions(const maliput::api::GeoPosition& geo_position, double radius);

  /// Redirects `lane_position` to `lane_id`'s Lane::ToGeoPosition().
  void ToGeoPosition(const maliput::api::LaneId& lane_id, const maliput::api::LanePosition& lane_position);

  /// Redirects `geo_position` to `lane_id`'s Lane::ToLanePosition().
  void ToLanePosition(const maliput::api::LaneId& lane_id, const maliput::api::GeoPosition& geo_position);

  /// Redirects `geo_position` to RoadGeometry::ToRoadPosition().
  void ToRoadPosition(const maliput::api::GeoPosition& geo_position);

  /// Looks for all the maximum speed limits allowed at `lane_id`.
  void GetMaxSpeedLimit(const maliput::api::LaneId& lane_id);

  /// Looks for all the direction usages at `lane_id`.
  void GetDirectionUsage(const maliput::api::LaneId& lane_id);

  /// Gets all right-of-way rules for the given `lane_s_range`.
  void GetRightOfWay(const maliput::api::rules::LaneSRange& lane_s_range);

  /// Gets all right-of-way rules' states for a given phase in a given phase
  /// ring.
  void GetPhaseRightOfWay(const maliput::api::rules::PhaseRing::Id& phase_ring_id,
                          const maliput::api::rules::Phase::Id& phase_id);

 private:
  // Finds QueryResults of Rules for `lane_id`.
  maliput::api::rules::RoadRulebook::QueryResults FindRulesFor(const maliput::api::LaneId& lane_id);

  std::ostream* out_{};
  const maliput::api::RoadNetwork* rn_{};
};

/// \brief Holds the mesh, material and visualization status.
class MaliputMesh {
 public:
  /// \brief Holds the visualization status.
  bool visible{false};

  /// \brief Holds the mesh status.
  bool enabled{false};

  /// \brief Holds a pointer to the mesh.
  std::unique_ptr<ignition::common::Mesh> mesh{};

  /// \brief Holds a pointer to the material information.
  std::unique_ptr<maliput::utility::Material> material{};
};

/// \brief Holds the information to build a label.
class MaliputLabel {
 public:
  /// \brief Holds the visualization status.
  bool visible{true};

  /// \brief Holds the mesh status.
  bool enabled{true};

  /// \brief Text to show by the label.
  std::string text{};

  /// \brief Position of the label.
  ignition::math::Vector3d position{};
};

/// \brief Type of label based on road entities.
enum class MaliputLabelType {
  kLane,         ///< A lane label.
  kBranchPoint,  ///< A branch point label.
};

/// \brief Converts @p _type into a MaliputLabelType.
/// \details @p _type must be either "lane_text_label" or
/// "branchpoint_text_label".
/// \throws std::runtime_error When @p _type is not "lane_text_label" nor
/// "branchpoint_text_label".
/// \return A MaliputLabelType that is kLane when @p _type is "lane_text_label"
/// and kBranchPoint when @p _type is "branchpoint_text_label".
MaliputLabelType FromString(const std::string& _type);

/// \brief Model of the plugin.
///
/// Holds the information, as a map of meshes and materials.
class MaliputViewerModel {
 public:
  /// \brief Constructor.
  MaliputViewerModel() = default;

  /// \brief Destructor.
  ~MaliputViewerModel() = default;

  /// \brief Loads the meshes from a yaml file.
  ///
  /// Gets the file path from GlobalAttributes and loads the RoadGeometry into
  /// memory. Then, converts it into a map of meshes, loading each mesh material
  /// information. Meshes that are not available, are set to kDisabled.
  bool Load(const std::string& _maliputFilePath);

  /// \brief Getter of the map of meshes.
  /// \return The map of meshes.
  const std::map<std::string, std::unique_ptr<MaliputMesh>>& Meshes() const;

  /// \brief Getter of the map of labels.
  /// \return The map of labels.
  const std::map<MaliputLabelType, std::vector<MaliputLabel>>& Labels() const;

  /// \brief Modifies the visualization state of @p key mesh.
  /// \param[in] _key The name of the mesh.
  /// \param[in] _isVisible The new visualization status of the mesh.
  void SetLayerState(const std::string& _key, bool _isVisible);

  /// \brief Modifies the visualization state of @p _type type text labels.
  /// \param[in] _type Labels type.
  /// \param[in] _isVisible The new visualization status of the text label.
  void SetTextLabelState(MaliputLabelType _type, bool _isVisible);

  /// \brief Get the lane associated with a point in world space if exists.
  /// \param[in] _position World position of point that intersected with a plane
  /// \return Lane associated with that position or nullptr if not found.
  const maliput::api::Lane* GetLaneFromWorldPosition(const ignition::math::Vector3d& _position);

  /// \brief Get the lane associated with an id.
  /// \param[in] _id Id of the lane.
  /// \return Lane associated with given id or nullptr if invalid id.
  const maliput::api::Lane* GetLaneFromId(const std::string& _id);

  /// \brief Get N lanes from the underlying road geometry.
  /// \param[in] _n Amount of lanes desired to get from the underlying
  /// road geometry.
  /// \tparam ContainerType A container class that must implement size(), push_back() and reserve() methods.
  /// See std::vector for further reference on each one of these methods. Its elements' type must be a string class
  /// that must be constructible with a single const char* argument and must support concatenation via operator+.
  /// \return Container that contains N lane ids.
  template <typename ContainerType>
  ContainerType GetNLanes(size_t _n) const;

  /// \brief Get all the lanes that the road geometry posses.
  /// \tparam ContainerType A container class that must implement size(), push_back() and reserve() methods.
  /// See std::vector for further reference on each one of these methods. Its elements' type must be a string class
  /// that must be constructible with a single const char* argument and must support concatenation via operator+.
  /// \return All lane ids from the underlying road geometry
  template <typename ContainerType>
  ContainerType GetAllLaneIds() const;

  /// \brief Get all the rules for a given lane.
  /// \param[in] _laneId Id of the desired lane to get the rules from.
  /// \tparam StringType A string class that must be constructible with a single const char* argument and
  /// must support concatenation via operator+.
  /// \return rules separated by brackets.
  /// Ex: [Right of way Rule]\n.
  template <typename StringType>
  StringType GetRulesOfLane(const std::string& _laneId) const;

 private:
  /// \brief Loads a maliput RoadGeometry of multilane from
  /// @p _maliputFilePath.
  /// \details Opens the file, iterates for each line, and tries to match it
  /// with "maliput_multilane_builder:".
  /// If the key doesn't exist in the file, it's not valid.
  /// Otherwise the correct loader will be called to parse the file.
  /// \param _maliputFilePath The YAML file path to parse.
  void LoadRoadGeometry(const std::string& _maliputFilePath);

  /// \brief Converts @p _geoMeshes into a
  ///        std::map<std::string, std::unique_ptr<ignition::common::Mesh>>
  ///        filling the instance variable meshes.
  /// \param[in] _geoMeshes An named collection of GeoMesh objects to convert.
  void ConvertMeshes(
      const std::map<std::string, std::pair<maliput::utility::mesh::GeoMesh, maliput::utility::Material>>& _geoMeshes);

  /// \brief Populates this->labels map with this->roadGeometry lane and branch
  ///        point IDs.
  void GenerateLabels();

  /// \brief Frees and clears the roadGeometry, maliputMeshes and labels.
  void Clear();

  /// \brief Get the right of way rules for a given LaneSRange.
  /// \param[in] _laneSRange Object that contains a lane id and a range in the s
  /// coordinate.
  /// \tparam StringType A string class that must be constructible with a single const char* argument and
  /// must support concatenation via operator+.
  /// \return Right of way rules as a StringType representation.
  template <typename StringType>
  StringType GetRightOfWayRules(const maliput::api::rules::LaneSRange& _laneSRange) const;

  /// \brief Get the max speed rules for a given lane id.
  /// \param[in] _laneId Id of the lane to get the max speed limit rules from.
  /// \tparam StringType A string class that must be constructible with a single const char* argument and
  /// must support concatenation via operator+.
  /// \return Max speed limit rules as a StringType representation.
  template <typename StringType>
  StringType GetMaxSpeedLimitRules(const maliput::api::LaneId& _laneId) const;

  /// \brief Get the direction usage rules for a given lane id.
  /// \param[in] _laneId Id of the lane to get the direction usage rules from.
  /// \tparam StringType A string class that must be constructible with a single const char* argument and
  /// must support concatenation via operator+.
  /// \return Direction usage rules as a StringType representation.
  template <typename StringType>
  StringType GetDirectionUsageRules(const maliput::api::LaneId& _laneId) const;

  // To support both malidrive and multilane files, we have both. roadNetwork
  // has a pointer to a RoadGeometry.

  /// \brief Maliput RoadGeometry pointer.
  std::unique_ptr<const maliput::api::RoadGeometry> roadGeometry;

  /// \brief Maliput RoadNetwork pointer.
  std::unique_ptr<const maliput::api::RoadNetwork> roadNetwork;

  /// \brief Map of meshes to hold all the ignition meshes.
  std::map<std::string, std::unique_ptr<MaliputMesh>> maliputMeshes;

  /// \brief Map of labels.
  std::map<MaliputLabelType, std::vector<MaliputLabel>> labels;
};

template <typename ContainerType>
ContainerType MaliputViewerModel::GetNLanes(size_t _n) const {
  const maliput::api::RoadGeometry* rg =
      this->roadGeometry ? this->roadGeometry.get() : this->roadNetwork->road_geometry();
  const std::unordered_map<maliput::api::LaneId, const maliput::api::Lane*>& all_lanes = rg->ById().GetLanes();
  ContainerType lanes;
  lanes.reserve(_n);
  for (const auto& lane : all_lanes) {
    if (static_cast<size_t>(lanes.size()) == _n) {
      break;
    }
    lanes.push_back(lane.first.string().c_str());
  }
  return lanes;
}

template <typename ContainerType>
ContainerType MaliputViewerModel::GetAllLaneIds() const {
  const maliput::api::RoadGeometry* rg =
      this->roadGeometry ? this->roadGeometry.get() : this->roadNetwork->road_geometry();
  const std::unordered_map<maliput::api::LaneId, const maliput::api::Lane*>& all_lanes = rg->ById().GetLanes();
  ContainerType lanes;
  lanes.reserve(all_lanes.size());
  for (const auto& lane : all_lanes) {
    lanes.push_back(lane.first.string().c_str());
  }
  return lanes;
}

template <typename StringType>
StringType MaliputViewerModel::GetRulesOfLane(const std::string& _laneId) const {
  if (this->roadNetwork == nullptr) {
    return StringType("There are no rules for this road");
  }
  maliput::api::LaneId id(_laneId);
  const maliput::api::RoadGeometry::IdIndex& roadIndex = this->roadNetwork->road_geometry()->ById();
  maliput::api::rules::LaneSRange laneSRange(id, maliput::api::rules::SRange(0., roadIndex.GetLane(id)->length()));
  StringType rules = "[Right of way rules]\n" + GetRightOfWayRules<StringType>(laneSRange) + "\n" +
                     "[Max speed limit rules]\n" + GetMaxSpeedLimitRules<StringType>(id) + "\n" +
                     "[Direction usage rules]\n" + GetDirectionUsageRules<StringType>(id) + "\n";
  return rules;
}

template <typename StringType>
StringType MaliputViewerModel::GetRightOfWayRules(const maliput::api::rules::LaneSRange& _laneSRange) const {
  std::ostringstream rightOfWayRules;
  RoadNetworkQuery query(&rightOfWayRules, roadNetwork.get());
  query.GetRightOfWay(_laneSRange);
  return StringType(rightOfWayRules.str().c_str());
}

template <typename StringType>
StringType MaliputViewerModel::GetMaxSpeedLimitRules(const maliput::api::LaneId& _laneId) const {
  std::ostringstream speedLimitRules;
  RoadNetworkQuery query(&speedLimitRules, roadNetwork.get());
  query.GetMaxSpeedLimit(_laneId);
  return StringType(speedLimitRules.str().c_str());
}

template <typename StringType>
StringType MaliputViewerModel::GetDirectionUsageRules(const maliput::api::LaneId& _laneId) const {
  std::ostringstream directionUsageRules;
  RoadNetworkQuery query(&directionUsageRules, roadNetwork.get());
  query.GetDirectionUsage(_laneId);
  return StringType(directionUsageRules.str().c_str());
}

}  // namespace gui
}  // namespace delphyne

#endif
