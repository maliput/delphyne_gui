// Copyright 2021 Toyota Research Institute

#include "maliput_viewer_plugin.hh"

#include <ignition/common/Console.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Text.hh>
#include <ignition/rendering/Visual.hh>

#include "global_attributes.hh"

namespace delphyne {
namespace gui {
namespace {

// \returns True when @p keyword is found in @p word.
bool FoundKeyword(const std::string& word, const std::string& keyword) {
  return word.find(keyword) != std::string::npos;
}

// \brief Returns the absolute path from @p fileUrl.
// \details `fileUrl` is expected to be conformed as:
//          "file://" + "absolute path"
//          If `fileUrl` is empty returns an empty string.
std::string GetPathFromFileUrl(const std::string& fileUrl) {
  static constexpr char const* kFileUrlLabel = "file://";
  return fileUrl.empty() ? fileUrl : fileUrl.substr(fileUrl.find(kFileUrlLabel) + strlen(kFileUrlLabel));
}

}  // namespace

MaliputViewerPlugin::MaliputViewerPlugin() : Plugin() {
  model = std::make_unique<MaliputViewerModel>();

  // Loads the maliput file path if any and parses it.
  if (GlobalAttributes::HasArgument("xodr_file")) {
    if (GlobalAttributes::HasArgument("malidrive_backend")) {
      model->SetOpenDriveBackend(GlobalAttributes::GetArgument("malidrive_backend"));
    }
    model->Load(GlobalAttributes::GetArgument("xodr_file"));
  } else if (GlobalAttributes::HasArgument("yaml_file")) {
    model->Load(GlobalAttributes::GetArgument("yaml_file"));
  }
}

QList<bool> MaliputViewerPlugin::LayerCheckboxes() const {
  // Returns the checkboxes' state by default.
  return {true /* asphalt */,      true /* lane */,
          true /* marker */,       true /* h_bounds */,
          true /* branchpoint */,  false /* grayed_asphalt */,
          false /* grayed_lane */, false /* grayed_marker */};
}

QList<bool> MaliputViewerPlugin::LabelCheckboxes() const {
  // Returns the checkboxes' state by default.
  return {true /* lane */, true /* branch_point */};
}

void MaliputViewerPlugin::OnNewRoadNetwork(const QString& _mapFile, const QString& _roadRulebookFile,
                                           const QString& _trafficLightBookFile, const QString& _phaseRingBookFile) {
  if (_mapFile.isEmpty()) {
    ignerr << "Select the map file before clicking the LOAD button." << std::endl;
    return;
  }
  Clear();
  mapFile = GetPathFromFileUrl(_mapFile.toStdString());
  roadRulebookFile = GetPathFromFileUrl(_roadRulebookFile.toStdString());
  trafficLightBookFile = GetPathFromFileUrl(_trafficLightBookFile.toStdString());
  phaseRingBookFile = GetPathFromFileUrl(_phaseRingBookFile.toStdString());
  model->Load(mapFile, roadRulebookFile, trafficLightBookFile, phaseRingBookFile);
  emit LayerCheckboxesChanged();
  emit LabelCheckboxesChanged();
  RenderMeshes();
}

void MaliputViewerPlugin::OnNewMeshLayerSelection(const QString& _layer, bool _state) {
  static constexpr char const* kAll{"all"};
  const std::string layer{_layer.toStdString()};
  const std::size_t all_keyword = layer.find(kAll);
  // If the keyword "all" is found, enable all of the parsed type.
  if (FoundKeyword(layer, kAll)) {
    const std::string keyword = layer.substr(0, all_keyword);
    for (auto const& it : model->Meshes()) {
      if (FoundKeyword(it.first, keyword)) {
        model->SetLayerState(it.first, _state);
      }
    }
  } else {
    model->SetLayerState(layer, _state);
  }
  RenderRoadMeshes(model->Meshes());
}

void MaliputViewerPlugin::OnNewTextLabelSelection(const QString& _label, bool _state) {
  const std::string label{_label.toStdString()};
  for (auto const& it : model->Labels()) {
    if (FoundKeyword(it.first, label)) {
      model->SetTextLabelState(it.first, _state);
    }
  }
  // TODO(#): There shouldn't be a need to clear the generated labels
  //          each time the visualization of the labels changes.
  for (auto it : textLabels) {
    this->rootVisual->RemoveChild(it.second);
  }
  textLabels.clear();

  RenderLabels(model->Labels());
}

void MaliputViewerPlugin::timerEvent(QTimerEvent* _event) {
  if (_event->timerId() != timer.timerId()) {
    return;
  }

  // Get the render engine.
  // Note: we don't support other engines than Ogre.
  auto engine = ignition::rendering::engine(kEngineName);
  scene = engine->SceneByName(kSceneName);
  if (!scene) {
    ignwarn << "Scene \"" << kSceneName << "\" not found yet. Trying again in "
            << " Trying again in " << kTimerPeriodInMs << "ms" << std::endl;
    return;
  }
  timer.stop();
  ConfigurateScene();
  RenderMeshes();
}

void MaliputViewerPlugin::RenderMeshes() {
  ignmsg << "Rendering Road meshes..." << std::endl;
  RenderRoadMeshes(model->Meshes());
  ignmsg << "Rendering Label meshes..." << std::endl;
  RenderLabels(model->Labels());
}

void MaliputViewerPlugin::RenderRoadMeshes(const std::map<std::string, std::unique_ptr<MaliputMesh>>& _maliputMeshes) {
  for (const auto& id_mesh : _maliputMeshes) {
    ignmsg << "Rendering road mesh: " << id_mesh.first << std::endl;

    // Checks if the mesh to be rendered already exists or not.
    const auto meshExists = meshes.find(id_mesh.first);

    if (!id_mesh.second->enabled) {
      ignmsg << "Road mesh " << id_mesh.first << " is disabled." << std::endl;
      // If the mesh already exists, set visibility to false.
      if (meshExists != this->meshes.end()) {
        meshes[id_mesh.first]->SetVisible(false);
      }
      continue;
    }
    // If the mesh doesn't exist, it creates new one.
    if (meshExists == meshes.end()) {
      ignition::rendering::VisualPtr visual;
      // Creates a material for the visual.
      ignition::rendering::MaterialPtr material = scene->CreateMaterial();
      if (!material) {
        ignerr << "Failed to create material.\n";
        continue;
      }
      visual = scene->CreateVisual();
      if (!visual) {
        ignerr << "Failed to create visual.\n";
        continue;
      }
      // Adds the visual to the map for later reference.
      meshes[id_mesh.first] = visual;
      // Sets the pose of the mesh.
      visual->SetLocalPose(ignition::math::Pose3d(0, 0, 0, 1, 0, 0, 0));
      // Loads the mesh into the visual.
      if (id_mesh.second->mesh.get() == nullptr) {
        ignerr << id_mesh.first << "'s mesh pointer is nullptr" << std::endl;
        continue;
      }
      ignition::rendering::MeshDescriptor descriptor(id_mesh.second->mesh.get());
      descriptor.Load();
      ignition::rendering::MeshPtr meshGeom = scene->CreateMesh(descriptor);
      visual->AddGeometry(meshGeom);
      // Adds the mesh to the parent root visual.
      rootVisual->AddChild(visual);

      // Applies the correct material to the mesh.
      if (!FillMaterial(id_mesh.second->material.get(), material)) {
        ignerr << "Failed to fill " << id_mesh.first << " material information.\n";
        continue;
      }
      visual->SetMaterial(material);
    }
    meshes.at(id_mesh.first)->SetVisible(id_mesh.second->visible);
  }
}

void MaliputViewerPlugin::RenderLabels(const std::map<std::string, MaliputLabel>& _labels) {
  for (const auto& id_label : _labels) {
    ignmsg << "Rendering label mesh: " << id_label.first << std::endl;
    // Checks if the text labels to be rendered already exists or not.
    const auto labelExists = textLabels.find(id_label.first);
    if (!id_label.second.enabled) {
      ignmsg << "Label mesh " << id_label.first << " is disabled." << std::endl;
      // If the text label already exists, set visibility to false.
      if (labelExists != textLabels.end()) {
        textLabels[id_label.first]->SetVisible(false);
      }
      continue;
    }
    // If the text label doesn't exist, it creates new one.
    if (labelExists == textLabels.end()) {
      ignition::rendering::VisualPtr visual;
      // Creates a material for the visual.
      ignition::rendering::MaterialPtr material = scene->CreateMaterial();
      if (!material) {
        ignerr << "Failed to create material.\n";
        continue;
      }
      visual = scene->CreateVisual();
      if (!visual) {
        ignerr << "Failed to create visual.\n";
        continue;
      }
      // Adds the visual to the map for later reference.
      textLabels[id_label.first] = visual;
      visual->SetLocalPose(ignition::math::Pose3d(id_label.second.position, ignition::math::Quaterniond()));
      // Creates the text geometry.
      ignition::rendering::TextPtr textGeometry = scene->CreateText();
      textGeometry->SetFontName("Liberation Sans");
      textGeometry->SetTextString(id_label.second.text);
      textGeometry->SetShowOnTop(true);
      textGeometry->SetTextAlignment(ignition::rendering::TextHorizontalAlign::CENTER,
                                     ignition::rendering::TextVerticalAlign::CENTER);
      visual->AddGeometry(textGeometry);
      // Adds the mesh to the parent root visual.
      rootVisual->AddChild(visual);
      // Assigns a material for the visual.
      if (id_label.second.labelType == MaliputLabelType::kLane) {
        CreateLaneLabelMaterial(material);
      } else if (id_label.second.labelType == MaliputLabelType::kBranchPoint) {
        CreateBranchPointLabelMaterial(material);
      } else {
        ignerr << "Unsupported label type for: " << id_label.first << std::endl;
      }
      // Applies the correct material to the mesh.
      visual->SetMaterial(material);
    }
    textLabels.at(id_label.first)->SetVisible(id_label.second.visible);
  }
}

void MaliputViewerPlugin::Clear() {
  // Clears the text labels.
  for (auto it : textLabels) {
    this->rootVisual->RemoveChild(it.second);
  }
  // Clears the meshes.
  for (auto it : meshes) {
    this->rootVisual->RemoveChild(it.second);
  }
  textLabels.clear();
  meshes.clear();
}

void MaliputViewerPlugin::CreateLaneLabelMaterial(ignition::rendering::MaterialPtr& _material) {
  _material->SetDiffuse(0.8, 0.8, 0.0);
  _material->SetAmbient(1.0, 1.0, 0.0);
  _material->SetSpecular(1.0, 1.0, 0.5);
  _material->SetShininess(10.);
  _material->SetTransparency(0.5);
}

void MaliputViewerPlugin::CreateBranchPointLabelMaterial(ignition::rendering::MaterialPtr& _material) {
  _material->SetDiffuse(0.0, 0.7, 0.0);
  _material->SetAmbient(1.0, 1.0, 0.0);
  _material->SetSpecular(1.0, 1.0, 0.5);
  _material->SetShininess(10.);
  _material->SetTransparency(0.5);
}

bool MaliputViewerPlugin::FillMaterial(const maliput::utility::Material* _maliputMaterial,
                                       ignition::rendering::MaterialPtr& _ignitionMaterial) {
  if (!_maliputMaterial) {
    return false;
  }

  _ignitionMaterial->SetDiffuse(_maliputMaterial->diffuse.x(), _maliputMaterial->diffuse.y(),
                                _maliputMaterial->diffuse.z());
  _ignitionMaterial->SetAmbient(_maliputMaterial->ambient.x(), _maliputMaterial->ambient.y(),
                                _maliputMaterial->ambient.z());
  _ignitionMaterial->SetSpecular(_maliputMaterial->specular.x(), _maliputMaterial->specular.y(),
                                 _maliputMaterial->specular.z());
  _ignitionMaterial->SetShininess(_maliputMaterial->shinines);
  _ignitionMaterial->SetTransparency(_maliputMaterial->transparency);

  return true;
}

void MaliputViewerPlugin::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  title = "Maliput Viewer Plugin";

