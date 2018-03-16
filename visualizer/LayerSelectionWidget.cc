// Copyright 2018 Open Source Robotics Foundation
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "LayerSelectionWidget.hh"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QVBoxLayout>

using namespace delphyne;
using namespace gui;

///////////////////////////////////////////////////////
LayerSelectionWidget::LayerSelectionWidget(QWidget* parent) :
    QWidget(parent) {
  // Build the widget.
  this->Build();
  // Connects all the check box events.
  QObject::connect(this->asphaltCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onAsphaltValueChanged(int)));
  QObject::connect(this->laneCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onLaneValueChanged(int)));
  QObject::connect(this->markerCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onMarkerValueChanged(int)));
  QObject::connect(this->hboundCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onHBoundsValueChanged(int)));
  QObject::connect(this->branchPointCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onBranchPointValueChanged(int)));
  QObject::connect(this->grayedAsphaltCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(onGrayedAsphaltValueChanged(int)));
  QObject::connect(this->grayedLaneCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onGrayedLaneValueChanged(int)));
  QObject::connect(this->grayedMarkerCheckBox, SIGNAL(stateChanged(int)), this,
    SLOT(onGrayedMarkerValueChanged(int)));
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

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(asphaltCheckBox);
  layout->addWidget(laneCheckBox);
  layout->addWidget(markerCheckBox);
  layout->addWidget(hboundCheckBox);
  layout->addWidget(branchPointCheckBox);
  layout->addWidget(grayedAsphaltCheckBox);
  layout->addWidget(grayedLaneCheckBox);
  layout->addWidget(grayedMarkerCheckBox);
  auto *groupBox = new QGroupBox("Layers", this);
  groupBox->setLayout(layout);

  auto *widgetLayout = new QVBoxLayout(this);
  widgetLayout->addWidget(groupBox);
  widgetLayout->addStretch();
  this->setLayout(widgetLayout);
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onAsphaltValueChanged(int state) {
  emit valueChanged("asphalt", this->asphaltCheckBox->isChecked());
}

///////////////////////////////////////////////////////
void LayerSelectionWidget::onLaneValueChanged(int state) {
  emit valueChanged("lane", this->laneCheckBox->isChecked());
}

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
