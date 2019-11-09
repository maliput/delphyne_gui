// Copyright 2017 Toyota Research Institute

#include "maliput_viewer_widget.hh"

#include <ignition/common/Console.hh>
#include <ignition/rendering/RayQuery.hh>
#include <maliput/api/lane.h>

#include "global_attributes.hh"
#include "rules_visualizer_widget.hh"

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
MaliputViewerWidget::MaliputViewerWidget(QWidget* parent) : Plugin() {
  // Loads the GUI.
  this->BuildGUI();
  // Loads the maliput file path if any and parses it.
  this->model = std::make_unique<MaliputViewerModel>();
  if (GlobalAttributes::HasArgument("xodr_file")) {
    this->model->Load(GlobalAttributes::GetArgument("xodr_file"));
    this->VisualizeFileName(GlobalAttributes::GetArgument("xodr_file"));
  } else if (GlobalAttributes::HasArgument("yaml_file")) {
    this->model->Load(GlobalAttributes::GetArgument("yaml_file"));
    this->VisualizeFileName(GlobalAttributes::GetArgument("yaml_file"));
  }
  this->meshDefaults["marker"] = true;
  this->meshDefaults["lane"] = true;
  this->meshDefaults["branchpoint"] = true;
  this->meshDefaults["branchpoint_labels"] = true;
  this->meshDefaults["lane_labels"] = true;

  QObject::connect(this->layerSelectionWidget, SIGNAL(valueChanged(const std::string&, bool)), this,
                   SLOT(OnLayerMeshChanged(const std::string&, bool)));

  QObject::connect(this->labelSelectionWidget, SIGNAL(valueChanged(const std::string&, bool)), this,
                   SLOT(OnTextLabelChanged(const std::string&, bool)));

  QObject::connect(
      this->maliputFileSelectionWidget,
      SIGNAL(maliputFileChanged(const std::string&, const std::string&, const std::string&, const std::string&)), this,
      SLOT(OnNewRoadNetwork(const std::string&, const std::string&, const std::string&, const std::string&)));
}

