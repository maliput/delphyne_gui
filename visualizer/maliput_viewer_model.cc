// Copyright 2018 Toyota Research Institute

#include "maliput_viewer_model.hh"

#include <iostream>
#include <string>

#include <ignition/common/Console.hh>
#include <drake/automotive/maliput/multilane/loader.h>
#include <delphyne/maliput/road_builder.h>

#include "maliput_mesh_builder.hh"

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
bool MaliputViewerModel::Load(const std::string& _maliputFilePath) {
  this->Clear();

  ignmsg << "About to load [" << _maliputFilePath << "] maliput file."
         << std::endl;
  LoadRoadGeometry(_maliputFilePath);
  ignmsg << "Loaded [" << _maliputFilePath << "] maliput file." << std::endl;
  ignmsg << "Loading RoadGeometry meshes of "
         << this->roadGeometry->id().string() << std::endl;
  std::map<std::string, drake::maliput::mesh::GeoMesh> geoMeshes =
      drake::maliput::mesh::BuildMeshes(this->roadGeometry.get(),
                                        drake::maliput::mesh::Features());
  ignmsg << "Meshes loaded." << std::endl;
  this->ConvertMeshes(geoMeshes);
  ignmsg << "Meshes converted to ignition type." << std::endl;
  this->GenerateLabels();
  ignmsg << "Labels generated." << std::endl;
  return true;
}


///////////////////////////////////////////////////////
void MaliputViewerModel::Clear() {
  this->roadGeometry.reset();
  this->labels.clear();
  this->maliputMeshes.clear();
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
    if (line.find("<OpenDRIVE>") != std::string::npos) {
      this->roadGeometry = delphyne::maliput::CreateMalidriveFromFile(
        _maliputFilePath.substr(_maliputFilePath.find_last_of("/") + 1),
        _maliputFilePath);
      return;
    }
    else if (line.find("maliput_multilane_builder:") != std::string::npos) {
      this->roadGeometry = drake::maliput::multilane::LoadFile(
          drake::maliput::multilane::BuilderFactory(), _maliputFilePath);
      return;
    }
  }
  throw std::runtime_error(_maliputFilePath +
                           " doesn't have any of the multilane keys");
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
      ignmsg << "Enabling mesh [" << it.first << "].\n";
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
void MaliputViewerModel::SetLayerState(
    const std::string& _key, bool _isVisible) {
  if (this->maliputMeshes.find(_key) == this->maliputMeshes.end()) {
    igndbg << _key + " doest not exist in maliputMeshes." << std::endl;
    return;
  }
  this->maliputMeshes[_key]->visible = _isVisible;
}

///////////////////////////////////////////////////////
void MaliputViewerModel::SetTextLabelState(
    MaliputLabelType _type, bool _isVisible) {
  switch (_type) {
    case MaliputLabelType::kLane: {
        std::vector<MaliputLabel>& lane_labels =
            labels[MaliputLabelType::kLane];
        for (MaliputLabel& label : lane_labels) {
          label.visible = _isVisible;
        }
      }
      break;
    case MaliputLabelType::kBranchPoint: {
        std::vector<MaliputLabel>& branchpoint_labels =
            labels[MaliputLabelType::kBranchPoint];
        for (MaliputLabel& label : branchpoint_labels) {
          label.visible = _isVisible;
        }
      }
      break;
    default:
      throw std::runtime_error(
          std::string("_type is not a valid MaliputLabelType."));
      break;
  }
}

///////////////////////////////////////////////////////
MaliputLabelType delphyne::gui::FromString(const std::string& _type) {
  if (_type == "lane_text_label") {
    return MaliputLabelType::kLane;
  } else if (_type == "branchpoint_text_label") {
    return MaliputLabelType::kBranchPoint;
  }
  throw std::runtime_error(
      std::string("_type = [") + _type +
      std::string(" ] is not \"lane_text_label\" nor ") +
      std::string("\"branchpoint_text_label\"."));
}
