// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH
#define DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH

#include <memory>
#include <string>

#include <maliput/api/road_geometry.h>

#include <ignition/gui/Plugin.hh>

#include "layer_selection_widget.hh"
#include "maliput_mesh.hh"
#include "maliput_viewer_model.hh"
#include "render_maliput_widget.hh"

namespace delphyne {
namespace gui {

/// \brief Plugin class that acts as a controller for the application.
///
/// Holds different view widgets and connects the model with the events and
/// changes in the GUI.
class MaliputViewerWidget : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  /// \brief Default constructor.
  explicit MaliputViewerWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~MaliputViewerWidget() = default;

 protected:
  /// \brief Override paintEngine to stop Qt From trying to draw on top of
  /// render window.
  /// \return NULL.
  virtual QPaintEngine* paintEngine() const;


 protected slots:
  /// \brief Updates the model based on the @p key mesh name and @p newValue and
  /// the mesh on the GUI.
  /// \param[in] key Name of the mesh.
  /// \param[in] newValue New mesh visualization status.
  void OnLayerMeshChanged(const std::string& key, bool newValue);

  /// \brief Updates the model based on the @p key text label group name and
  /// @p newValue and the label group on the GUI.
  /// \param[in] key Name of the label group.
  /// \param[in] newValue New label group visualization status.
  void OnTextLabelChanged(const std::string& key, bool newValue);

  /// \brief Clears the visualizer, loads the new set of meshes and text labels.
  /// \param filePath The path to the YAML file to load and visualize.
  void OnNewMultilaneFile(const std::string& filePath);

 protected:
  /// \brief Overridden method to receive Qt paint event.
  /// \param[in] _e The event that happened.
  virtual void paintEvent(QPaintEvent* _e);

 private:
  /// \brief Loads in the GUI the file name that @p filePath refers to.
  /// \param filePath The file path to the yaml file.
  void VisualizeFileName(const std::string& filePath);

  /// \brief Builds the widgets of the GUI.
  void BuildGUI();

  /// \brief Widget to hold and modify the visualization status of each layer.
  LayerSelectionWidget* layerSelectionWidget{nullptr};

  /// \brief Widget to hold and modify the visualization status of lane and
  /// branch point text labels.
  LabelSelectionWidget* labelSelectionWidget{nullptr};

  /// \brief Widget to load yaml files.
  MaliputFileSelectionWidget* maliputFileSelectionWidget{nullptr};

  /// \brief World render widget.
  RenderMaliputWidget* renderWidget{nullptr};

  /// \brief Model that holds the meshes and the visualization status.
  std::unique_ptr<MaliputViewerModel> model{};

  /// \brief Flag used to record the first paint event and sync on the road
  /// meshes when required.
  bool first_run_{false};

};

}
}

#endif
