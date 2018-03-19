// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_LAYERSELECTIONWIDGET_HH
#define DELPHYNE_GUI_LAYERSELECTIONWIDGET_HH

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>

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
