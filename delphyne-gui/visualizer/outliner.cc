// Copyright 2019 Toyota Research Institute

#include "outliner.hh"

#include <delphyne/macros.h>

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
      cubesUsed(0),
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

  maliput::api::RBounds initialRBounds = _lane->lane_bounds(0.);
  maliput::api::RBounds endRBounds = _lane->lane_bounds(max_s);

  maliput::api::GeoPosition initialRMinGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(0., initialRBounds.min(), 0.));
  maliput::api::GeoPosition initialRMaxGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(0., initialRBounds.max(), 0.));

  maliput::api::GeoPosition endRMinGeoPos =
      _lane->ToGeoPosition(maliput::api::LanePosition(max_s, endRBounds.min(), 0.));
  maliput::api::GeoPosition endRMaxGeoPos =
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

  cubesUsed = 4;
  size_t remainingCubes = cubes.size() - cubesUsed;

  MoveCubeAtMidPointInR(initialRMinGeoPos, initialRMaxGeoPos, remainingCubes);

  MoveCubeAtMidPointInR(endRMinGeoPos, endRMaxGeoPos, remainingCubes);

  size_t cubesLeftSide = (remainingCubes / 2) + remainingCubes % 2;
  size_t cubesRightSide = remainingCubes - cubesLeftSide;

  // If we have less cubes to cover the lane, increase the tolerance.
  double newToleranceForLeftSide = max_s / cubesLeftSide;
  double oldTolerance = minTolerance;
  if (newToleranceForLeftSide < minTolerance) {
    cubesLeftSide = static_cast<size_t>(max_s / minTolerance);
  } else {
    minTolerance = newToleranceForLeftSide;
  }
  MoveCubeAtMidPointInSLeftSide(_lane, 0., max_s, cubesLeftSide);

  double newToleranceForRightSide = max_s / cubesRightSide;
  if (newToleranceForRightSide < minTolerance) {
    cubesRightSide = static_cast<size_t>(max_s / minTolerance);
  } else {
    minTolerance = newToleranceForRightSide;
  }
  MoveCubeAtMidPointInSRightSide(_lane, 0., max_s, cubesRightSide);

  size_t cubesToHide = lastCubesUsed != 0 ? lastCubesUsed : cubes.size();
  for (size_t cubesUnused = cubesUsed; cubesUnused < cubesToHide; ++cubesUnused) {
    cubes[cubesUnused]->SetVisible(false);
  }
  lastCubesUsed = cubesUsed;
  minTolerance = oldTolerance;
}

void Outliner::SetVisibility(bool _visible) {
  for (size_t i = 0; i < lastCubesUsed; ++i) {
    cubes[i]->SetVisible(_visible);
  }
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

void Outliner::MoveCubeAtMidPointInR(const maliput::api::GeoPosition& _minRGeoPos,
                                     const maliput::api::GeoPosition& _maxRGeoPos, size_t& _maxAmountOfCubesToUse) {
  maliput::api::GeoPosition midPoint = (_maxRGeoPos + _minRGeoPos);
  midPoint.set_x(midPoint.x() / 2.0);
  midPoint.set_y(midPoint.y() / 2.0);
  midPoint.set_z(midPoint.z() / 2.0);
  if ((_maxRGeoPos - midPoint).length() > minTolerance && cubesUsed < cubes.size() && _maxAmountOfCubesToUse != 0) {
    cubes[cubesUsed]->SetWorldPosition(midPoint.x(), midPoint.y(), midPoint.z());
    cubes[cubesUsed]->SetVisible(true);
    ++cubesUsed;
    --_maxAmountOfCubesToUse;
    MoveCubeAtMidPointInR(_minRGeoPos, midPoint, _maxAmountOfCubesToUse);
    MoveCubeAtMidPointInR(midPoint, _maxRGeoPos, _maxAmountOfCubesToUse);
  }
}

void Outliner::MoveCubeAtMidPointInSLeftSide(const maliput::api::Lane* _lane, double min_s, double max_s,
                                             size_t& _maxAmountOfCubesToUse) {
  const double mid_s = (max_s + min_s) / 2.0;
  maliput::api::RBounds midRBounds = _lane->lane_bounds(mid_s);

  maliput::api::GeoPosition leftMidPoint =
      _lane->ToGeoPosition(maliput::api::LanePosition(mid_s, midRBounds.min(), 0.));
  maliput::api::GeoPosition extremePoint =
      _lane->ToGeoPosition(maliput::api::LanePosition(max_s, midRBounds.min(), 0.));

  if ((leftMidPoint - extremePoint).length() > minTolerance && cubesUsed < cubes.size() &&
      _maxAmountOfCubesToUse != 0) {
    ignition::math::Vector3d leftMidPointMathVector(leftMidPoint.x(), leftMidPoint.y(), leftMidPoint.z());
    ignition::math::Vector3d extremeMidPointMathVector(extremePoint.x(), extremePoint.y(), extremePoint.z());
    cubes[cubesUsed]->SetWorldPosition(leftMidPoint.x(), leftMidPoint.y(), leftMidPoint.z());
    cubes[cubesUsed]->SetWorldRotation(
        ignition::math::Matrix4d::LookAt(leftMidPointMathVector, extremeMidPointMathVector).Pose().Rot());
    cubes[cubesUsed]->SetVisible(true);
    ++cubesUsed;
    --_maxAmountOfCubesToUse;
    MoveCubeAtMidPointInSLeftSide(_lane, mid_s, max_s, _maxAmountOfCubesToUse);
    MoveCubeAtMidPointInSLeftSide(_lane, min_s, mid_s, _maxAmountOfCubesToUse);
  }
}

void Outliner::MoveCubeAtMidPointInSRightSide(const maliput::api::Lane* _lane, double min_s, double max_s,
                                              size_t& _maxAmountOfCubesToUse) {
  const double mid_s = (max_s + min_s) / 2.0;
  maliput::api::RBounds midRBounds = _lane->lane_bounds(mid_s);

  maliput::api::GeoPosition rightMidPoint =
      _lane->ToGeoPosition(maliput::api::LanePosition(mid_s, midRBounds.max(), 0.));
  maliput::api::GeoPosition extremePoint =
      _lane->ToGeoPosition(maliput::api::LanePosition(max_s, midRBounds.max(), 0.));

  if ((rightMidPoint - extremePoint).length() > minTolerance && cubesUsed < cubes.size() &&
      _maxAmountOfCubesToUse != 0) {
    ignition::math::Vector3d rightMidPointMathVector(rightMidPoint.x(), rightMidPoint.y(), rightMidPoint.z());
    ignition::math::Vector3d extremeMidPointMathVector(extremePoint.x(), extremePoint.y(), extremePoint.z());
    cubes[cubesUsed]->SetWorldPosition(rightMidPoint.x(), rightMidPoint.y(), rightMidPoint.z());
    cubes[cubesUsed]->SetWorldRotation(
        ignition::math::Matrix4d::LookAt(rightMidPointMathVector, extremeMidPointMathVector).Pose().Rot());
    cubes[cubesUsed]->SetVisible(true);
    ++cubesUsed;
    --_maxAmountOfCubesToUse;
    MoveCubeAtMidPointInSRightSide(_lane, mid_s, max_s, _maxAmountOfCubesToUse);
    MoveCubeAtMidPointInSRightSide(_lane, min_s, mid_s, _maxAmountOfCubesToUse);
  }
}

}  // namespace gui
}  // namespace delphyne
