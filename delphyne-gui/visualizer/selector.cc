// Copyright 2019 Toyota Research Institute

#include "selector.hh"

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

Selector::Selector(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ, int _poolSize,
                   int _numLanes, double _minTolerance)
    : scene(_scene),
      numLanes(_numLanes),
      cubesPerLane(_poolSize),
      minTolerance(_minTolerance < _scaleX * 2.0 ? _scaleX : _minTolerance) {
  DELPHYNE_DEMAND(_poolSize > 3);
  DELPHYNE_DEMAND(_numLanes > 0);
  DELPHYNE_DEMAND(_scene != nullptr);
  DELPHYNE_DEMAND(_minTolerance >= 0);
  DELPHYNE_DEMAND(_scaleX > 0);
  DELPHYNE_DEMAND(_scaleY > 0);
  DELPHYNE_DEMAND(_scaleZ > 0);
  ignition::rendering::MaterialPtr material = _scene->CreateMaterial();
  material->SetDiffuse(255.0, 0.0, 0.0, 1.0);
  material->SetAmbient(255.0, 0.0, 0.0, 1.0);
  populationMap.resize(_numLanes);
  dimensionScale = ignition::math::Vector3d(_scaleX, _scaleY, _scaleZ);
  cubeMaterial = material;

  // Create and init space for _numLanes selected lanes, create more later if more lanes are selected
  this->CreateCubes(_scene, _scaleX, _scaleY, _scaleZ, cubeMaterial, _numLanes * _poolSize);
}

int Selector::GetLaneIndex(int _slot) { return cubesPerLane * _slot; }

int Selector::GetRemainingCubes() { return cubesPerLane - 4; }

int Selector::GetCubeIndex(int _laneIndex) { return 4 + _laneIndex; }

void Selector::SelectLane(const maliput::api::Lane* _lane) {
  DELPHYNE_DEMAND(_lane != nullptr);
  const std::string laneId = _lane->id().string();
  const std::string start_bp_id = _lane->GetBranchPoint(maliput::api::LaneEnd::kStart)->id().string();
  const std::string end_bp_id = _lane->GetBranchPoint(maliput::api::LaneEnd::kFinish)->id().string();

  // Remove markers from previously selected lane
  if (lanesSelected[laneId]) {
    const int slot = lanesSelected[laneId] - 1;
    const int laneIndex = slot * cubesPerLane;
    branchPointsSelected[start_bp_id]--;
    branchPointsSelected[end_bp_id]--;
    // Turn off visibility of a previously selected lane
    SetVisibilityOfCubesStartingFromTo(laneIndex, laneIndex + cubesPerLane, false);
    populationMap[slot] = false;
    lanesSelected[laneId] = 0;
    return;
  }

  // Increment value of branch point selection
  branchPointsSelected[start_bp_id]++;
  branchPointsSelected[end_bp_id]++;

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

  // Find first empty slot of unused markers
  int slot = FindFirstEmpty();

  // If all preinitialized markers are in use, create more
  if (slot == -1) {
    this->CreateCubes(scene, dimensionScale.X(), dimensionScale.Y(), dimensionScale.Z(), cubeMaterial,
                      numLanes * cubesPerLane);
    for (size_t i = 0; i < numLanes; ++i) {
      populationMap.push_back(false);
    }
    slot = FindFirstEmpty();
  }

  // Add one for easy distinction between non-existing keys
  lanesSelected[laneId] = slot + 1;

  const unsigned int laneIndex = GetLaneIndex(slot);
  cubes[laneIndex]->SetWorldPosition(initialRMinGeoPos.x(), initialRMinGeoPos.y(), initialRMinGeoPos.z());
  cubes[laneIndex]->SetVisible(true);
  cubes[laneIndex + 1]->SetWorldPosition(initialRMaxGeoPos.x(), initialRMaxGeoPos.y(), initialRMaxGeoPos.z());
  cubes[laneIndex + 1]->SetVisible(true);

  cubes[laneIndex + 2]->SetWorldPosition(endRMinGeoPos.x(), endRMinGeoPos.y(), endRMinGeoPos.z());
  cubes[laneIndex + 2]->SetVisible(true);
  cubes[laneIndex + 3]->SetWorldPosition(endRMaxGeoPos.x(), endRMaxGeoPos.y(), endRMaxGeoPos.z());
  cubes[laneIndex + 3]->SetVisible(true);

  int cubesUsed = GetCubeIndex(laneIndex);
  int remainingCubes = GetRemainingCubes();
  MoveCubeAtMidPointInR(initialRMinGeoPos, initialRMaxGeoPos, &cubesUsed, &remainingCubes);

  MoveCubeAtMidPointInR(endRMinGeoPos, endRMaxGeoPos, &cubesUsed, &remainingCubes);

  int cubesLeftSide = std::ceil(remainingCubes / 2);
  int cubesRightSide = remainingCubes - cubesLeftSide;
  double oldTolerance = minTolerance;

  // If we have less cubes to cover the lane, increase the tolerance.
  minTolerance = GetNewToleranceToPopulateLane(max_s, cubesLeftSide);
  cubesLeftSide = static_cast<int>(max_s / minTolerance);
  MoveCubeAtMidPointInS(_lane, 0., max_s, true, &cubesUsed, &cubesLeftSide);

  minTolerance = oldTolerance;
  minTolerance = GetNewToleranceToPopulateLane(max_s, cubesRightSide);
  cubesRightSide = static_cast<int>(max_s / minTolerance);
  MoveCubeAtMidPointInS(_lane, 0., max_s, false, &cubesUsed, &cubesRightSide);
  minTolerance = oldTolerance;
}

