// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_LAYERSELECTIONWIDGET_HH
#define DELPHYNE_GUI_LAYERSELECTIONWIDGET_HH

#include <string>

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

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

/// \brief Widget with checkboxes to enable / disable label visualization.
class LabelSelectionWidget : public QWidget {
  Q_OBJECT

 public:
  /// \brief Default constructor.
  explicit LabelSelectionWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~LabelSelectionWidget();

 public slots:
  void onLaneValueChanged(int state);
  void onBranchPointValueChanged(int state);

 signals:
  void valueChanged(const std::string& key, bool newValue);

 private:
  /// \brief Builds the GUI with all the check boxes for label toggling.
  void Build();

  QCheckBox* branchPointCheckBox{nullptr};
  QCheckBox* laneCheckBox{nullptr};
};

/// \brief Controls the button and the file dialog to load a yaml file.
class MaliputFileSelectionWidget : public QWidget {
  Q_OBJECT

 public:
  /// \brief Constructor.
  explicit MaliputFileSelectionWidget(QWidget* parent = 0);

  /// \brief Destructor.
  virtual ~MaliputFileSelectionWidget();

  /// Sets @p fileName into the label to display the loaded file.
  void SetFileNameLabel(const std::string& fileName);

 public slots:
  void onLoadButtonPressed();

 signals:
  void maliputFileChanged(const std::string& filePath);

 private:
  /// \brief Builds the GUI with a button to load a file dialog and a label to
  /// display the name of the loaded file.
  void Build();

  QPushButton* loadButton{nullptr};
  QLabel* fileNameLabel{nullptr};
  std::string fileDialogOpenPath{};
};

}
}
#endif
