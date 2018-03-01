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

#ifndef DELPHYNE_GUI_LAYERSELECTIONWIDGET_HH
#define DELPHYNE_GUI_LAYERSELECTIONWIDGET_HH

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>

#include "MaliputViewerModel.hh"

namespace delphyne {
namespace gui {

/// \brief Widget with checkboxes to enable / disable mesh visualization.
class LayerSelectionWidget : public QWidget {
  Q_OBJECT

 public:

  /// \brief Default constructor.
  explicit LayerSelectionWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~LayerSelectionWidget();

 public slots:
  void onAsphaltValueChanged(int state);
  void onLaneValueChanged(int state);
  void onMarkerValueChanged(int state);
  void onHBoundsValueChanged(int state);
  void onBranchPointValueChanged(int state);
  void onGrayedAsphaltValueChanged(int state);
  void onGrayedLaneValueChanged(int state);
  void onGrayedMarkerValueChanged(int state);

 signals:
  void valueChanged(const std::string& key, bool newValue);

 private:
  /// \brief Builds the GUI with all the check boxes for mesh toggling.
  void Build();

  QCheckBox* asphaltCheckBox{nullptr};
  QCheckBox* laneCheckBox{nullptr};
  QCheckBox* markerCheckBox{nullptr};
  QCheckBox* hboundCheckBox{nullptr};
  QCheckBox* branchPointCheckBox{nullptr};
  QCheckBox* grayedAsphaltCheckBox{nullptr};
  QCheckBox* grayedLaneCheckBox{nullptr};
  QCheckBox* grayedMarkerCheckBox{nullptr};
};

}
}
#endif