int Selector::FindFirstEmpty() {
  for (size_t i = 0; i < populationMap.size(); ++i) {
    if (!populationMap[i]) {
      populationMap[i] = true;
      return i;
    }
  }
  return -1;
}

void Selector::SetVisibility(bool _visible) { SetVisibilityOfCubesStartingFromTo(0, cubes.size(), _visible); }

void Selector::ResetPopulationMap() {
  for (size_t i = 0; i < populationMap.size(); ++i) {
    populationMap[i] = false;
  }
}

std::vector<std::string> Selector::GetSelectedBranchPoints() {
  std::vector<std::string> selectedBranchPoints;

  for (const auto& i : branchPointsSelected) {
    if (i.second) {
      selectedBranchPoints.push_back(i.first);
    }
  }

  return selectedBranchPoints;
}

std::vector<std::string> Selector::GetSelectedLanes() {
  std::vector<std::string> selectedLanes;

  for (const auto& i : lanesSelected) {
    if (i.second) {
      selectedLanes.push_back(i.first);
    }
  }

  return selectedLanes;
}

void Selector::ClearSelectedBranchPoints() { branchPointsSelected.clear(); }

void Selector::ClearSelectedLanes() { lanesSelected.clear(); }

void Selector::DeselectAll() {
  SetVisibility(false);
  ResetPopulationMap();
  ClearSelectedLanes();
  ClearSelectedBranchPoints();
}

bool Selector::IsSelected(const std::string& _id) { return lanesSelected[_id] || branchPointsSelected[_id]; }

bool Selector::IsSelected(const maliput::api::Lane* _lane) { return this->IsSelected(_lane->id().string()); }

void Selector::CreateCubes(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ,
                           ignition::rendering::MaterialPtr& _material, unsigned int _numCubes) {
  for (size_t i = 0; i < _numCubes; ++i) {
    ignition::rendering::VisualPtr cube = _scene->CreateVisual();
    cube->AddGeometry(_scene->CreateBox());
    cube->SetMaterial(_material, false);
    cube->SetVisible(false);
    cube->SetWorldScale(_scaleX, _scaleY, _scaleZ);
    cube->SetWorldPosition(0., 0., 0.);
    _scene->RootVisual()->AddChild(cube);
    cubes.push_back(cube);
  }
}

double Selector::GetNewToleranceToPopulateLane(double _laneLength, int _cubesUsedForSide) {
  const double newToleranceForLaneSide = _laneLength / _cubesUsedForSide;
  return newToleranceForLaneSide < minTolerance ? minTolerance : newToleranceForLaneSide;
}

void Selector::MoveCubeAtMidPointInR(const maliput::api::GeoPosition& _minRGeoPos,
                                     const maliput::api::GeoPosition& _maxRGeoPos, int* _cubesUsed,
                                     int* _maxAmountOfCubesToUse) {
  maliput::api::GeoPosition midPoint = maliput::api::GeoPosition::FromXyz((_maxRGeoPos.xyz() + _minRGeoPos.xyz()) / 2);
  if ((_maxRGeoPos - midPoint).length() > minTolerance && *_maxAmountOfCubesToUse != 0) {
    cubes[*_cubesUsed]->SetWorldPosition(midPoint.x(), midPoint.y(), midPoint.z());
    cubes[*_cubesUsed]->SetVisible(true);
    ++(*_cubesUsed);
    --(*_maxAmountOfCubesToUse);
    MoveCubeAtMidPointInR(_minRGeoPos, midPoint, _cubesUsed, _maxAmountOfCubesToUse);
    MoveCubeAtMidPointInR(midPoint, _maxRGeoPos, _cubesUsed, _maxAmountOfCubesToUse);
  }
}

void Selector::MoveCubeAtMidPointInS(const maliput::api::Lane* _lane, double min_s, double max_s, bool _left_side,
                                     int* _cubesUsed, int* _maxAmountOfCubesToUse) {
  const double mid_s = (max_s + min_s) / 2.0;
  const maliput::api::RBounds minRBounds = _lane->lane_bounds(min_s);
  const maliput::api::RBounds midRBounds = _lane->lane_bounds(mid_s);
  const maliput::api::RBounds maxRBounds = _lane->lane_bounds(max_s);
  const double r_min_bound = _left_side ? minRBounds.min() : minRBounds.max();
  const double r_mid_bound = _left_side ? midRBounds.min() : midRBounds.max();
  const double r_max_bound = _left_side ? maxRBounds.min() : maxRBounds.max();

  const maliput::api::GeoPosition minPoint = _lane->ToGeoPosition(maliput::api::LanePosition(min_s, r_min_bound, 0.));
  const maliput::api::GeoPosition midPoint = _lane->ToGeoPosition(maliput::api::LanePosition(mid_s, r_mid_bound, 0.));
  const maliput::api::GeoPosition maxPoint = _lane->ToGeoPosition(maliput::api::LanePosition(max_s, r_max_bound, 0.));

  if (!DoPointsViolateTolerance(midPoint, maxPoint) && !DoPointsViolateTolerance(midPoint, minPoint) &&
      *_maxAmountOfCubesToUse != 0) {
    ignition::math::Vector3d extremeMidPointMathVector(maxPoint.x(), maxPoint.y(), maxPoint.z());
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

void Selector::SetVisibilityOfCubesStartingFromTo(int _startFrom, int _to, bool _visible) {
  for (int i = _startFrom; i < _to; ++i) {
    cubes[i]->SetVisible(_visible);
  }
}

bool Selector::DoPointsViolateTolerance(const maliput::api::GeoPosition& _first_point,
                                        const maliput::api::GeoPosition& _second_point) {
  return (_first_point - _second_point).length() < minTolerance;
}

}  // namespace gui
}  // namespace delphyne
