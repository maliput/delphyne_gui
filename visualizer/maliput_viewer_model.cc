// Copyright 2018 Toyota Research Institute

#include "MaliputViewerModel.hh"

#include <ignition/common/Console.hh>

#include <drake/automotive/maliput/monolane/loader.h>

#include "GlobalAttributes.hh"
#include "MaliputMeshBuilder.hh"

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
bool MaliputViewerModel::Load() {
  if (GlobalAttributes::HasArgument("yaml_file")) {
    const std::string maliputFilePath =
      GlobalAttributes::GetArgument("yaml_file");
    igndbg << "About to load ["
           << maliputFilePath <<
           "] monolane file." << std::endl;
    this->roadGeometry = drake::maliput::monolane::LoadFile(maliputFilePath);
    igndbg << "Loaded [" << maliputFilePath << "] monolane file." << std::endl;
    igndbg << "Loading RoadGeometry meshes..." << std::endl;
    std::map<std::string, drake::maliput::mesh::GeoMesh> geoMeshes =
      drake::maliput::mesh::BuildMeshes(this->roadGeometry.get(),
        drake::maliput::mesh::Features());
    igndbg << "Meshes loaded." << std::endl;
    this->ConvertMeshes(geoMeshes);
    igndbg << "Meshes converted to ignition type." << std::endl;
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
void MaliputViewerModel::SetLayerState(const std::string& _key, bool _isVisible) {
  if (this->maliputMeshes.find(_key) == this->maliputMeshes.end()) {
    throw std::runtime_error(_key + " doest not exist in maliputMeshes.");
  }
  this->maliputMeshes[_key]->visible = _isVisible;
}
