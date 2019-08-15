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

  QObject::connect(this->maliputFileSelectionWidget, SIGNAL(maliputFileChanged(const std::string&)), this,
                   SLOT(OnNewMultilaneFile(const std::string&)));
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnLayerMeshChanged(const std::string& key, bool newValue) {
  // Updates the model.
  this->model->SetLayerState(key, newValue);
  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnTextLabelChanged(const std::string& key, bool newValue) {
  // Updates the model.
  this->model->SetTextLabelState(FromString(key), newValue);
  // Replicates into the GUI.
  this->renderWidget->RenderLabels(this->model->Labels());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnNewMultilaneFile(const std::string& filePath) {
  if (filePath.empty()) {
    return;
  }

  // Clears the GUI meshes and then populates with the meshes.
  this->renderWidget->Clear();

  // Loads the new file.
  this->model = std::make_unique<MaliputViewerModel>();
  this->model->Load(filePath);
  this->rulesVisualizerWiget->ClearText();
  this->rulesVisualizerWiget->ClearLaneList();
  const auto lane_ids = this->model->GetAllLaneIds<std::vector, QString>();
  for (size_t i = 0; i < lane_ids.size(); ++i)
  {
    this->rulesVisualizerWiget->AddLaneId(lane_ids[i]);
  }

  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
  this->renderWidget->RenderLabels(this->model->Labels());

  this->VisualizeFileName(filePath);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::VisualizeFileName(const std::string& filePath) {
  const auto filePathDividerPosition = filePath.rfind("/");
  if (filePathDividerPosition != std::string::npos) {
    this->maliputFileSelectionWidget->SetFileNameLabel(filePath.substr(filePathDividerPosition + 1));
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnVisualClicked(ignition::rendering::RayQueryResult rayResult) {
  if (this->model)
  {
    const maliput::api::Lane* lane = this->model->GetLaneFromWorldPosition(rayResult.point);
    if (lane)
    {
      const std::string& lane_id = lane->id().string();
      ignmsg << "Clicked lane ID: " << lane_id << "\n";
      OnRulesForLaneRequested(QString(lane_id.c_str()));
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
  this->rulesVisualizerWiget = new RulesVisualizerWidget(this);

  QObject::connect(this->renderWidget, SIGNAL(VisualClicked(ignition::rendering::RayQueryResult)), this,
                   SLOT(OnVisualClicked(ignition::rendering::RayQueryResult)));

  QObject::connect(
    this->rulesVisualizerWiget,
    SIGNAL(RequestRulesForLaneId(QString)),
    this,
    SLOT(OnRulesForLaneRequested(QString))
    );

  auto verticalLayout = new QVBoxLayout(this);
  verticalLayout->addWidget(this->maliputFileSelectionWidget);
  verticalLayout->addWidget(this->layerSelectionWidget);
  verticalLayout->addWidget(this->labelSelectionWidget);
  verticalLayout->addWidget(this->rulesVisualizerWiget);
  auto controlGroup = new QGroupBox("Control panel", this);
  controlGroup->setLayout(verticalLayout);
  controlGroup->setSizePolicy(QSizePolicy::Policy::Fixed,
    QSizePolicy::Policy::Fixed);

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

void MaliputViewerWidget::OnRulesForLaneRequested(QString laneId) {
  emit this->rulesVisualizerWiget->ReceiveRules(laneId,
    this->model->GetRulesOfLane<QString>(laneId.toStdString()));
}

/////////////////////////////////////////////////
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::MaliputViewerWidget, ignition::gui::Plugin)
