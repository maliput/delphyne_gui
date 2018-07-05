// Copyright 2018 Toyota Research Institute

#include "maliput_viewer_model.hh"

#include <iostream>

#include <ignition/common/Console.hh>

#include <drake/automotive/maliput/monolane/loader.h>
#include <drake/automotive/maliput/multilane/loader.h>

#include "global_attributes.hh"
#include "maliput_mesh_builder.hh"

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
bool MaliputViewerModel::Load() {
  if (GlobalAttributes::HasArgument("yaml_file")) {
    const std::string maliputFilePath =
      GlobalAttributes::GetArgument("yaml_file");
    igndbg << "About to load [" << maliputFilePath << "] maliput file."
           << std::endl;
    LoadRoadGeometry(maliputFilePath);
    igndbg << "Loaded [" << maliputFilePath << "] maliput file." << std::endl;
    igndbg << "Loading RoadGeometry meshes..." << std::endl;
    std::map<std::string, drake::maliput::mesh::GeoMesh> geoMeshes =
      drake::maliput::mesh::BuildMeshes(this->roadGeometry.get(),
        drake::maliput::mesh::Features());
    igndbg << "Meshes loaded." << std::endl;
    this->ConvertMeshes(geoMeshes);
    igndbg << "Meshes converted to ignition type." << std::endl;
    this->GenerateLabels();
    igndbg << "Labels generated." << std::endl;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////
const std::map<std::string, std::unique_ptr<MaliputMesh>>&
  MaliputViewerModel::Meshes() const {
  return this->maliputMeshes;
}

/////////////////////////////////////////////////
const std::map<MaliputLabelType, std::vector<MaliputLabel>>&
MaliputViewerModel::Labels() const {
  return this->labels;
}

/////////////////////////////////////////////////
void MaliputViewerModel::LoadRoadGeometry(const std::string& _maliputFilePath) {
  std::ifstream fileStream(_maliputFilePath);
  if (!fileStream.is_open()) {
    throw std::runtime_error(_maliputFilePath +
                             " doesn't exist or can't be opened.");
  }
  std::string line;
  while (!fileStream.eof()) {
    std::getline(fileStream, line);
    if (line.find("maliput_monolane_builder") != std::string::npos) {
      this->roadGeometry = drake::maliput::monolane::LoadFile(_maliputFilePath);
      return;
    } else if (line.find("maliput_multilane_builder") != std::string::npos) {
      this->roadGeometry = drake::maliput::multilane::LoadFile(
          drake::maliput::multilane::BuilderFactory(), _maliputFilePath);
      return;
    }
  }
  throw std::runtime_error(_maliputFilePath +
                           " doesn't have  any of the multilane nor"
                           " monolane keys");
}

/////////////////////////////////////////////////
void MaliputViewerModel::ConvertMeshes(
  const std::map<std::string, drake::maliput::mesh::GeoMesh>& _geoMeshes) {
  for (const auto& it : _geoMeshes) {
    auto maliputMesh = std::make_unique<MaliputMesh>();
    // Converts from drake to ignition mesh and sets the state.
    maliputMesh->mesh = drake::maliput::mesh::Convert(it.first, it.second);
    if (maliputMesh->mesh == nullptr) {
      ignmsg << "Skipping mesh [" << it.first << "] because it is empty.\n";
      maliputMesh->enabled = false;
      maliputMesh->visible = false;
    } else {
      maliputMesh->enabled = true;
      maliputMesh->visible = true;
    }
    // Retrieves the material
    maliputMesh->material = drake::maliput::mesh::GetMaterialByName(it.first);

    this->maliputMeshes[it.first] = std::move(maliputMesh);
  }
}

///////////////////////////////////////////////////////
namespace {

// \brief An offset to be applied over lane's height to avoid meshes
// intersection.
static const double kLaneHeightOffset{7.};

// \brief An offset to be applied over branch point's height to avoid meshes
// intersection.
static const double kBranchPointHeightOffset{3.};

// \brief Returns the world position of @p laneEnd.
// \param laneEnd The LaneEnd to get the position from.
// \return An ignition::math::Vector3d with the world position of
// @p laneEnd.lane at @p laneEnd.end extent.
ignition::math::Vector3d LaneEndWorldPosition(
    const drake::maliput::api::LaneEnd& laneEnd) {
  const double s_position =
      laneEnd.end == drake::maliput::api::LaneEnd::Which::kStart
          ? 0.
          : laneEnd.lane->length();
  const drake::maliput::api::GeoPosition position =
      laneEnd.lane->ToGeoPosition({s_position, 0., 0.});
  return {position.x(), position.y(), position.z() + kBranchPointHeightOffset};
}

// \brief Builds a MaliputLabel from the branch point @p bp.
// \details Sets label's position to one of the lane ends that @p bp has.
// Label's text is @p bp's ID.
// \param bp A BranchPoint to build a label from.
// \return A MaliputLabel with @p bp's information.
MaliputLabel LabelFor(const drake::maliput::api::BranchPoint& bp) {
  MaliputLabel label;
  label.text = bp.id().string();
  label.visible = true;
  if (bp.GetASide() && bp.GetASide()->size() != 0) {
    label.position = LaneEndWorldPosition(bp.GetASide()->get(0));
  } else if (bp.GetBSide() && bp.GetBSide()->size() != 0) {
    label.position = LaneEndWorldPosition(bp.GetBSide()->get(0));
  } else {
    ignerr << "Maliput's BranchPoint [" << bp.id().string()
           << "] has two empty sides." << std::endl;
  }
  return label;
}

// \brief Builds a MaliputLabel from the lane @p lane.
// \details Sets label's position to half the @p lane's length world position.
// Label's text is @p lane's ID.
// \param lane A Lane to build a label from.
// \return A MaliputLabel with @p lane's information.
MaliputLabel LabelFor(const drake::maliput::api::Lane& lane) {
  MaliputLabel label;
  label.text = lane.id().string();
  label.visible = true;
  const drake::maliput::api::GeoPosition position =
      lane.ToGeoPosition({lane.length() / 2., 0., 0.});
  label.position.Set(position.x(), position.y(),
                     position.z() + kLaneHeightOffset);
  return label;
}
}

///////////////////////////////////////////////////////
void MaliputViewerModel::GenerateLabels() {
  // Traverses branch points to generate labels for them.
  this->labels[MaliputLabelType::kBranchPoint] = std::vector<MaliputLabel>();
  for (int i = 0; i < roadGeometry->num_branch_points(); ++i) {
    const drake::maliput::api::BranchPoint* bp = roadGeometry->branch_point(i);
    this->labels[MaliputLabelType::kBranchPoint].push_back(LabelFor(*bp));
  }

  // Traverses lanes to generate labels for them.
  this->labels[MaliputLabelType::kLane] = std::vector<MaliputLabel>();
  for (int i = 0; i < this->roadGeometry->num_junctions(); ++i) {
    const drake::maliput::api::Junction* junction =
        this->roadGeometry->junction(i);
    for (int j = 0; j < junction->num_segments(); ++j) {
      const drake::maliput::api::Segment* segment = junction->segment(j);
      for (int k = 0; k < segment->num_lanes(); ++k) {
        this->labels[MaliputLabelType::kLane].push_back(
            LabelFor(*segment->lane(k)));
      }
    }
  }
}

///////////////////////////////////////////////////////
void MaliputViewerModel::SetLayerState(const std::string& _key, bool _isVisible) {
  if (this->maliputMeshes.find(_key) == this->maliputMeshes.end()) {
    throw std::runtime_error(_key + " doest not exist in maliputMeshes.");
  }
  this->maliputMeshes[_key]->visible = _isVisible;
}
