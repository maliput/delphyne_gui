// Copyright 2017 Toyota Research Institute

#include "maliput_viewer_widget.hh"

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
MaliputViewerWidget::MaliputViewerWidget(QWidget* parent)
    : Plugin() {
  // Loads the GUI.
  this->BuildGUI();
  // Loads the maliput file path if any and parses it.
  this->model = std::make_unique<MaliputViewerModel>();
  this->model->Load();

  QObject::connect(this->layerSelectionWidget,
    SIGNAL(valueChanged(const std::string&, bool)), this,
    SLOT(OnLayerMeshChanged(const std::string&, bool)));

  QObject::connect(this->labelSelectionWidget,
    SIGNAL(valueChanged(const std::string&, bool)), this,
    SLOT(OnTextLabelChanged(const std::string&, bool)));
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnLayerMeshChanged(const std::string& key,
  bool newValue) {
  // Updates the model.
  this->model->SetLayerState(key, newValue);
  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnTextLabelChanged(
    const std::string& key, bool newValue) {
  // Updates the model.
  this->model->SetTextLabelState(FromString(key), newValue);
  // Replicates into the GUI.
  this->renderWidget->RenderLabels(this->model->Labels());
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

  this->layerSelectionWidget = new LayerSelectionWidget(this);
  this->labelSelectionWidget = new LabelSelectionWidget(this);
  this->renderWidget = new RenderMaliputWidget(this);

  auto verticalLayout = new QVBoxLayout(this);
  verticalLayout->addWidget(this->layerSelectionWidget);
  verticalLayout->addWidget(this->labelSelectionWidget);
  auto controlGroup = new QGroupBox("Control panel", this);
  controlGroup->setLayout(verticalLayout);

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

/////////////////////////////////////////////////
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::MaliputViewerWidget,
                                  ignition::gui::Plugin)
