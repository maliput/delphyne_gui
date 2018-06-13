// Copyright 2017 Toyota Research Institute
//
// Note: code in this file was forked and changed from this file:
// https://github.com/RobotLocomotion/drake/blob/master/automotive/maliput/utility/generate_obj.cc
// and this commit: 82bf3c8a02678f553c746bdbbe0f8e5a345841b7

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <delphyne/macros.h>

#include <drake/automotive/maliput/api/branch_point.h>
#include <drake/automotive/maliput/api/junction.h>
#include <drake/automotive/maliput/api/lane.h>
#include <drake/automotive/maliput/api/lane_data.h>
#include <drake/automotive/maliput/api/road_geometry.h>
#include <drake/automotive/maliput/api/segment.h>

#include <ignition/common/Console.hh>
#include <ignition/common/SubMesh.hh>
#include <ignition/math/Vector3.hh>

#include "maliput_mesh_builder.hh"

namespace drake {
namespace maliput {
namespace mesh {
namespace {


// Traverses @p lane, generating a cover of the surface with with quads
// (4-vertex faces) which are added to @p mesh.  The quads are squares in
// the (s,r) space of the lane.
//
// @param mesh  the GeoMesh which will receive the quads
// @param lane  the api::Lane to cover with quads
// @param grid_unit  size of each quad (length of edge in s and r dimensions)
// @param use_driveable_bounds  if true, use the lane's driveable_bounds()
//        to determine the lateral extent of the coverage; otherwise, use
//        lane_bounds()
// @param elevation a function taking `(s, r)` as parameters and returning
//        the corresponding elevation `h`, to yield a quad vertex `(s, r, h)`
void CoverLaneWithQuads(
    GeoMesh* mesh, const api::Lane* lane,
    double grid_unit,
    bool use_driveable_bounds,
    const std::function<double(double, double)>& elevation) {
  const double s_max = lane->length();

  for (double s0 = 0; s0 < s_max; s0 += grid_unit) {
    double s1 = s0 + grid_unit;
    if (s1 > s_max) { s1 = s_max; }

    const api::RBounds rb0 = use_driveable_bounds ?
        lane->driveable_bounds(s0) : lane->lane_bounds(s0);
    const api::RBounds rb1 = use_driveable_bounds ?
        lane->driveable_bounds(s1) : lane->lane_bounds(s1);

    // Left side of lane (r >= 0).
    {
      double r00 = 0.;
      double r10 = 0.;
      while ((r00 < rb0.max()) && (r10 < rb1.max())) {
        const double r01 = std::min(r00 + grid_unit, rb0.max());
        const double r11 = std::min(r10 + grid_unit, rb1.max());
        //
        // (s1,r11) o <-- o (s1,r10)       ^ +s
        //          |     ^                |
        //          v     |          +r <--o
        // (s0,r01) o --> * (s0,r00)
        //
        SrhFace srh_face({
            {s0, r00, elevation(s0, r00)},
            {s1, r10, elevation(s1, r10)},
            {s1, r11, elevation(s1, r11)},
            {s0, r01, elevation(s0, r01)}}, {0., 0., 1.});
        mesh->PushFace(srh_face.ToGeoFace(lane));

        r00 += grid_unit;
        r10 += grid_unit;
      }
    }
    // Right side of lane (r <= 0).
    {
      double r00 = 0.;
      double r10 = 0.;
      while ((r00 > rb0.min()) && (r10 > rb1.min())) {
        const double r01 = std::max(r00 - grid_unit, rb0.min());
        const double r11 = std::max(r10 - grid_unit, rb1.min());
        //
        // (s1,r10) o <-- o (s1,r11)  ^ +s
        //          |     ^           |
        //          v     |           o--> -r
        // (s0,r00) * --> o (s0,r01)
        //
        SrhFace srh_face({
            {s0, r00, elevation(s0, r00)},
            {s0, r01, elevation(s0, r01)},
            {s1, r11, elevation(s1, r11)},
            {s1, r10, elevation(s1, r10)}}, {0., 0., 1.});
        mesh->PushFace(srh_face.ToGeoFace(lane));

        r00 -= grid_unit;
        r10 -= grid_unit;
      }
    }
  }
}


// Adds faces to @p mesh which draw stripes along the lane_bounds() of
// @p lane.
//
// @param mesh  the GeoMesh which will receive the faces
// @param lane  the api::Lane to provide the bounds and surface
// @param grid_unit  longitudinal size (s dimension) of each face
// @param h_offset  h value of each vertex (height above road surface)
// @param stripe_width  width (r dimension) of each stripe
void StripeLaneBounds(GeoMesh* mesh, const api::Lane* lane,
                      double grid_unit, double h_offset,
                      double stripe_width) {
  const double half_stripe = 0.5 * stripe_width;

  const double s_max = lane->length();
  for (double s0 = 0; s0 < s_max; s0 += grid_unit) {
    double s1 = s0 + grid_unit;
    if (s1 > s_max) { s1 = s_max; }

    api::RBounds rb0 = lane->lane_bounds(s0);
    api::RBounds rb1 = lane->lane_bounds(s1);

    // Left side of lane.
    {
      SrhFace srh_face({
          {s0, rb0.max() - half_stripe, h_offset},
          {s1, rb1.max() - half_stripe, h_offset},
          {s1, rb1.max() + half_stripe, h_offset},
          {s0, rb0.max() + half_stripe, h_offset}}, {0., 0., 1.});
      mesh->PushFace(srh_face.ToGeoFace(lane));
    }
    // Right side of lane.
    {
      SrhFace srh_face({
          {s0, rb0.min() - half_stripe, h_offset},
          {s1, rb1.min() - half_stripe, h_offset},
          {s1, rb1.min() + half_stripe, h_offset},
          {s0, rb0.min() + half_stripe, h_offset}}, {0., 0., 1.});
      mesh->PushFace(srh_face.ToGeoFace(lane));
    }
  }
}


// Adds faces to @p mesh which draw a simple triangular arrow in the
// `Lane`-frame of @p lane.  The width of the arrow is fixed at 80% of
// the lane_bounds() of @p lane at the base of the arrow.
//
// @param mesh  the GeoMesh which will receive the faces
// @param lane  the api::Lane to provide the bounds and surface
// @param grid_unit  size of each quad (length of edge in s and r dimensions)
// @param s_offset  longitudinal offset of the base of the arrow from the
//                  beginning (s = 0) of @p lane
// @param s_size  length of the arrow from base to tip
// @param h_offset  h value of each vertex (height above road surface)
void DrawLaneArrow(GeoMesh* mesh, const api::Lane* lane, double grid_unit,
                   double s_offset, double s_size, double h_offset) {
  DELPHYNE_DEMAND(s_offset >= 0.);
  DELPHYNE_DEMAND((s_offset + s_size) <= lane->length());

  const double kRelativeWidth = 0.8;

  const api::RBounds rb0 = lane->lane_bounds(s_offset);

  const int max_num_s_units = static_cast<int>(std::ceil(s_size / grid_unit));

  const double rl_size = rb0.max() * kRelativeWidth;
  const double rr_size = -rb0.min() * kRelativeWidth;
  const int max_num_rl_units = static_cast<int>(std::ceil(rl_size / grid_unit));
  const int max_num_rr_units = static_cast<int>(std::ceil(rr_size / grid_unit));

  const int num_units = std::max(max_num_s_units,
                                 std::max(max_num_rl_units,
                                          max_num_rr_units));
  DELPHYNE_DEMAND(num_units >= 1);
  const double s_unit = s_size / num_units;
  const double rl_unit = rl_size / num_units;
  const double rr_unit = rr_size / num_units;

  int num_r_units = num_units;
  for (int si = 0; si < num_units; ++si) {
    double s0 = s_offset + (si * s_unit);
    double s1 = s_offset + ((si + 1.) * s_unit);
    // Left side of lane.
    {
      double r00 = 0.;
      double r10 = 0.;
      for (int ri = 0; ri < (num_r_units - 1); ++ri) {
        const double r01 = r00 + rl_unit;
        const double r11 = r10 + rl_unit;
        //
        // (s1,r11) o <-- o (s1,r10)       ^ +s
        //          |     ^                |
        //          v     |          +r <--o
        // (s0,r01) o --> * (s0,r00)
        //
        SrhFace srh_face({
            {s0, r00, h_offset},
            {s1, r10, h_offset},
            {s1, r11, h_offset},
            {s0, r01, h_offset}}, {0., 0., 1.});
        mesh->PushFace(srh_face.ToGeoFace(lane));

        r00 += rl_unit;
        r10 += rl_unit;
      }
      //                o (s1,r10)       ^ +s
      //              / ^                |
      //            /   |          +r <--o
      // (s0,r01) o --> * (s0,r00)
      SrhFace srh_face({
          {s0, r00, h_offset},
          {s1, r10, h_offset},
          {s0, r00 + rl_unit, h_offset}}, {0., 0., 1.});
      mesh->PushFace(srh_face.ToGeoFace(lane));
    }
    // Right side of lane.
    {
      double r00 = 0.;
      double r10 = 0.;
      for (int ri = 0; ri < (num_r_units - 1); ++ri) {
        const double r01 = r00 - rr_unit;
        const double r11 = r10 - rr_unit;
        //
        // (s1,r10) o <-- o (s1,r11)  ^ +s
        //          |     ^           |
        //          v     |           o--> -r
        // (s0,r00) * --> o (s0,r01)
        //
        SrhFace srh_face({
            {s0, r00, h_offset},
            {s0, r01, h_offset},
            {s1, r11, h_offset},
            {s1, r10, h_offset}}, {0., 0., 1.});
        mesh->PushFace(srh_face.ToGeoFace(lane));

        r00 -= rr_unit;
        r10 -= rr_unit;
      }
      //
      // (s1,r10) o                 ^ +s
      //          | \               |
      //          v   \             o--> -r
      // (s0,r00) * --> o (s0,r01)
      //
      SrhFace srh_face({
          {s0, r00, h_offset},
          {s0, r00 - rr_unit, h_offset},
          {s1, r10, h_offset}}, {0., 0., 1.});
      mesh->PushFace(srh_face.ToGeoFace(lane));
    }

    num_r_units -= 1;
  }
}


// Marks the start and finish ends of @p lane with arrows, rendered into
// @p mesh.
//
// @param mesh  the GeoMesh which will receive the arrows
// @param lane  the api::Lane to provide the surface
// @param grid_unit  size of each quad (length of edge in s and r dimensions)
// @param h_offset  h value of each vertex (height above road surface)
void MarkLaneEnds(GeoMesh* mesh, const api::Lane* lane, double grid_unit,
                  double h_offset) {
  // To avoid crossing boundaries (and tripping assertions) due to
  // numeric precision issues, we will nudge the arrows inward from
  // the ends of the lanes by the RoadGeometry's linear_tolerance().
  const double nudge =
      lane->segment()->junction()->road_geometry()->linear_tolerance();
  const double max_length = 0.3 * lane->length();
  // Arrows are sized relative to their respective ends.
  const api::RBounds start_rb = lane->lane_bounds(0.);
  const double start_s_size = std::min(max_length,
                                       (start_rb.max() - start_rb.min()));

  const api::RBounds finish_rb = lane->lane_bounds(lane->length());
  const double finish_s_size = std::min(max_length,
                                        (finish_rb.max() - finish_rb.min()));

  DrawLaneArrow(mesh, lane, grid_unit,
                0. + nudge, start_s_size, h_offset);
  DrawLaneArrow(mesh, lane, grid_unit,
                lane->length() - finish_s_size - nudge, finish_s_size,
                h_offset);
}


// Calculates an appropriate grid-unit size for @p lane.
double PickGridUnit(const api::Lane* lane,
                    double max_size, double min_resolution) {
  double result = max_size;
  const api::RBounds rb0 = lane->lane_bounds(0.);
  const api::RBounds rb1 = lane->lane_bounds(lane->length());
  result = std::min(result, (rb0.max() - rb0.min()) / min_resolution);
  result = std::min(result, (rb1.max() - rb1.min()) / min_resolution);
  result = std::min(result, lane->length() / min_resolution);
  return result;
}


// Renders a BranchPoint @p branch_point as a collection of pointy
// arrows for each branch.  @p base_elevation is the desired elevation
// of the center of the rendering (above the road surface), and
// @p height is the vertical size of rendering.  The actual elevation
// may be raised in order to avoid overlapping other nearby
// BranchPoints.  @p mesh is the mesh into which the rendering occurs.
// @p previous_centers is a list of the world-frame positions of the
// centers of previously rendered BranchPoints (in order to avoid
// overlaps with them); this list will be updated with the rendered
// center of this BranchPoint.
void RenderBranchPoint(
    const api::BranchPoint* const branch_point,
    const double base_elevation, const double height,
    GeoMesh* mesh,
    std::vector<api::GeoPosition>* previous_centers) {
  if ((branch_point->GetASide()->size() == 0) &&
      (branch_point->GetBSide()->size() == 0)) {
    // No branches?  Odd, but, oh, well... nothing to do here.
    return;
  }

  // Arbitrarily pick one of the LaneEnds in the BranchPoint as a reference
  // for its geometry (e.g., *where* is the BranchPoint).
  const api::LaneEnd reference_end =
      (branch_point->GetASide()->size() > 0) ?
      branch_point->GetASide()->get(0) :
      branch_point->GetBSide()->get(0);
  const double reference_end_s =
      (reference_end.end == api::LaneEnd::kStart) ? 0. :
      reference_end.lane->length();
  const api::RBounds reference_bounds =
      reference_end.lane->lane_bounds(reference_end_s);
  const double sr_margin = reference_bounds.max() - reference_bounds.min();
  const double h_margin = height;

  // Choose an elevation that keeps this BranchPoint out of the way
  // of previously rendered BranchPoints.
  double elevation = base_elevation;
  bool has_conflict = true;
  while (has_conflict) {
    // Calculate center in world-frame with current elevation.
    const api::LanePosition center_srh(
        (reference_end.end == api::LaneEnd::kStart) ? 0. :
        reference_end.lane->length(),
        0., elevation);
    const api::Rotation orientation =
        reference_end.lane->GetOrientation(center_srh);
    const api::GeoPosition center_xyz =
        reference_end.lane->ToGeoPosition(center_srh);

    has_conflict = false;
    // Compare center against every already-rendered center....
    // If distance in sr-plane is too close and distance along h-axis is
    // too close, then increase elevation and try again.
    for (const api::GeoPosition& previous_xyz : *previous_centers) {
      const Vector3<double> delta_xyz = previous_xyz.xyz() - center_xyz.xyz();
      const Vector3<double> delta_srh =
          orientation.matrix().transpose() * delta_xyz;

      if ((Vector2<double>(delta_srh.x(), delta_srh.y()).norm() < sr_margin) &&
          (std::abs(delta_srh.z()) < h_margin)) {
        has_conflict = true;
        elevation += height;
        break;
      }
    }

    if (!has_conflict) {
      previous_centers->emplace_back(center_xyz);
    }
  }

  // Finally, draw the BranchPoint as:
  // - a single vertical diamond, facing into the lane of reference_end;
  // - for each branch (LaneEnd), an arrow formed from a pair of very
  //   pointy trapezoids (one in the sr-plane, one in the sh-plane) pointing
  //   into the lane.
  static const double kWidthFactor = 0.1;
  static const double kTipFactor = 0.1;
  static const double kLengthFactor = 1.0;
  static const double kMaxLengthFraction = 0.4;

  // Helper to draw a LaneEnd as either diamond or arrow.
  const auto draw_branch =
      [elevation, height, &mesh](const api::LaneEnd& lane_end,
                                 bool as_diamond) {
    const double end_s =
      (lane_end.end == api::LaneEnd::kStart) ? 0. : lane_end.lane->length();
    const api::RBounds r_bounds = lane_end.lane->lane_bounds(end_s);

    const double half_width =
      (r_bounds.max() - r_bounds.min()) * kWidthFactor * 0.5;
    const double length =
      std::min(kMaxLengthFraction * lane_end.lane->length(),
               kLengthFactor * (r_bounds.max() - r_bounds.min())) *
      ((lane_end.end == api::LaneEnd::kStart) ? 1. : -1);

    const double left_r =
      half_width * ((lane_end.end == api::LaneEnd::kStart) ? 1. : -1);
    const double right_r = -left_r;

    if (as_diamond) {
      SrhFace srh_face({
          {end_s, 0., elevation - (0.5 * height)},
          {end_s, right_r, elevation},
          {end_s, 0., elevation + (0.5 * height)},
          {end_s, left_r, elevation}},
        api::LanePosition{(end_s == 0. ? 1. : -1), 0., 0.});
      mesh->PushFace(srh_face.ToGeoFace(lane_end.lane));
    } else {
      SrhFace srh_face1({
          {end_s, left_r, elevation},
          {end_s, right_r, elevation},
          {end_s + length, right_r * kTipFactor, elevation},
          {end_s + length, left_r * kTipFactor, elevation}},
        api::LanePosition{0., 0., 1.});
      SrhFace srh_face2({
          {end_s, 0., elevation - (0.5 * height)},
          {end_s, 0., elevation + (0.5 * height)},
          {end_s + length, 0., elevation + (0.5 * kTipFactor * height)},
          {end_s + length, 0., elevation - (0.5 * kTipFactor * height)}
        },
        api::LanePosition{0., (length > 0. ? 1. : -1.), 0.});
      mesh->PushFace(srh_face1.ToGeoFace(lane_end.lane));
      mesh->PushFace(srh_face2.ToGeoFace(lane_end.lane));
    }
  };

  // Helper to draw all LaneEnds in a LaneEndSet as arrows.
  const auto draw_arrows = [&draw_branch](const api::LaneEndSet* set) {
    for (int i = 0; i < set->size(); ++i) {
      draw_branch(set->get(i), false);
    }
  };

  draw_branch(reference_end, true /* as_diamond */);
  draw_arrows(branch_point->GetASide());
  draw_arrows(branch_point->GetBSide());
}


void RenderSegment(const api::Segment* segment,
                   const Features& features,
                   GeoMesh* asphalt_mesh,
                   GeoMesh* lane_mesh,
                   GeoMesh* marker_mesh,
                   GeoMesh* h_bounds_mesh) {
  // Lane 0 should be as good as any other for driveable-bounds.
  CoverLaneWithQuads(asphalt_mesh, segment->lane(0),
                     PickGridUnit(segment->lane(0),
                                  features.max_grid_unit,
                                  features.min_grid_resolution),
                     true /*use_driveable_bounds*/,
                     [](double, double) { return 0.; });
  if (features.draw_elevation_bounds) {
    CoverLaneWithQuads(
        h_bounds_mesh,
        segment->lane(0),
        PickGridUnit(segment->lane(0),
                     features.max_grid_unit,
                     features.min_grid_resolution),
        true /*use_driveable_bounds*/,
        [&segment](double s, double r) {
          return segment->lane(0)->elevation_bounds(s, r).max(); });
    CoverLaneWithQuads(
        h_bounds_mesh,
        segment->lane(0),
        PickGridUnit(segment->lane(0),
                     features.max_grid_unit,
                     features.min_grid_resolution),
        true /*use_driveable_bounds*/,
        [&segment](double s, double r) {
          return segment->lane(0)->elevation_bounds(s, r).min(); });
  }
  for (int li = 0; li < segment->num_lanes(); ++li) {
    const api::Lane* lane = segment->lane(li);
    const double grid_unit = PickGridUnit(lane,
                                          features.max_grid_unit,
                                          features.min_grid_resolution);
    if (features.draw_lane_haze) {
      CoverLaneWithQuads(lane_mesh, lane, grid_unit,
                         false /*use_driveable_bounds*/,
                         [&features](double, double) {
                           return features.lane_haze_elevation;
                         });
    }
    if (features.draw_stripes) {
      StripeLaneBounds(marker_mesh, lane, grid_unit,
                       features.stripe_elevation,
                       features.stripe_width);
    }
    if (features.draw_arrows) {
      MarkLaneEnds(marker_mesh, lane, grid_unit,
                   features.arrow_elevation);
    }
  }
}


bool IsSegmentRenderedNormally(const api::SegmentId& id,
                               const std::vector<api::SegmentId>& highlights) {
  if (highlights.empty()) {
    return true;
  }
  for (const api::SegmentId& highlighted_id : highlights) {
    if (id == highlighted_id) {
      return true;
    }
  }
  return false;
}


// Sorts a vector of tuples composed of 3D points and an index in a polar and
// counterclockwise way.
//
// Points in `unordered_vector` are supposed to be on the same plane. Based on
// that, the mid point of all of them is computed and then used as a center of
// coordinates. Angles are computed using that center and the compared to sort
// the vector.
// @returns A std::vector<std::tuple<ignition::math::Vector3d, int>> with the
// same items of `unordered_vector` but sorted in a polar and counterclockwise
// order.
// @throws std::runtime_error When `unordered_vector`' size is zero.
std::vector<std::tuple<ignition::math::Vector3d, int>> PolarSort(
    const std::vector<std::tuple<ignition::math::Vector3d, int>>&
        unordered_vector) {
  DELPHYNE_DEMAND(unordered_vector.size() > 0);

  std::vector<std::tuple<ignition::math::Vector3d, int>> ordered_vector =
      unordered_vector;

  // Computes the center.
  std::vector<ignition::math::Vector3d> points;
  ignition::math::Vector3d center;
  for(const std::tuple<ignition::math::Vector3d, int>& vertex_index :
      unordered_vector) {
    center += std::get<0>(vertex_index);
  }
  center /= static_cast<double>(points.size());

  // TODO(agalbachicar)    This polar sort should be done with respect to the
  //                       plane of all the points in the face instead of the
  //                       plane z=0. We need the normal of the plane to do
  //                       so.
  std::sort(ordered_vector.begin(), ordered_vector.end(),
      [center](const std::tuple<ignition::math::Vector3d, int>& a,
               const std::tuple<ignition::math::Vector3d, int>& b) {
        double a_y = std::atan2(std::get<0>(a).Y() - center.Y(),
                                      std::get<0>(a).X() - center.X());
        double b_y = std::atan2(std::get<0>(b).Y() - center.Y(),
                                std::get<0>(b).X() - center.X());
        if (a_y < 0) a_y += 2 * M_PI;
        if (b_y < 0) b_y += 2 * M_PI;
        return a_y < b_y;
      });
  return ordered_vector;
}

// This map holds the properties of different materials. Those properties were
// taken from the original .mtl description that lives in GenerateObjFile().
const std::map<std::string, Material> kMaterial{
  {"asphalt",  {{0.2, 0.2, 0.2}, {0.1, 0.1, 0.1}, {0.3, 0.3, 0.3}, 10., 0.0}},
  {"lane",     {{0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, 10., 0.8}},
  {"marker",   {{0.8, 0.8, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.5}, 10., 0.5}},
  {"h_bounds", {{0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, 10., 0.8}},
  {"branch_point", {{0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, 10., 0.9}},
  {"grayed_asphalt", {{0.1, 0.1, 0.1}, {0.2, 0.2, 0.2}, {0.3, 0.3, 0.3}, 10., 0.9}},
  {"grayed_lane", {{0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, 10., 0.9}},
  {"grayed_marker", {{0.8, 0.8, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.5}, 10., 0.9}}
};

}  // namespace


std::map<std::string, GeoMesh> BuildMeshes(const api::RoadGeometry* rg,
                                           const Features& features) {
  GeoMesh asphalt_mesh;
  GeoMesh lane_mesh;
  GeoMesh marker_mesh;
  GeoMesh h_bounds_mesh;
  GeoMesh branch_point_mesh;

  GeoMesh grayed_asphalt_mesh;
  GeoMesh grayed_lane_mesh;
  GeoMesh grayed_marker_mesh;

  // Walk the network.
  for (int ji = 0; ji < rg->num_junctions(); ++ji) {
    const api::Junction* junction = rg->junction(ji);
    for (int si = 0; si < junction->num_segments(); ++si) {
      const api::Segment* segment = junction->segment(si);
      // TODO(maddog@tri.global)  Id's need well-defined comparison semantics.
      if (IsSegmentRenderedNormally(segment->id(),
                                    features.highlighted_segments)) {
        RenderSegment(segment, features,
                      &asphalt_mesh, &lane_mesh, &marker_mesh, &h_bounds_mesh);
      } else {
        RenderSegment(segment, features,
                      &grayed_asphalt_mesh, &grayed_lane_mesh,
                      &grayed_marker_mesh, &h_bounds_mesh);
      }
    }
  }

  if (features.draw_branch_points) {
    std::vector<api::GeoPosition> rendered_centers;
    for (int bpi = 0; bpi < rg->num_branch_points(); ++bpi) {
      const api::BranchPoint* branch_point = rg->branch_point(bpi);
      RenderBranchPoint(branch_point,
                        features.branch_point_elevation,
                        features.branch_point_height,
                        &branch_point_mesh,
                        &rendered_centers);
    }
  }

  std::map<std::string, GeoMesh> meshes;
  meshes["asphalt"] = asphalt_mesh;
  meshes["lane"] = lane_mesh;
  meshes["marker"] = marker_mesh;
  meshes["h_bounds"] = h_bounds_mesh;
  meshes["branch_point"] = branch_point_mesh;
  meshes["grayed_asphalt"] = grayed_asphalt_mesh;
  meshes["grayed_lane"] = grayed_lane_mesh;
  meshes["grayed_marker"] = grayed_marker_mesh;
  return meshes;
}

void GenerateObjFile(const api::RoadGeometry* rg,
                     const std::string& dirpath,
                     const std::string& fileroot,
                     const Features& features) {
  std::map<std::string, GeoMesh> meshes = BuildMeshes(rg, features);
  const GeoMesh& asphalt_mesh = meshes["asphalt"];
  const GeoMesh& lane_mesh = meshes["lane"];
  const GeoMesh& marker_mesh = meshes["marker"];
  const GeoMesh& h_bounds_mesh = meshes["h_bounds"];
  const GeoMesh& branch_point_mesh = meshes["branch_point"];
  const GeoMesh& grayed_asphalt_mesh = meshes["grayed_asphalt"];
  const GeoMesh& grayed_lane_mesh = meshes["grayed_lane"];
  const GeoMesh& grayed_marker_mesh = meshes["grayed_marker"];

  const std::string kLaneHaze("lane_haze");
  const std::string kMarkerPaint("marker_paint");
  const std::string kBlandAsphalt("bland_asphalt");
  const std::string kBranchPointGlow("branch_point_glow");
  const std::string kHBoundsHaze("h_bounds_haze");

  const std::string kGrayedLaneHaze("grayed_lane_haze");
  const std::string kGrayedMarkerPaint("grayed_marker_paint");
  const std::string kGrayedBlandAsphalt("grayed_bland_asphalt");

  const std::string obj_filename = fileroot + ".obj";
  const std::string mtl_filename = fileroot + ".mtl";

  // Create the requested OBJ file.
  {
    // Figure out the fixed-point precision necessary to render OBJ vertices
    // with enough precision relative to linear_tolerance().
    //
    // Given linear_tolerance ε, we conservatively want to bound the rendering
    // error per component to `ε / (sqrt(3) * 10)`.  The `sqrt(3)` is
    // because the worst-case error in total 3-space distance is `sqrt(3)`
    // times the per-component error.  The `10` is a fudge-factor to ensure
    // that the "rendering error in an OBJ vertex with respect to the
    // maliput-expressed value" is within 10% of the "error-bound between
    // the maliput-expressed position and the underlying ground-truth".
    // In other words, we're aiming for the rendered vertex to be within
    // 110% ε of the ground-truth position.
    //
    // The bound on error due to rounding to `n` places is `0.5 * 10^(-n)`,
    // so we want `n` such that `0.5 * 10^(-n) < ε / (sqrt(3) * 10)`.
    // This yields:  `n > log10(sqrt(3) * 5) - log10(ε)`.
    DELPHYNE_DEMAND(rg->linear_tolerance() > 0.);
    const int precision =
        std::max(0., std::ceil(std::log10(std::sqrt(3.) * 5.) -
                               std::log10(rg->linear_tolerance())));

    std::ofstream os(obj_filename, std::ios::binary);
    fmt::print(os,
               R"X(# GENERATED BY maliput::utility::GenerateObjFile()
#
# DON'T BE A HERO.  Do not edit by hand.

mtllib {}
)X",
               mtl_filename);
    int vertex_index_offset = 0;
    int normal_index_offset = 0;
    std::tie(vertex_index_offset, normal_index_offset) =
        asphalt_mesh.EmitObj(os, kBlandAsphalt,
                             precision, features.origin,
                             vertex_index_offset, normal_index_offset);
    std::tie(vertex_index_offset, normal_index_offset) =
        lane_mesh.EmitObj(os, kLaneHaze,
                          precision, features.origin,
                          vertex_index_offset, normal_index_offset);
    std::tie(vertex_index_offset, normal_index_offset) =
        marker_mesh.EmitObj(os, kMarkerPaint,
                            precision, features.origin,
                            vertex_index_offset, normal_index_offset);
    std::tie(vertex_index_offset, normal_index_offset) =
        branch_point_mesh.EmitObj(os, kBranchPointGlow,
                                  precision, features.origin,
                                  vertex_index_offset, normal_index_offset);

    std::tie(vertex_index_offset, normal_index_offset) =
        grayed_asphalt_mesh.EmitObj(os, kGrayedBlandAsphalt,
                                    precision, features.origin,
                                    vertex_index_offset, normal_index_offset);
    std::tie(vertex_index_offset, normal_index_offset) =
        grayed_lane_mesh.EmitObj(os, kGrayedLaneHaze,
                                 precision, features.origin,
                                 vertex_index_offset, normal_index_offset);
    std::tie(vertex_index_offset, normal_index_offset) =
        grayed_marker_mesh.EmitObj(os, kGrayedMarkerPaint,
                                   precision, features.origin,
                                   vertex_index_offset, normal_index_offset);

    std::tie(vertex_index_offset, normal_index_offset) =
        h_bounds_mesh.EmitObj(os, kHBoundsHaze,
                              precision, features.origin,
                              vertex_index_offset, normal_index_offset);
  }

  // Create the MTL file referenced by the OBJ file.
  {
    std::ofstream os(mtl_filename, std::ios::binary);
    fmt::print(os,
               R"X(# GENERATED BY maliput::utility::GenerateObjFile()
#
# DON'T BE A HERO.  Do not edit by hand.

newmtl {}
Ka 0.8 0.8 0.0
Kd 1.0 1.0 0.0
Ks 1.0 1.0 0.5
Ns 10.0
illum 2
d 0.5

newmtl {}
Ka 0.1 0.1 0.1
Kd 0.2 0.2 0.2
Ks 0.3 0.3 0.3
Ns 10.0
illum 2

newmtl {}
Ka 0.9 0.9 0.9
Kd 0.9 0.9 0.9
Ks 0.9 0.9 0.9
Ns 10.0
illum 2
d 0.20

newmtl {}
Ka 0.0 0.0 1.0
Kd 0.0 0.0 1.0
Ks 0.0 0.0 1.0
Ns 10.0
illum 2
d 0.80

newmtl {}
Ka 0.8 0.8 0.0
Kd 1.0 1.0 0.0
Ks 1.0 1.0 0.5
Ns 10.0
illum 2
d 0.1

newmtl {}
Ka 0.1 0.1 0.1
Kd 0.2 0.2 0.2
Ks 0.3 0.3 0.3
Ns 10.0
illum 2
d 0.10

newmtl {}
Ka 0.9 0.9 0.9
Kd 0.9 0.9 0.9
Ks 0.9 0.9 0.9
Ns 10.0
illum 2
d 0.10

newmtl {}
Ka 0.0 0.0 1.0
Kd 0.0 0.0 1.0
Ks 0.0 0.0 1.0
Ns 10.0
illum 2
d 0.20
)X",
               kMarkerPaint, kBlandAsphalt, kLaneHaze, kBranchPointGlow,
               kGrayedMarkerPaint, kGrayedBlandAsphalt, kGrayedLaneHaze,
               kHBoundsHaze);
  }
}

std::unique_ptr<ignition::common::Mesh> Convert(const std::string& name,
                                                const GeoMesh& geo_mesh) {
  // Checks before actually creating the mesh.
  if (geo_mesh.num_vertices() < 3 && geo_mesh.get_index_faces().size() < 1) {
    return nullptr;
  }

  auto mesh = std::make_unique<ignition::common::Mesh>();
  mesh->SetName(name);

  auto sub_mesh = std::make_unique<ignition::common::SubMesh>();
  sub_mesh->SetPrimitiveType(ignition::common::SubMesh::TRIANGLES);

  auto geo_position_to_ign_vector = [](
      const drake::maliput::api::GeoPosition& v) {
    return ignition::math::Vector3d(v.x(), v.y(), v.z());
  };

  // Adds vertices, fake normals and fake texture coordinates. Each vertex needs
  // a normal and a texture coordinate. Fake texture coordinates are added given
  // that Drake's meshes do not have any information about that and normals are
  // filled with stubs to match the same number of vertices. Later, when
  // iterating through faces, normals will be filled with the correct value.
  // Note that geo_mesh may not have the same number of vertices as normals
  // given that it keeps no duplicates.
  for (int i = 0; i < geo_mesh.num_vertices(); ++i) {
    sub_mesh->AddVertex(
        geo_position_to_ign_vector(geo_mesh.get_vertex(i).v()));
    sub_mesh->AddNormal({0, 0, 1});
    sub_mesh->AddTexCoord({0, 0});
  }


  // Sets the indices based on how the faces were built.
  for (const IndexFace& index_face : geo_mesh.get_index_faces()) {
    // TODO(agalbachicar):    I'm assuming that IndexFace will not have more
    //                        than 4 vertices. The class supports more, however
    //                        proper triangulation code needs to be done so as
    //                        to support it.
    DELPHYNE_DEMAND(index_face.vertices().size() == 3 ||
                    index_face.vertices().size() == 4);

    std::vector<std::tuple<ignition::math::Vector3d, int>>
        ordered_vertices_indices;
    for (const IndexFace::Vertex& ifv : index_face.vertices()) {
      // Sets the correct normal.
      const drake::maliput::api::GeoPosition normal =
          geo_mesh.get_normal(ifv.normal_index).n();
      sub_mesh->SetNormal(ifv.vertex_index, geo_position_to_ign_vector(normal));
      // Adds the vertices to the vector so we can later order them.
      ordered_vertices_indices.push_back(
          std::make_tuple<ignition::math::Vector3d, int>(
              sub_mesh->Vertex(ifv.vertex_index), int(ifv.vertex_index)));
    }
    ordered_vertices_indices = PolarSort(ordered_vertices_indices);

    sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[0]));
    sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[1]));
    sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[2]));

    sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[2]));
    sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[1]));
    sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[0]));
    // Includes the remaining triangle.
    if (index_face.vertices().size() == 4) {
      sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[2]));
      sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[3]));
      sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[0]));

      sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[0]));
      sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[3]));
      sub_mesh->AddIndex(std::get<1>(ordered_vertices_indices[2]));
    }
  }

  mesh->AddSubMesh(std::move(sub_mesh));
  return mesh;
}

std::unique_ptr<Material> GetMaterialByName(const std::string& material_name) {
  if (kMaterial.find(material_name) == kMaterial.end()) {
    return nullptr;
  }
  return std::make_unique<Material>(kMaterial.at(material_name));
}

}  // namespace mesh
}  // namespace maliput
}  // namespace drake
