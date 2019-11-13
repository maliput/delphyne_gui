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

  QObject::connect(this->layerSelectionWidget, SIGNAL(valueChanged(const std::string&, bool)), this,
                   SLOT(OnLayerMeshChanged(const std::string&, bool)));

  QObject::connect(this->labelSelectionWidget, SIGNAL(valueChanged(const std::string&, bool)), this,
                   SLOT(OnTextLabelChanged(const std::string&, bool)));

  QObject::connect(
      this->maliputFileSelectionWidget,
      SIGNAL(maliputFileChanged(const std::string&, const std::string&, const std::string&, const std::string&)), this,
      SLOT(OnNewRoadNetwork(const std::string&, const std::string&, const std::string&, const std::string&)));
}

/////////////////////////////////////////////////
void MaliputViewerWidget::UpdateMeshDefaults(const std::string& key, bool newValue) {
  // Store the value the mesh should return to if not selected
  if (FoundKeyword(key, "marker")) {
    this->meshDefaults[kMarker] = newValue;
  } else if (FoundKeyword(key, "lane_text")) {
    this->meshDefaults[kLaneLabels] = newValue;
  } else if (FoundKeyword(key, "branch_point_text")) {
    this->meshDefaults[kBranchPointLabels] = newValue;
  } else if (FoundKeyword(key, "lane")) {
    this->meshDefaults[kLane] = newValue;
  } else if (FoundKeyword(key, "branch_point")) {
    this->meshDefaults[kBranchPoint] = newValue;
  }
}

/////////////////////////////////////////////////
bool MaliputViewerWidget::FoundKeyword(const std::string& key, const std::string& keyword) {
  return key.find(keyword) != std::string::npos;
}

