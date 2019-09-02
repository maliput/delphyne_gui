// Copyright 2018 Toyota Research Institute

#include "layer_selection_widget.hh"
#include <ignition/common/Filesystem.hh>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

using namespace delphyne;
using namespace gui;

///////////////////////////////////////////////////////
LayerSelectionWidget::LayerSelectionWidget(QWidget* parent) : QWidget(parent) {
  // Build the widget.
  this->Build();
  // Connects all the check box events.
  QObject::connect(this->asphaltCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onAsphaltValueChanged(int)));
  QObject::connect(this->laneCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onLaneValueChanged(int)));
  QObject::connect(this->markerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onMarkerValueChanged(int)));
  QObject::connect(this->hboundCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onHBoundsValueChanged(int)));
  QObject::connect(this->branchPointCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onBranchPointValueChanged(int)));
  QObject::connect(this->grayedAsphaltCheckBox, SIGNAL(stateChanged(int)), this,
                   SLOT(onGrayedAsphaltValueChanged(int)));
  QObject::connect(this->grayedLaneCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onGrayedLaneValueChanged(int)));
  QObject::connect(this->grayedMarkerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onGrayedMarkerValueChanged(int)));
}

///////////////////////////////////////////////////////
LayerSelectionWidget::~LayerSelectionWidget() {}

