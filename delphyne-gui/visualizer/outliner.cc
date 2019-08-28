// Copyright 2019 Toyota Research Institute

#include "outliner.hh"

#include <delphyne/macros.h>

#include <ignition/common/Console.hh>
#include <ignition/math/Helpers.hh>
#include <ignition/math/Matrix4.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/rendering/Material.hh>
#include <ignition/rendering/Mesh.hh>
#include <ignition/rendering/Scene.hh>

#include <maliput/api/lane.h>
#include <maliput/api/lane_data.h>

namespace delphyne {
namespace gui {

Outliner::Outliner(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ,
                   size_t _poolSize, double _minTolerance)
    : cubes(_poolSize),
      lastLaneOutlined(nullptr),
      lastCubesUsed(0),
      minTolerance(_minTolerance < _scaleX * 2.0 ? _scaleX : _minTolerance) {
  DELPHYNE_DEMAND(_poolSize > 3);
  ignition::rendering::MaterialPtr material = _scene->CreateMaterial();
  material->SetDiffuse(255.0, 0.0, 0.0, 1.0);
  material->SetAmbient(255.0, 0.0, 0.0, 1.0);
  this->CreateCubes(_scene, _scaleX, _scaleY, _scaleZ, material);
}

void Outliner::OutlineLane(const maliput::api::Lane* _lane) {
  // Don't iterate through the same lane twice.
  if (_lane == lastLaneOutlined) {
    return;
  }
  lastLaneOutlined = _lane;
  const double max_s = _lane->length();

  const maliput::api::RBounds initialRBounds = _lane->lane_bounds(0.);
  const maliput::api::RBounds endRBounds = _lane->lane_bounds(max_s);

  const maliput::api::GeoPosition initialRMinGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(0., initialRBounds.min(), 0.));
  const maliput::api::GeoPosition initialRMaxGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(0., initialRBounds.max(), 0.));

  const maliput::api::GeoPosition endRMinGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(max_s, endRBounds.min(), 0.));
  const maliput::api::GeoPosition endRMaxGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(max_s, endRBounds.max(), 0.));

  // Set cubes in the extremes of the lane.
  cubes[0]->SetWorldPosition(initialRMinGeoPos.x(), initialRMinGeoPos.y(), initialRMinGeoPos.z());
  cubes[0]->SetVisible(true);
  cubes[1]->SetWorldPosition(initialRMaxGeoPos.x(), initialRMaxGeoPos.y(), initialRMaxGeoPos.z());
  cubes[1]->SetVisible(true);

  cubes[2]->SetWorldPosition(endRMinGeoPos.x(), endRMinGeoPos.y(), endRMinGeoPos.z());
  cubes[2]->SetVisible(true);
  cubes[3]->SetWorldPosition(endRMaxGeoPos.x(), endRMaxGeoPos.y(), endRMaxGeoPos.z());
  cubes[3]->SetVisible(true);

  size_t cubesUsed = 4;
  size_t remainingCubes = cubes.size() - cubesUsed;

  MoveCubeAtMidPointInR(initialRMinGeoPos, initialRMaxGeoPos, &cubesUsed, &remainingCubes);

  MoveCubeAtMidPointInR(endRMinGeoPos, endRMaxGeoPos, &cubesUsed, &remainingCubes);

  size_t cubesLeftSide = std::ceil(remainingCubes / 2);
  size_t cubesRightSide = remainingCubes - cubesLeftSide;
  double oldTolerance = minTolerance;

  // If we have less cubes to cover the lane, increase the tolerance.
  minTolerance = GetNewToleranceToPopulateLane(max_s, cubesLeftSide);
  cubesLeftSide = static_cast<size_t>(max_s / minTolerance);
  MoveCubeAtMidPointInS(_lane, 0., max_s, true, &cubesUsed, &cubesLeftSide);

  minTolerance = GetNewToleranceToPopulateLane(max_s, cubesRightSide);
  cubesRightSide = static_cast<size_t>(max_s / minTolerance);
  MoveCubeAtMidPointInS(_lane, 0., max_s, false, &cubesUsed, &cubesRightSide);

  SetVisibilityOfCubesStartingFromTo(cubesUsed, lastCubesUsed != 0 ? lastCubesUsed : cubes.size(), false);
  lastCubesUsed = cubesUsed;
  minTolerance = oldTolerance;
}

