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

#include <maliput/api/rules/regions.h>
#include <maliput/api/rules/right_of_way_rule.h>
#include <maliput-utilities/generate_obj.h>
#include <maliput-utilities/mesh.h>

#include <ignition/common/Mesh.hh>

// Code copied from malidrive's repository. You can find it in
// src/applications/malidrive_query.cc.
//
// Returns a vector of all possible direction usage values. Item order
// matches maliput::api::rules::DirectionUsageRule::Type enumeration.
const std::vector<std::string> DirectionUsageRuleNames();

// Serializes `road_position` into `out`.
std::ostream& operator<<(
    std::ostream& out,
    const maliput::api::RoadPosition& road_position);

// Serializes `road_position_result` into `out`.
std::ostream& operator<<(
    std::ostream& out,
    const maliput::api::RoadPositionResult& road_position_result);

// Serializes `state_type` into `out`.
std::ostream& operator <<(
    std::ostream& out,
    const maliput::api::rules::RightOfWayRule::State::Type& state_type);

// Serializes `state` into `out`.
std::ostream& operator <<(
    std::ostream& out,
    const maliput::api::rules::RightOfWayRule::State& state);

// Serializes `s_range` into `out`.
std::ostream& operator <<(
    std::ostream& out,
    const maliput::api::rules::SRange& s_range);

// Serializes `lane_s_range` into `out`.
std::ostream& operator <<(
    std::ostream& out,
    const maliput::api::rules::LaneSRange& lane_s_range);

// Serializes `lane_s_route` into `out`.
std::ostream& operator <<(
    std::ostream& out,
    const maliput::api::rules::LaneSRoute& lane_s_route);