///////////////////////////////////////////////////////
void LayerSelectionWidget::Build() {
  this->asphaltCheckBox = new QCheckBox("Asphalt", this);
  this->laneCheckBox = new QCheckBox("Lane", this);
  this->markerCheckBox = new QCheckBox("Marker", this);
  this->hboundCheckBox = new QCheckBox("HBounds", this);
  this->branchPointCheckBox = new QCheckBox("Branch point", this);
  this->grayedAsphaltCheckBox = new QCheckBox("Grayed asphalt", this);
  this->grayedLaneCheckBox = new QCheckBox("Grayed lane", this);
  this->grayedMarkerCheckBox = new QCheckBox("Grayed marker", this);

  // Sets all the check boxes as activated.
  this->asphaltCheckBox->setChecked(true);
  this->laneCheckBox->setChecked(true);
  this->markerCheckBox->setChecked(true);
  this->hboundCheckBox->setChecked(true);
  this->branchPointCheckBox->setChecked(true);
  // TODO(agalbachicar):  Once grayed layers are created, we can allow to toggle
  // them. In the meantime, we have just wired them and disabled.
  this->grayedAsphaltCheckBox->setCheckable(false);
  this->grayedLaneCheckBox->setCheckable(false);
  this->grayedMarkerCheckBox->setCheckable(false);

  auto* layout = new QVBoxLayout(this);
  layout->addWidget(asphaltCheckBox);
  layout->addWidget(laneCheckBox);
  layout->addWidget(markerCheckBox);
  layout->addWidget(hboundCheckBox);
  layout->addWidget(branchPointCheckBox);
  layout->addWidget(grayedAsphaltCheckBox);
  layout->addWidget(grayedLaneCheckBox);
  layout->addWidget(grayedMarkerCheckBox);
  auto* groupBox = new QGroupBox("Layers", this);
  groupBox->setLayout(layout);

  auto* widgetLayout = new QVBoxLayout(this);
  widgetLayout->addWidget(groupBox);
  widgetLayout->addStretch();
  this->setLayout(widgetLayout);
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onAsphaltValueChanged(int state) {
  emit valueChanged("asphalt", this->asphaltCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onLaneValueChanged(int state) { emit valueChanged("lane", this->laneCheckBox->isChecked()); }

///////////////////////////////////////////////////////
void LayerSelectionWidget::onMarkerValueChanged(int state) {
  emit valueChanged("marker", this->markerCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onHBoundsValueChanged(int state) {
  emit valueChanged("h_bounds", this->hboundCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onBranchPointValueChanged(int state) {
  emit valueChanged("branch_point", this->branchPointCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onGrayedAsphaltValueChanged(int state) {
  emit valueChanged("grayed_asphalt", this->grayedAsphaltCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onGrayedLaneValueChanged(int state) {
  emit valueChanged("grayed_lane", this->grayedLaneCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onGrayedMarkerValueChanged(int state) {
  emit valueChanged("grayed_marker", this->grayedMarkerCheckBox->isChecked());
}

///////////////////////////////////////////////////////
LabelSelectionWidget::LabelSelectionWidget(QWidget* parent) : QWidget(parent) {
  // Build the widget.
  this->Build();
  // Connects all the check box events.
  QObject::connect(this->laneCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onLaneValueChanged(int)));
  QObject::connect(this->branchPointCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onBranchPointValueChanged(int)));
}

///////////////////////////////////////////////////////
LabelSelectionWidget::~LabelSelectionWidget() {}

///////////////////////////////////////////////////////
void LabelSelectionWidget::onLaneValueChanged(int state) {
  emit valueChanged("lane_text_label", this->laneCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LabelSelectionWidget::onBranchPointValueChanged(int state) {
  emit valueChanged("branchpoint_text_label", this->branchPointCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LabelSelectionWidget::Build() {
  this->laneCheckBox = new QCheckBox("Lane", this);
  this->branchPointCheckBox = new QCheckBox("BranchPoint", this);

  // Sets all the check boxes as activated.
  this->laneCheckBox->setChecked(true);
  this->branchPointCheckBox->setChecked(true);

  auto* layout = new QVBoxLayout(this);
  layout->addWidget(laneCheckBox);
  layout->addWidget(branchPointCheckBox);

  auto* groupBox = new QGroupBox("Labels", this);
  groupBox->setLayout(layout);

  auto* widgetLayout = new QVBoxLayout(this);
  widgetLayout->addWidget(groupBox);
  widgetLayout->addStretch();
  this->setLayout(widgetLayout);
}

///////////////////////////////////////////////////////
MaliputFileSelectionWidget::MaliputFileSelectionWidget(QWidget* parent) : QWidget(parent) {
  if (!QDir::homePath().isEmpty()) {
    this->fileDialogOpenPath = QDir::toNativeSeparators(QDir::homePath()).toStdString();
  } else {
    this->fileDialogOpenPath = "/";
  }
  // Build the widget.
  this->Build();
  // Connects all the check box events.
  QObject::connect(this->loadButton, SIGNAL(released()), this, SLOT(onLoadButtonPressed()));
  QObject::connect(this->roadRulebookButton, SIGNAL(released()), this, SLOT(onRoadRulebookButtonPressed()));
  QObject::connect(this->trafficLightRulesButton, SIGNAL(released()), this, SLOT(onTrafficLightrulesButtonPressed()));
  QObject::connect(this->phaseRingButton, SIGNAL(released()), this, SLOT(onPhaseRingButtonPressed()));
}

///////////////////////////////////////////////////////
MaliputFileSelectionWidget::~MaliputFileSelectionWidget() {}

///////////////////////////////////////////////////////
void MaliputFileSelectionWidget::SetFileNameLabel(const std::string& fileName) {
  this->fileNameLabel->setText(QString::fromStdString(fileName));
}

void MaliputFileSelectionWidget::ClearLineEdits(bool keepOldTextsAsPlaceholders) {
  if (keepOldTextsAsPlaceholders) {
    QFileInfo ruleFile(this->roadRulebookLineEdit->text());
    this->roadRulebookLineEdit->setPlaceholderText(ruleFile.baseName());

    ruleFile.setFile(this->trafficLightRulesLineEdit->text());
    this->trafficLightRulesLineEdit->setPlaceholderText(ruleFile.baseName());

    ruleFile.setFile(this->phaseRingLineEdit->text());
    this->phaseRingLineEdit->setPlaceholderText(ruleFile.baseName());
  } else {
    this->roadRulebookLineEdit->setPlaceholderText(QString());
    this->trafficLightRulesLineEdit->setPlaceholderText(QString());
    this->phaseRingLineEdit->setPlaceholderText(QString());
  }
  this->roadRulebookLineEdit->clear();
  this->trafficLightRulesLineEdit->clear();
  this->phaseRingLineEdit->clear();
}

///////////////////////////////////////////////////////
void MaliputFileSelectionWidget::onLoadButtonPressed() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Maliput XODR or YAML"),
                                                  QString::fromStdString(this->fileDialogOpenPath),
                                                  tr("XODR Files (*.xodr);;YAML files (*.yaml)"));
  if (!fileName.isEmpty()) {
    this->fileDialogOpenPath = fileName.toStdString();
  }
  emit maliputFileChanged(fileName.toStdString(), this->roadRulebookLineEdit->text().toStdString(),
                          this->trafficLightRulesLineEdit->text().toStdString(),
                          this->phaseRingLineEdit->text().toStdString());
}

void MaliputFileSelectionWidget::onRoadRulebookButtonPressed() {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open Road Rulebook YAML"), QString::fromStdString(this->fileDialogOpenPath), tr("YAML files (*.yaml)"));
  this->roadRulebookLineEdit->setText(fileName);
}

void MaliputFileSelectionWidget::onTrafficLightrulesButtonPressed() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Traffic Lights Rulebook YAML"),
                                   QString::fromStdString(this->fileDialogOpenPath), tr("YAML files (*.yaml)"));
  this->trafficLightRulesLineEdit->setText(fileName);
}

void MaliputFileSelectionWidget::onPhaseRingButtonPressed() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Traffic Lights Rulebook YAML"),
                                   QString::fromStdString(this->fileDialogOpenPath), tr("YAML files (*.yaml)"));
  this->phaseRingLineEdit->setText(fileName);
}

///////////////////////////////////////////////////////
void MaliputFileSelectionWidget::Build() {
  this->loadButton = new QPushButton("Load", this);
  this->fileNameLabel = new QLabel("", this);
  this->roadRulebookLineEdit = new QLineEdit(this);
  this->trafficLightRulesLineEdit = new QLineEdit(this);
  this->phaseRingLineEdit = new QLineEdit(this);
  this->roadRulebookButton = new QPushButton("Select road rulebook", this);
  this->trafficLightRulesButton = new QPushButton("Select traffic lights book", this);
  this->phaseRingButton = new QPushButton("Select phase ring book", this);

  QWidget* roadRulebookWidgetContainer = new QWidget(this);
  QHBoxLayout* roadRulebookLayout = new QHBoxLayout(roadRulebookWidgetContainer);
  roadRulebookLayout->addWidget(roadRulebookLineEdit);
  roadRulebookLayout->addWidget(roadRulebookButton);

  QWidget* trafficLightRulebookWidgetContainer = new QWidget(this);
  QHBoxLayout* trafficLightRulesLayout = new QHBoxLayout(trafficLightRulebookWidgetContainer);
  trafficLightRulesLayout->addWidget(trafficLightRulesLineEdit);
  trafficLightRulesLayout->addWidget(trafficLightRulesButton);

  QWidget* phaseRingWidgetContainer = new QWidget(this);
  QHBoxLayout* phaseRingLayout = new QHBoxLayout(phaseRingWidgetContainer);
  phaseRingLayout->addWidget(phaseRingLineEdit);
  phaseRingLayout->addWidget(phaseRingButton);

  auto* layout = new QVBoxLayout(this);
  layout->addWidget(fileNameLabel);
  layout->addWidget(loadButton);
  layout->addWidget(roadRulebookWidgetContainer);
  layout->addWidget(trafficLightRulebookWidgetContainer);
  layout->addWidget(phaseRingWidgetContainer);

  this->setLayout(layout);
}
