// Copyright 2017 Toyota Research Institute
//
// Note: code in this file was forked and changed from this file:
// https://github.com/RobotLocomotion/drake/blob/master/automotive/maliput/utility/generate_obj.cc
// and this commit: 82bf3c8a02678f553c746bdbbe0f8e5a345841b7

#ifndef MALIPUT_MESH_BUILDER_HH
#define MALIPUT_MESH_BUILDER_HH

#include <map>
#include <string>
#include <vector>

#include <drake/automotive/maliput/api/road_geometry.h>
#include <drake/automotive/maliput/api/segment.h>

#include <ignition/common/Mesh.hh>

#include "maliput_mesh.hh"

namespace drake {
namespace maliput {
namespace mesh {

/// Multitude of parameters for generating an OBJ model of a road surface,
/// with sensible defaults.
struct Features {
  /// Maximum distance between rendered vertices, in either s- or r-dimension,
  /// along a lane's surface
  double max_grid_unit{1.0};
  /// Minimum number of vertices, in either s- or r-dimension, along a lane's
  /// surface.
  double min_grid_resolution{5.0};
  /// Draw stripes along lane_bounds() of each lane?
  bool draw_stripes{true};
  /// Draw arrows at start/finish of each lane?
  bool draw_arrows{true};
  /// Draw highlighting swath with lane_bounds() of each lane?
  bool draw_lane_haze{true};
  /// Draw branching at BranchPoints?
  bool draw_branch_points{true};
  /// Draw highlighting of elevation_bounds of each lane?
  bool draw_elevation_bounds{true};
  /// Absolute width of stripes
  double stripe_width{0.25};
  /// Absolute elevation (h) of stripes above road surface
  double stripe_elevation{0.05};
  /// Absolute elevation (h) of arrows above road surface
  double arrow_elevation{0.05};
  /// Absolute elevation (h) of lane-haze above road surface
  double lane_haze_elevation{0.02};
  /// Absolute elevation (h) of branch-points above road surface
  double branch_point_elevation{0.5};
  /// Height of rendered branch-point arrows
  double branch_point_height{0.5};
  /// Origin of OBJ coordinates relative to world-frame
  api::GeoPosition origin{0., 0., 0.};
  /// ID's of specific segments to be highlighted.  (If non-empty, then the
  /// Segments *not* specified on this list will be rendered as grayed-out.)
  std::vector<api::SegmentId> highlighted_segments;
};


/// Material information.
struct Material {
  ignition::math::Vector3d diffuse;  /// Kd
  ignition::math::Vector3d ambient;  /// Ka
  ignition::math::Vector3d specular;  /// Ks
  double shinines;  /// Ns
  double transparency;  /// 1.0 - d
};


/// Builds a map of meshes based on `features` properties and the RoadGeometry.
///
/// @param road_geometry  the api::RoadGeometry to model.
/// @param features  parameters for constructing the mesh.
/// @return A map with the meshes. Keys will be std::string objects in the
/// following list:
///   - asphalt
///   - lane
///   - marker
///   - h_bounds
///   - branch_point
///   - grayed_asphalt
///   - grayed_lane
///   - grayed_marker
std::map<std::string, GeoMesh> BuildMeshes(const api::RoadGeometry* rg,
                                           const Features& features);

/// Generates a Wavefront OBJ model of the road surface of an api::RoadGeometry.
///
/// @param road_geometry  the api::RoadGeometry to model
/// @param dirpath  directory component of the output pathnames
/// @param fileroot  root of the filename component of the output pathnames
/// @param features  parameters for constructing the mesh
///
/// GenerateObjFile actually produces two files:  the first, named
/// [@p dirpath]/[@p fileroot].obj, is a Wavefront OBJ containing the
/// mesh which models the api::RoadGeometry.  The second file is a
/// Wavefront MTL file named [@p dirpath]/[@p fileroot].mtl, containing
/// descriptions of materials referenced by the OBJ file.
///
/// The produced mesh covers the area within the driveable-bounds of the
/// road surface described by the RoadGeometry.
void GenerateObjFile(const api::RoadGeometry* rg,
                     const std::string& dirpath,
                     const std::string& fileroot,
                     const Features& features);


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
std::unique_ptr<ignition::common::Mesh> Convert(const std::string& name,
                                                const GeoMesh& geo_mesh);


/// Gets a Material based on `material_name` key.
///
/// Possible `material_name` values may be any of the following:
///   - asphalt
///   - lane
///   - marker
///   - h_bounds
///   - branch_point
///   - grayed_asphalt
///   - grayed_lane
///   - grayed_marker
///
/// @param material_name The key to get the material.
/// @return A std::unique_ptr<Material> with a filled Material structure. It
/// will be nullptr when `material_name` is not any of the previous options.
std::unique_ptr<Material> GetMaterialByName(const std::string& material_name);


}  // namespace mesh
}  // namespace maliput
}  // namespace drake

#endif