/////////////////////////////////////////////////
std::string MaliputViewerWidget::GetID(const std::string& keyword) {
  const std::size_t firstNum = keyword.find_first_of("0123456789");
  std::string id = keyword.substr(firstNum, keyword.length() - firstNum + 1);
  return id;
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnLayerMeshChanged(const std::string& key, bool newValue) {
  // If the keyword "all" is found, enable all of the parsed type
  const std::size_t all_keyword = key.find(kAll);
  if (FoundKeyword(key, kAll)) {
    const std::string keyword = key.substr(0, all_keyword);
    this->UpdateMeshDefaults(key, newValue);
    for (auto const& it : this->model->Meshes()) {
      if (FoundKeyword(it.first, keyword)) {
        const std::string id = GetID(it.first);
        // If the region is not selected, update with the default setting
        if (!this->renderWidget->IsSelected(id)) {
          // Updates the model.
          this->model->SetLayerState(it.first, newValue);
        }
      }
    }
  } else {
    // Updates the model.
    this->model->SetLayerState(key, newValue);
    // If the asphalt is turned off, deselect all lanes
    if (FoundKeyword(key, kAsphalt) && !newValue) {
      this->renderWidget->DeselectAll();
      this->renderWidget->SetArrowVisibility(false);
    }
  }

  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnTextLabelChanged(const std::string& key, bool newValue) {
  // Updates the model.
  if (key == "lane_text_label" || key == "branch_point_text_label") {
    const int keyword_index = key.find("text");
    const std::string keyword = key.substr(0, keyword_index);
    this->UpdateMeshDefaults(key, newValue);
    for (auto const& it : this->model->Labels()) {
      if (FoundKeyword(it.first, keyword)) {
        // Updates the model.
        if (!this->renderWidget->IsSelected(it.second.text)) {
          this->model->SetTextLabelState(it.first, newValue);
        }
      }
    }
  } else {
    this->model->SetTextLabelState(key, newValue);
  }

  // Replicates into the GUI.
  this->renderWidget->RenderLabels(this->model->Labels());
}

/////////////////////////////////////////////////
std::string MaliputViewerWidget::MarkerMeshMapKey(const std::string& id) { return kMarker + "_" + id; }

/////////////////////////////////////////////////
std::string MaliputViewerWidget::BranchPointMeshMapKey(const std::string& id) { return kBranchPoint + "_" + id; }

/////////////////////////////////////////////////
std::string MaliputViewerWidget::LaneMeshMapKey(const std::string& id) { return kLane + "_" + id; }

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

void MaliputViewerWidget::UpdateSelectedWithDefault() {
  const std::vector<std::string> selectedLanes = this->renderWidget->GetSelectedLanes();
  const std::vector<std::string> selectedBranchPoints = this->renderWidget->GetSelectedBranchPoints();
  for (const auto& id : selectedLanes) {
    this->model->SetLayerState(LaneMeshMapKey(id), meshDefaults[kLane]);
    this->model->SetLayerState(MarkerMeshMapKey(id), meshDefaults[kMarker]);
    this->model->SetTextLabelState(LaneMeshMapKey(id), meshDefaults[kLaneLabels]);
  }
  for (const auto& id : selectedBranchPoints) {
    this->model->SetLayerState(BranchPointMeshMapKey(id), meshDefaults[kBranchPoint]);
    this->model->SetTextLabelState(BranchPointMeshMapKey(id), meshDefaults[kBranchPointLabels]);
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::VisualizeFileName(const std::string& filePath) {
  const auto filePathDividerPosition = filePath.rfind("/");
  if (filePathDividerPosition != std::string::npos) {
    this->maliputFileSelectionWidget->SetFileNameLabel(filePath.substr(filePathDividerPosition + 1));
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnSetAllSelectedRegionsToDefault() {
  UpdateSelectedWithDefault();

  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
  this->renderWidget->RenderLabels(this->model->Labels());
}

void MaliputViewerWidget::UpdateLane(const std::string& id) {
  const bool isLaneVisualized = this->renderWidget->IsSelected(id) || meshDefaults[kLane];
  const bool isMarkerVisualized = this->renderWidget->IsSelected(id) || meshDefaults[kMarker];
  const bool isLaneLabelVisualized = this->renderWidget->IsSelected(id) || meshDefaults[kLaneLabels];

  const std::string laneKey = LaneMeshMapKey(id);
  const std::string markerKey = MarkerMeshMapKey(id);

  this->OnLayerMeshChanged(laneKey, isLaneVisualized);
  this->OnLayerMeshChanged(markerKey, isMarkerVisualized);
  this->OnTextLabelChanged(laneKey, isLaneLabelVisualized);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::UpdateBranchPoint(const std::string& id) {
  const bool isBPMeshVisualized = this->renderWidget->IsSelected(id) || meshDefaults[kBranchPoint];
  const bool isBPLabelVisualized = this->renderWidget->IsSelected(id) || meshDefaults[kBranchPointLabels];
  const std::string key = BranchPointMeshMapKey(id);

  this->OnLayerMeshChanged(key, isBPMeshVisualized);
  this->OnTextLabelChanged(key, isBPLabelVisualized);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnVisualClicked(ignition::rendering::RayQueryResult rayResult) {
  if (this->model) {
    const maliput::api::Lane* lane = this->model->GetLaneFromWorldPosition(rayResult.point);
    if (lane) {
      const std::string& lane_id = lane->id().string();
      const std::string start_bp_id = lane->GetBranchPoint(maliput::api::LaneEnd::kStart)->id().string();
      const std::string end_bp_id = lane->GetBranchPoint(maliput::api::LaneEnd::kFinish)->id().string();

      ignmsg << "Clicked lane ID: " << lane_id << "\n";
      this->renderWidget->SelectLane(lane);

      // Update visualization to default if it is deselected
      UpdateLane(lane_id);
      UpdateBranchPoint(start_bp_id);
      UpdateBranchPoint(end_bp_id);

      PhaseRingPhaseIds phaseRingIdAndPhaseIdSelected = this->rulesVisualizerWidget->GetSelectedPhaseRingAndPhaseId();
      emit this->rulesVisualizerWidget->ReceiveRules(
          QString(lane_id.c_str()),
          this->model->GetRulesOfLane<QString>(phaseRingIdAndPhaseIdSelected.phase_ring_id.toStdString(),
                                               phaseRingIdAndPhaseIdSelected.phase_id.toStdString(), lane_id));
      this->renderWidget->PutArrowAt(rayResult.distance, rayResult.point);
      this->renderWidget->SetArrowVisibility(true);
    } else {
      UpdateSelectedWithDefault();
      this->renderWidget->SetArrowVisibility(false);
      this->renderWidget->DeselectAll();
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

  QObject::connect(this->renderWidget, SIGNAL(SetAllSelectedRegionsToDefault()), this,
                   SLOT(OnSetAllSelectedRegionsToDefault()));

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
