// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERMODEL_HH
#define DELPHYNE_GUI_MALIPUTVIEWERMODEL_HH

#include <map>
#include <memory>
#include <string>

#include <maliput/api/road_geometry.h>
#include <maliput/api/road_network.h>

#include <ignition/common/Mesh.hh>

#include "maliput_mesh.hh"
#include "maliput_mesh_builder.hh"

namespace delphyne {
namespace gui {

/// \brief Holds the mesh, material and visualization status.
class MaliputMesh {
 public:
  /// \brief Holds the visualization status.
  bool visible{false};

  /// \brief Holds the mesh status.
  bool enabled{false};

  /// \brief Holds a pointer to the mesh.
  std::unique_ptr<ignition::common::Mesh> mesh{};

  /// \brief Holds a pointer to the material information. Note that this is
  /// defined on MaliputMeshBuilder.hh
  std::unique_ptr<maliput::mesh::Material> material{};
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
  /// \param[in] _geoMeshes A map of std::string <--> GeoMesh objects to
  ///            cd /convert.
  void ConvertMeshes(
    const std::map<std::string, maliput::mesh::GeoMesh>& _geoMeshes);

  /// \brief Populates this->labels map with this->roadGeometry lane and branch
  ///        point IDs.
  void GenerateLabels();

  /// \brief Frees and clears the roadGeometry, maliputMeshes and labels.
  void Clear();

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

}
}
#endif