void Outliner::SetVisibility(bool _visible) {
  SetVisibilityOfCubesStartingFromTo(0, lastCubesUsed, _visible);
  lastCubesUsed = 0;
  lastLaneOutlined = nullptr;
}

void Outliner::CreateCubes(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ,
                           ignition::rendering::MaterialPtr& _material) {
  for (size_t i = 0; i < cubes.size(); ++i) {
    ignition::rendering::VisualPtr cube = _scene->CreateVisual();
    cube->AddGeometry(_scene->CreateBox());
    cube->SetMaterial(_material);
    cube->SetVisible(false);
    cube->SetWorldScale(_scaleX, _scaleY, _scaleZ);
    cube->SetWorldPosition(0., 0., 0.);
    _scene->RootVisual()->AddChild(cube);
    this->cubes[i] = cube;
  }
}

double Outliner::GetNewToleranceToPopulateLane(double _laneLength, size_t _cubesUsedForSide) {
  const double newToleranceForLaneSide = _laneLength / _cubesUsedForSide;
  return newToleranceForLaneSide < minTolerance ? minTolerance : newToleranceForLaneSide;
}

void Outliner::MoveCubeAtMidPointInR(const maliput::api::GeoPosition& _minRGeoPos,
                                     const maliput::api::GeoPosition& _maxRGeoPos, size_t* _cubesUsed,
                                     size_t* _maxAmountOfCubesToUse) {
  maliput::api::GeoPosition midPoint = maliput::api::GeoPosition::FromXyz((_maxRGeoPos.xyz() + _minRGeoPos.xyz()) / 2);
  if ((_maxRGeoPos - midPoint).length() > minTolerance && *_cubesUsed < cubes.size() && *_maxAmountOfCubesToUse != 0) {
    cubes[*_cubesUsed]->SetWorldPosition(midPoint.x(), midPoint.y(), midPoint.z());
    cubes[*_cubesUsed]->SetVisible(true);
    ++(*_cubesUsed);
    --(*_maxAmountOfCubesToUse);
    MoveCubeAtMidPointInR(_minRGeoPos, midPoint, _cubesUsed, _maxAmountOfCubesToUse);
    MoveCubeAtMidPointInR(midPoint, _maxRGeoPos, _cubesUsed, _maxAmountOfCubesToUse);
  }
}

void Outliner::MoveCubeAtMidPointInS(const maliput::api::Lane* _lane, double min_s, double max_s, bool _left_side,
                                     size_t* _cubesUsed, size_t* _maxAmountOfCubesToUse) {
  const double mid_s = (max_s + min_s) / 2.0;
  maliput::api::RBounds midRBounds = _lane->lane_bounds(mid_s);
  const double r_bound = _left_side ? midRBounds.min() : midRBounds.max();

  maliput::api::GeoPosition midPoint = _lane->ToGeoPosition(maliput::api::LanePosition(mid_s, r_bound, 0.));
  maliput::api::GeoPosition extremePoint = _lane->ToGeoPosition(maliput::api::LanePosition(max_s, r_bound, 0.));

  if ((midPoint - extremePoint).length() > minTolerance && *_cubesUsed < cubes.size() && *_maxAmountOfCubesToUse != 0) {
    ignition::math::Vector3d extremeMidPointMathVector(extremePoint.x(), extremePoint.y(), extremePoint.z());
    cubes[*_cubesUsed]->SetWorldPosition(midPoint.x(), midPoint.y(), midPoint.z());
    cubes[*_cubesUsed]->SetWorldRotation(
        ignition::math::Matrix4d::LookAt(cubes[*_cubesUsed]->WorldPosition(), extremeMidPointMathVector).Pose().Rot());
    cubes[*_cubesUsed]->SetVisible(true);
    ++(*_cubesUsed);
    --(*_maxAmountOfCubesToUse);
    MoveCubeAtMidPointInS(_lane, mid_s, max_s, _left_side, _cubesUsed, _maxAmountOfCubesToUse);
    MoveCubeAtMidPointInS(_lane, min_s, mid_s, _left_side, _cubesUsed, _maxAmountOfCubesToUse);
  }
}

void Outliner::SetVisibilityOfCubesStartingFromTo(size_t _startFrom, size_t _to, bool _visible) {
  for (size_t i = _startFrom; i < _to; ++i) {
    cubes[i]->SetVisible(_visible);
  }
}

}  // namespace gui
}  // namespace delphyne