// Serializes `zone_type` into `out`.
std::ostream& operator <<(
    std::ostream& out,
    const maliput::api::rules::RightOfWayRule::ZoneType& zone_type);

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
  RoadNetworkQuery(std::ostream* out, const maliput::api::RoadNetwork* rn) :
      out_(out), rn_(rn) {
    DELPHYNE_DEMAND(out_ != nullptr);
    DELPHYNE_DEMAND(rn_ != nullptr);
  }

  /// Redirects `geo_position` and `radius` to RoadGeometry::FindRoadPosition().
  void FindRoadPositions(const maliput::api::GeoPosition& geo_position,
                         double radius) {
    const std::vector<maliput::api::RoadPositionResult> results =
        rn_->road_geometry()->FindRoadPositions(geo_position, radius);

    (*out_) << "FindRoadPositions(geo_position:" << geo_position
            << ", radius: " << radius
            << ")" << std::endl;
    for (const maliput::api::RoadPositionResult& result : results) {
      (*out_) << "              : Result: " << result << std::endl;
    }
  }

  /// Redirects `lane_position` to `lane_id`'s Lane::ToGeoPosition().
  void ToGeoPosition(const maliput::api::LaneId& lane_id,
                     const maliput::api::LanePosition& lane_position) {
    const maliput::api::Lane* lane =
        rn_->road_geometry()->ById().GetLane(lane_id);

    if (lane == nullptr) {
      (*out_) << "              : Result: Could not find lane. " << std::endl;
      return;
    }

    const maliput::api::GeoPosition geo_position =
        lane->ToGeoPosition(lane_position);

    (*out_) << "(" << lane_id.string() << ")->ToGeoPosition(lane_position: "
            << lane_position << ")" << std::endl;
    (*out_) << "              : Result: geo_position:"
            << geo_position << std::endl;

    double distance = 0.0;
    maliput::api::GeoPosition nearest_pos;
    const maliput::api::RoadPosition road_position =
        rn_->road_geometry()->ToRoadPosition(
            geo_position, nullptr, &nearest_pos, &distance);

    (*out_) << "              : Result round_trip geo_position" << nearest_pos
            << ", with distance: " << distance
            << std::endl;
    (*out_) << "              : RoadPosition: " << road_position << std::endl;
  }

  /// Redirects `geo_position` to `lane_id`'s Lane::ToLanePosition().
  void ToLanePosition(const maliput::api::LaneId& lane_id,
                      const maliput::api::GeoPosition& geo_position) {
    const maliput::api::Lane* lane =
        rn_->road_geometry()->ById().GetLane(lane_id);
    if (lane == nullptr) {
      (*out_) << "              : Result: Could not find lane. " << std::endl;
      return;
    }

    double distance{};
    maliput::api::GeoPosition nearest_pos;
    const maliput::api::LanePosition lane_position =
        lane->ToLanePosition(geo_position, &nearest_pos, &distance);

    (*out_) << "(" << lane_id.string()
            << ")->ToLanePosition(geo_position: " << geo_position
            << ")" << std::endl;
    (*out_) << "              : Result: lane_pos:"
            << lane_position << ", nearest_pos: "
            << nearest_pos << ", with distance: "
            << distance << std::endl;
  }

  /// Redirects `geo_position` to RoadGeometry::ToRoadPosition().
  void ToRoadPosition(const maliput::api::GeoPosition& geo_position) {
    double distance;
    maliput::api::GeoPosition nearest_pos;
    const maliput::api::RoadPosition road_position =
        rn_->road_geometry()->ToRoadPosition(
            geo_position, nullptr, &nearest_pos, &distance);

    (*out_) << "ToRoadPosition(geo_position: " << geo_position
            << ")" << std::endl;
    (*out_) << "              : Result: nearest_pos:"
            << nearest_pos << " with distance: " << distance << std::endl;
    (*out_) << "                RoadPosition: " << road_position << std::endl;
  }

  /// Looks for all the maximum speed limits allowed at `lane_id`.
  void GetMaxSpeedLimit(const maliput::api::LaneId& lane_id) {
    const maliput::api::rules::RoadRulebook::QueryResults query_result =
        FindRulesFor(lane_id);

    const int n_speed_limits =
        static_cast<int>(query_result.speed_limit.size());
    if (n_speed_limits > 0) {
      double max_speed = query_result.speed_limit[0].max();
      maliput::api::rules::SpeedLimitRule::Id max_speed_id =
          query_result.speed_limit[0].id();
      for (int i = 1; i < n_speed_limits; i++) {
        const double max_speed_cur = query_result.speed_limit[i].max();
        if (max_speed_cur < max_speed) {
          max_speed = max_speed_cur;
          max_speed_id = query_result.speed_limit[i].id();
        }
      }

      (*out_) << "Speed limit (" << max_speed_id.string() << "): "
              << max_speed << " m/s" << std::endl;
    } else {
      (*out_) << "There is no speed limit found for this lane" << std::endl;
    }
  }

  /// Looks for all the direction usages at `lane_id`.
  void GetDirectionUsage(const maliput::api::LaneId& lane_id) {
    const maliput::api::rules::RoadRulebook::QueryResults query_result =
        FindRulesFor(lane_id);

    const int n_rules = static_cast<int>(query_result.direction_usage.size());
    const std::vector<std::string> direction_usage_names =
        DirectionUsageRuleNames();

    if (n_rules > 0) {
      for (const auto& direction_rule : query_result.direction_usage) {
        const auto& states = direction_rule.states();
        for(const auto& state : states) {
          const int state_type = int(state.second.type());
          if (state_type < 0 ||
              state_type >= int(direction_usage_names.size()) ) {
            (*out_) << "              : Result: Invalid direction usage rule. "
                    << std::endl;
            return;
          }

          (*out_) << "              : Result: Rule ("
                  << direction_rule.id().string() << "): "
                  << direction_usage_names[state_type] << std::endl;
        }
      }
    } else {
      (*out_) << "              : Result: There is no direction usage rules "
              << "found for this lane" << std::endl;
    }
  }

  /// Gets all right-of-way rules for the given `lane_s_range`.
  void GetRightOfWay(const maliput::api::rules::LaneSRange& lane_s_range) {
    const maliput::api::rules::RoadRulebook::QueryResults results =
      rn_->rulebook()->FindRules({lane_s_range}, 0.);
    const maliput::api::rules::RuleStateProvider*
      rule_state_provider = rn_->rule_state_provider();
    (*out_) << "Right of way for " << lane_s_range << ":" << std::endl;
    for (const auto& rule : results.right_of_way) {
      (*out_) << "    Rule(id: " << rule.id().string()
              << ", zone: " << rule.zone()
              << ", zone-type: '" << rule.zone_type() << "'";
      if (!rule.is_static()) {
        (*out_) << ", states: [";
        for (const auto& entry : rule.states()) {
          (*out_) << entry.second << ", ";
        }
        (*out_) << "]";
        auto rule_state_result = rule_state_provider->GetState(rule.id());
        if (rule_state_result.has_value()) {
          auto it = rule.states().find(rule_state_result->current_id);
          DELPHYNE_DEMAND(it != rule.states().end());
          (*out_) << ", current_state: " << it->second;
        }
      } else {
        (*out_) << ", current_state: " << rule.static_state();
      }
      (*out_) << ", static: " << (rule.is_static() ? "yes" : "no")
              << ")" << std::endl << std::endl;
    }
  }

  /// Gets all right-of-way rules' states for a given phase in a given phase
  /// ring.
  void
  GetPhaseRightOfWay(const maliput::api::rules::PhaseRing::Id& phase_ring_id,
                     const maliput::api::rules::Phase::Id& phase_id ) {
    const maliput::api::rules::PhaseRingBook* phase_ring_book =
      rn_->phase_ring_book();
    if (phase_ring_book == nullptr) {
      (*out_) << "Road network has no phase ring book" << std::endl;
      return;
    }

    const maliput::api::rules::RoadRulebook* road_rule_book = rn_->rulebook();
    if (road_rule_book == nullptr) {
      (*out_) << "Road network has no road rule book" << std::endl;
      return;
    }

    drake::optional<maliput::api::rules::PhaseRing> phase_ring =
      phase_ring_book->GetPhaseRing(phase_ring_id);
    if (!phase_ring.has_value()) {
      (*out_) << "'" << phase_ring_id.string()
              << "' is not a known phase ring"
              << std::endl;
      return;
    }

    auto it = phase_ring->phases().find(phase_id);
    if (it == phase_ring->phases().end()) {
      (*out_) << "'" << phase_id.string()
              << "' is not a phase in phase ring '"
              << phase_ring_id.string() << "'"
              << std::endl;
      return;
    }

    const maliput::api::rules::Phase& phase = it->second;
    (*out_) << "Right of way for " << phase_id.string() << ":" << std::endl;
    for (const auto& rule_id_to_rule_state_id : phase.rule_states()) {
      const maliput::api::rules::RightOfWayRule rule =
        road_rule_book->GetRule(rule_id_to_rule_state_id.first);
      const maliput::api::rules::RightOfWayRule::State& rule_state =
        rule.states().at(rule_id_to_rule_state_id.second);
      (*out_) << "    Rule(id: " << rule.id().string()
              << ", zone: " << rule.zone()
              << ", zone-type: '" << rule.zone_type() << "'"
              << ", current_state: " << rule_state
              << ", static: " << (rule.is_static() ? "yes" : "no")
              << ")" << std::endl << std::endl;
    }
  }

 private:
  // Finds QueryResults of Rules for `lane_id`.
  maliput::api::rules::RoadRulebook::QueryResults FindRulesFor(
      const maliput::api::LaneId& lane_id) {
    const maliput::api::Lane* lane =
        rn_->road_geometry()->ById().GetLane(lane_id);
    if (lane == nullptr) {
      std::cerr << " Could not find lane. " << std::endl;
      return maliput::api::rules::RoadRulebook::QueryResults();
    }

    const maliput::api::rules::SRange s_range(0., lane->length());
    const maliput::api::rules::LaneSRange lane_s_range(lane->id(), s_range);
    const std::vector<maliput::api::rules::LaneSRange> lane_s_ranges(
        1, lane_s_range);

    return rn_->rulebook()->FindRules(lane_s_ranges, 0.);
  }

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

  /// \brief Get a N amount of lanes from the owned road geometry.
  /// \param[in] _amount_of_lanes Amount of lanes desired to get from the owned
  /// road geometry
  /// \return Container that holds a class capable to be constructed using
  /// const char* and also needs to have a size, push_back and reserve
  /// method implemented
  template<typename Container>
  Container GetNLanes(size_t _amount_of_lanes) const;

  /// \brief Get all the lanes that the road geometry posses.
  /// \return Container that holds a class capable to be constructed using
  /// const char* and also needs to have a push_back and reserve
  /// method implemented
  template<typename Container>
  Container GetAllLaneIds() const;

  /// \brief Get all the rules for a given lane
  /// \param[in] _laneId Id of the desired lane to get the rules
  /// \return String class that it can be constructed with a const char* and has
  /// operator+ overriden, where each rule is separated by brackets.
  /// Ex: [Right of way Rule]\n.
  template<typename String>
  String GetRulesOfLane(const std::string& _laneId) const;

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
  /// \return String class that it can be constructed with a const char* and has
  /// operator+ overriden.
  template<typename String>
  String GetRightOfWayRules(
    const maliput::api::rules::LaneSRange& _laneSRange) const;

  /// \brief Get the max speed rules for a given lane id.
  /// \param[in] _laneId Id of the lane to get the max speed limit.
  /// \return String class that it can be constructed with a const char* and has
  /// operator+ overriden.
  template<typename String>
  String GetMaxSpeedLimitRules(
    const maliput::api::LaneId& _laneId) const;

  /// \brief Get the direction usage rules for a given lane id.
  /// \param[in] _laneId Id of the lane to get the direction usage rules.
  /// \return String class that it can be constructed with a const char* and has
  /// operator+ overriden.
  template<typename String>
  String GetDirectionUsageRules(
    const maliput::api::LaneId& _laneId) const;

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

