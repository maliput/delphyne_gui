// Copyright 2017 Toyota Research Institute

#ifndef MALIPUT_MESH_CONVERTER_HH
#define MALIPUT_MESH_CONVERTER_HH

#include <map>
#include <string>
#include <vector>

#include <ignition/common/Mesh.hh>
#include <maliput-utilities/mesh.h>

namespace delphyne {
namespace mesh {

/// Converts a GeoMesh into an ignition::common::Mesh.
///
/// The ignition::common::Mesh will contain only one ignition::common::SubMesh,
/// and current implementation only supports up to four vertices per GeoFace.
/// Note that generated meshes will point to both sides, consequently for each
/// group of three vertices that represent a triangle in the mesh, there are six
/// indices entries.
///
/// @param name The name of the mesh.
/// @param geo_mesh The GeoMesh to convert into a ignition::common::Mesh. It
/// must have at least once GeoFace with at least three vertices on it.
/// @return A std::unique_ptr<ignition::common::Mesh> with the equivalent mesh
/// construction. The pointer will be nullptr when the mesh has not the correct
/// vertices requirements.
std::unique_ptr<ignition::common::Mesh> Convert(
    const std::string& name,
    const maliput::utility::mesh::GeoMesh& geo_mesh);


}  // namespace mesh
}  // namespace delphyne

#endif