void MaliputViewerWidget::UpdateMeshDefaults(const std::string& key, bool newValue) {
  // Store the value the mesh should return to if not selected
  const std::size_t found_marker = key.find("marker");
  const std::size_t found_lane = key.find("lane");
  const std::size_t found_branchpoint = key.find("branch_point");
  const std::size_t found_lane_text = key.find("lane_text");
  const std::size_t found_branchpoint_text = key.find("branchpoint_text");

  if (found_marker != std::string::npos) {
    meshDefaults["marker"] = newValue;
  } else if (found_lane != std::string::npos) {
    meshDefaults["lane"] = newValue;
  } else if (found_branchpoint != std::string::npos) {
    meshDefaults["branchpoint"] = newValue;
  } else if (found_lane_text != std::string::npos) {
    meshDefaults["lane_labels"] = newValue;
  } else if (found_branchpoint_text != std::string::npos) {
    meshDefaults["branchpoint_labels"] = newValue;
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnLayerMeshChanged(const std::string& key, bool newValue) {
  // If the keyword "all" is found, enable all of the parsed type
  const std::size_t all_index = key.find("all");
  if (all_index != std::string::npos) {
    std::string keyword = key.substr(0, all_index);
    UpdateMeshDefaults(key, newValue);
    for (auto const& it : this->model->Meshes()) {
      if (it.first.find(keyword) != std::string::npos) {
        // Updates the model.
        const std::size_t firstNum = it.first.find_first_of("0123456789");
        std::string lane_id = it.first.substr(firstNum, it.first.length() - firstNum + 1);
        if (!this->renderWidget->IsSelected(lane_id)) {
          this->model->SetLayerState(it.first, newValue);
        }
      }
    }
  } else {
    // Updates the model.
    this->model->SetLayerState(key, newValue);
    // If the asphalt is turned off, deselect all lanes
    const std::size_t found_asphalt = key.find("asphalt");
    if (found_asphalt != std::string::npos && !newValue) {
      this->renderWidget->DeselectAllLanes();
    }
  }

  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnTextLabelChanged(const std::string& key, bool newValue) {
  // Updates the model.
  if (key == "lane_text_label" || key == "branchpoint_text_label") {
    UpdateMeshDefaults(key, newValue);
    this->model->SetTextLabelState(FromString(key), newValue);
  } else {
    this->model->SetTextLabelState(key, newValue);
  }

  // Replicates into the GUI.
  this->renderWidget->RenderLabels(this->model->Labels());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnNewRoadNetwork(const std::string& filePath, const std::string& roadRulebookFilePath,
                                           const std::string& trafficLightRulesFilePath,
                                           const std::string& phaseRingFilePath) {
  if (filePath.empty()) {
    return;
  }

  // Clears the GUI meshes and then populates with the meshes.
  this->renderWidget->Clear();

  // Loads the new file.
  this->model = std::make_unique<MaliputViewerModel>();
  this->model->Load(filePath, roadRulebookFilePath, trafficLightRulesFilePath, phaseRingFilePath);
  this->rulesVisualizerWidget->ClearText();
  this->rulesVisualizerWidget->ClearLaneList();
  this->rulesVisualizerWidget->ConstructPhaseRingTree(this->model->GetPhaseRings<QString>());
  const auto lane_ids = this->model->GetAllLaneIds<std::vector<QString>>();
  for (const QString& lane_id : lane_ids) {
    this->rulesVisualizerWidget->AddLaneId(lane_id);
  }

  this->renderWidget->RenderTrafficLights(this->model->GetTrafficLights());
  this->renderWidget->RenderArrow();
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
  this->renderWidget->RenderLabels(this->model->Labels());

  this->VisualizeFileName(filePath);
  this->maliputFileSelectionWidget->ClearLineEdits(true);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::VisualizeFileName(const std::string& filePath) {
  const auto filePathDividerPosition = filePath.rfind("/");
  if (filePathDividerPosition != std::string::npos) {
    this->maliputFileSelectionWidget->SetFileNameLabel(filePath.substr(filePathDividerPosition + 1));
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnSetAllLanesToDefault() {
  std::vector<std::string> selectedLanes = this->renderWidget->GetSelectedLanes();
  for (const auto& i : selectedLanes) {
    this->model->SetLayerState("lane_" + i, meshDefaults["lane"]);
    this->model->SetLayerState("marker_" + i, meshDefaults["marker"]);
    // TODO set branchpoint, lane and branchpoint labels here
    // this->model->SetLayerState("lane_" + i, meshDefaults["lane"]);
    // this->model->SetLayerState("marker_" + i, meshDefaults["marker"]);
    // this->meshDefaults["branchpoint"] = true;
    // this->meshDefaults["branchpoint_labels"] = true;
    // this->meshDefaults["lane_labels"] = true;
  }
  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnVisualClicked(ignition::rendering::RayQueryResult rayResult) {
  if (this->model) {
    const maliput::api::Lane* lane = this->model->GetLaneFromWorldPosition(rayResult.point);
    if (lane) {
      const std::string& lane_id = lane->id().string();
      ignmsg << "Clicked lane ID: " << lane_id << "\n";
      this->renderWidget->SelectLane(lane);
      const bool visualized = this->renderWidget->IsSelected(lane);
      // Set the mesh back to the check box default if the lane is not selected
      if (!visualized) {
        OnLayerMeshChanged("lane_" + lane_id, meshDefaults["lane"]);
        OnLayerMeshChanged("marker_" + lane_id, meshDefaults["marker"]);
      } else {
        OnLayerMeshChanged("lane_" + lane_id, visualized);
        OnLayerMeshChanged("marker_" + lane_id, visualized);
      }
      PhaseRingPhaseIds phaseRingIdAndPhaseIdSelected = this->rulesVisualizerWidget->GetSelectedPhaseRingAndPhaseId();
      emit this->rulesVisualizerWidget->ReceiveRules(
          QString(lane_id.c_str()),
          this->model->GetRulesOfLane<QString>(phaseRingIdAndPhaseIdSelected.phase_ring_id.toStdString(),
                                               phaseRingIdAndPhaseIdSelected.phase_id.toStdString(), lane_id));
      this->renderWidget->PutArrowAt(rayResult.distance, rayResult.point);
      this->renderWidget->SetArrowVisibility(true);
    } else {
      std::vector<std::string> selectedLanes = this->renderWidget->GetSelectedLanes();
      for (const auto& i : selectedLanes) {
        OnLayerMeshChanged("lane_" + i, meshDefaults["lane"]);
        OnLayerMeshChanged("marker_" + i, meshDefaults["marker"]);
      }
      this->renderWidget->SetArrowVisibility(false);
      this->renderWidget->DeselectAllLanes();
    }
  }
}

/////////////////////////////////////////////////
QPaintEngine* MaliputViewerWidget::paintEngine() const { return nullptr; }

/////////////////////////////////////////////////
void MaliputViewerWidget::BuildGUI() {
  this->setFocusPolicy(Qt::StrongFocus);
  this->setMouseTracking(true);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->title = "MaliputViewerWidget";

  this->setMinimumHeight(100);

  this->maliputFileSelectionWidget = new MaliputFileSelectionWidget(this);
  this->layerSelectionWidget = new LayerSelectionWidget(this);
  this->labelSelectionWidget = new LabelSelectionWidget(this);
  this->renderWidget = new RenderMaliputWidget(this);
  this->rulesVisualizerWidget = new RulesVisualizerWidget(this);

  QObject::connect(this->renderWidget, SIGNAL(VisualClicked(ignition::rendering::RayQueryResult)), this,
                   SLOT(OnVisualClicked(ignition::rendering::RayQueryResult)));

  QObject::connect(this->renderWidget, SIGNAL(SetAllLanesToDefault()), this, SLOT(OnSetAllLanesToDefault()));

  QObject::connect(this->rulesVisualizerWidget, SIGNAL(RequestRules()), this, SLOT(OnRulesForLaneRequested()));

  auto verticalLayout = new QVBoxLayout(this);
  verticalLayout->addWidget(this->maliputFileSelectionWidget);
  verticalLayout->addWidget(this->layerSelectionWidget);
  verticalLayout->addWidget(this->labelSelectionWidget);
  verticalLayout->addWidget(this->rulesVisualizerWidget);
  auto controlGroup = new QGroupBox("Control panel", this);
  controlGroup->setLayout(verticalLayout);
  controlGroup->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

  auto horizontalLayout = new QHBoxLayout(this);
  horizontalLayout->addWidget(this->renderWidget);
  horizontalLayout->addWidget(controlGroup);

  this->setLayout(horizontalLayout);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::paintEvent(QPaintEvent* _e) {
  QWidget::paintEvent(_e);

  // TODO(agalbachicar): Properly sync the first paint event, so road meshes can
  // be queried from the model and set to the view.
  if (!first_run_) {
    first_run_ = true;
    this->renderWidget->RenderRoadMeshes(this->model->Meshes());
    this->renderWidget->RenderLabels(this->model->Labels());
  }

  this->renderWidget->paintEvent(_e);
}

void MaliputViewerWidget::OnRulesForLaneRequested() {
  PhaseRingPhaseIds phaseRingPhaseIds = this->rulesVisualizerWidget->GetSelectedPhaseRingAndPhaseId();
  const std::string phase_ring_id = phaseRingPhaseIds.phase_ring_id.toStdString();
  const std::string phase_id = phaseRingPhaseIds.phase_id.toStdString();
  this->renderWidget->SetStateOfTrafficLights(this->model->GetBulbStates(phase_ring_id, phase_id));

  const std::string lane_id = this->rulesVisualizerWidget->GetSelectedLaneId().toStdString();
  if (!lane_id.empty()) {
    this->renderWidget->SelectLane(this->model->GetLaneFromId(lane_id));
    emit this->rulesVisualizerWidget->ReceiveRules(
        this->rulesVisualizerWidget->GetSelectedLaneId(),
        this->model->GetRulesOfLane<QString>(phase_ring_id, phase_id, lane_id));
  }
}

/////////////////////////////////////////////////
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::MaliputViewerWidget, ignition::gui::Plugin)