template<typename Container>
Container MaliputViewerModel::GetNLanes(size_t _amount_of_lanes) const {

  const maliput::api::RoadGeometry* rg = this->roadGeometry ?
    this->roadGeometry.get() : this->roadNetwork->road_geometry();
  const std::unordered_map<
    maliput::api::LaneId, const maliput::api::Lane*>& all_lanes =
      rg->ById().GetLanes();
  Container lanes;
  lanes.reserve(_amount_of_lanes);
  for (const auto& lane : all_lanes)
  {
    if (static_cast<size_t>(lanes.size()) == _amount_of_lanes)
    {
      break;
    }
    lanes.push_back(lane.first.string().c_str());
  }
  return lanes;
}

template<typename Container>
Container MaliputViewerModel::GetAllLaneIds() const {
  const maliput::api::RoadGeometry* rg = this->roadGeometry ?
  this->roadGeometry.get() : this->roadNetwork->road_geometry();
  const std::unordered_map<
    maliput::api::LaneId, const maliput::api::Lane*>& all_lanes =
      rg->ById().GetLanes();
  Container lanes;
  lanes.reserve(all_lanes.size());
  for (const auto& lane : all_lanes)
  {
    lanes.push_back(lane.first.string().c_str());
  }
  return lanes;
}

template<typename String>
String MaliputViewerModel::GetRulesOfLane(const std::string& _laneId) const {
  if (this->roadNetwork == nullptr) {
    return String("There are no rules for this road");
  }
  maliput::api::LaneId id(_laneId);
  const maliput::api::RoadGeometry::IdIndex& roadIndex =
    this->roadNetwork->road_geometry()->ById();
  maliput::api::rules::LaneSRange laneSRange(id,
    maliput::api::rules::SRange(0., roadIndex.GetLane(id)->length()));
  String rules =
           "[Right of way rules]\n" +
           GetRightOfWayRules<String>(laneSRange) + "\n" +
           "[Max speed limit rules]\n" +
           GetMaxSpeedLimitRules<String>(id) + "\n" +
           "[Direction usage rules]\n" +
           GetDirectionUsageRules<String>(id) + "\n";
  return rules;
}

template<typename String>
String MaliputViewerModel::GetRightOfWayRules(
    const maliput::api::rules::LaneSRange& _laneSRange) const {
  std::ostringstream rightOfWayRules;
  RoadNetworkQuery query(&rightOfWayRules, roadNetwork.get());
  query.GetRightOfWay(_laneSRange);
  return String(rightOfWayRules.str().c_str());
}

template<typename String>
String MaliputViewerModel::GetMaxSpeedLimitRules(
  const maliput::api::LaneId& _laneId) const {

  std::ostringstream speedLimitRules;
  RoadNetworkQuery query(&speedLimitRules, roadNetwork.get());
  query.GetMaxSpeedLimit(_laneId);
  return String(speedLimitRules.str().c_str());
}

template<typename String>
String MaliputViewerModel::GetDirectionUsageRules(
  const maliput::api::LaneId& _laneId) const {

  std::ostringstream directionUsageRules;
  RoadNetworkQuery query(&directionUsageRules, roadNetwork.get());
  query.GetDirectionUsage(_laneId);
  return String(directionUsageRules.str().c_str());
}

}  // namespace gui
}  // namespace delphyne

#endif
