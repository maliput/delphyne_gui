// Copyright 2017 Toyota Research Institute

#include "MaliputViewerWidget.hh"

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
}

/////////////////////////////////////////////////
void MaliputViewerWidget::OnLayerMeshChanged(const std::string& key,
  bool newValue) {
  // Updates the model.
  this->model->SetLayerState(key, MaliputMesh::BooleanToVisualState(newValue));
  // Replicates into the GUI.
  this->renderWidget->RenderRoadMeshes(this->model->Meshes());
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
  this->renderWidget = new RenderMaliputWidget(this);
  auto layout = new QHBoxLayout(this);
  layout->addWidget(this->renderWidget);
  layout->addWidget(this->layerSelectionWidget);
  this->setLayout(layout);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::paintEvent(QPaintEvent* _e) {
  QWidget::paintEvent(_e);

  // TODO(agalbachicar): Properly sync the first paint event, so road meshes can
  //                     be queried from the model and set to the view.
  static bool first_run{false};
  if (!first_run) {
    first_run = true;
    this->renderWidget->RenderRoadMeshes(this->model->Meshes());
  }

  this->renderWidget->paintEvent(_e);
}

/////////////////////////////////////////////////
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::MaliputViewerWidget,
                                  ignition::gui::Plugin)