  if (!_pluginElem) {
    ignerr << "Error reading plugin XML element " << std::endl;
  }

  // Get the render engine.
  // Note: we don't support other engines than Ogre.
  auto engine = ignition::rendering::engine(kEngineName);
  if (!engine) {
    ignerr << "Engine \"" << kEngineName << "\" not supported, origin display plugin won't work." << std::endl;
    return;
  }
  // Get the scene.
  scene = engine->SceneByName(kSceneName);
  if (!scene) {
    ignwarn << "Scene \"" << kSceneName << "\" not found, meshes won't be loaded until the scene is created."
            << " Trying again in " << kTimerPeriodInMs << "ms" << std::endl;
    timer.start(kTimerPeriodInMs, this);
    return;
  }
  ConfigurateScene();
  RenderMeshes();
}

void MaliputViewerPlugin::ConfigurateScene() {
  rootVisual = scene->RootVisual();
  if (!rootVisual) {
    ignerr << "Failed to find the root visual" << std::endl;
    return;
  }
  // Lights.
  const double lightRed = 0.88;
  const double lightGreen = 0.88;
  const double lightBlue = 0.95;
  scene->SetAmbientLight(lightRed, lightGreen, lightBlue);
  auto directionalLight = scene->CreateDirectionalLight();
  if (!directionalLight) {
    ignerr << "Failed to create a directional light" << std::endl;
    return;
  }
  directionalLight->SetDirection(-0.5, -0.5, -1);
  directionalLight->SetDiffuseColor(lightRed, lightGreen, lightBlue);
  directionalLight->SetSpecularColor(lightRed, lightGreen, lightBlue);
  rootVisual->AddChild(directionalLight);
}

}  // namespace gui
}  // namespace delphyne

// Register this plugin
IGNITION_ADD_PLUGIN(delphyne::gui::MaliputViewerPlugin, ignition::gui::Plugin)
