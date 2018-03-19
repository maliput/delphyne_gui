// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERMODEL_HH
#define DELPHYNE_GUI_MALIPUTVIEWERMODEL_HH

#include <map>
#include <memory>
#include <string>

#include <drake/automotive/maliput/api/road_geometry.h>

#include <ignition/common/Mesh.hh>

#include "MaliputMesh.hh"
#include "MaliputMeshBuilder.hh"

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
  std::unique_ptr<drake::maliput::mesh::Material> material{};
};

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
  bool Load();

  /// \brief Getter of the map of meshes.
  /// \return The map of meshes.
  const std::map<std::string, std::unique_ptr<MaliputMesh>>& Meshes() const;

  /// \brief Modifies the visualization state of @p key mesh.
  /// \param[in] _key The name of the mesh.
  /// \param[in] _newVisualState The new visualization status of the mesh.
  void SetLayerState(const std::string& _key, bool _isVisible);

 private:
  /// \brief Converts @p _geoMeshes into a
  ///        std::map<std::string, std::unique_ptr<ignition::common::Mesh>>
  ///        filling the instance variable meshes.
  /// \param[in] _geoMeshes A map of std::string <--> GeoMesh objects to
  ///            cd /convert.
  void ConvertMeshes(
    const std::map<std::string, drake::maliput::mesh::GeoMesh>& _geoMeshes);

  /// \brief Maliput RoadGeometry pointer.
  std::unique_ptr<const drake::maliput::api::RoadGeometry> roadGeometry;

  /// \brief Map of meshes to hold all the ignition meshes.
  std::map<std::string, std::unique_ptr<MaliputMesh>> maliputMeshes;
};

}
}
#endif
