// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH
#define DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH

#include <memory>
#include <string>

#include <maliput/api/road_geometry.h>

#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/RayQuery.hh>

#include "layer_selection_widget.hh"
#include "maliput_viewer_model.hh"
#include "render_maliput_widget.hh"
#include "rules_visualizer_widget.hh"

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
  void OnNewRoadNetwork(const std::string& filePath, const std::string& roadRulebookFilePath,
                        const std::string& trafficLightRulesFilePath, const std::string& phaseRingFilePath);

  /// \brief Prints the ID of the lane if any was selected.
  /// \param[in] rayResult Ray that contains the point where the click hit.
  void OnVisualClicked(ignition::rendering::RayQueryResult rayResult);

  /// \brief Sets all selected regions back to the default values indicated by the GUI checkboxes
  /// and updates the visualizer
  void OnSetAllSelectedRegionsToDefault();

  /// \brief Emits rulesVisualizerWidget's ReceiveRules signal with all the rules related
  /// to the selected lane, phase ring and phase if any.
  void OnRulesForLaneRequested();

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

  /// \brief Renders the arrow that will be spawned above a clicked lane.
  void RenderArrow();

  /// \brief Updates all stored defaults of the meshes.
  /// \param[in] key The key indicating which default to update.
  /// \param[in] newValue The new value to set the default value.
  void UpdateMeshDefaults(const std::string& key, bool newValue);

  /// \brief Updates a lane so that if it is selected then all meshes are on, but if it is
  /// not selected, all meshes are set to the default values.
  /// \param[in] id The id of the lane.
  void UpdateLane(const std::string& id);

  /// \brief Updates a branch point so that if it is selected then all meshes are on, but if it is
  /// not selected, all meshes are set to the default values.
  /// \param[in] id The id of the branch point.
  void UpdateBranchPoint(const std::string& id);

  /// \brief Updates all of the selected regions to the default mesh values.
  void UpdateSelectedWithDefault();

  /// \brief Returns the corresponding map key for the marker. The string is
  /// calculated as being `kMarker` + "_" + id.
  /// \param[in] id The id of the marker.
  std::string MarkerMeshMapKey(const std::string& id);

  /// \brief Returns the corresponding map key for the branch point. The string is
  /// calculated as being `kBranchPoint` + "_" + id.
  /// \param[in] id The id of the branch point.
  std::string BranchPointMeshMapKey(const std::string& id);

  /// \brief Returns the corresponding map key for the lane. The string is
  /// calculated as being `kLane` + "_" + id.
  /// \param[in] id The id of the lane.
  std::string LaneMeshMapKey(const std::string& id);

  bool FoundKeyword(const std::string& key, const std::string& keyword);

  /// \brief Obtains the id of a branchpoint or lane from the string used as it's
  /// key value in the mesh map.
  /// \param[in] key The key that the lane or branchpoint is associated with in the
  /// mesh map.
  /// \returns The lane or branchpoint id.
  std::string GetID(const std::string& key);

  /// \brief Widget to hold and modify the visualization status of each layer.
  LayerSelectionWidget* layerSelectionWidget{nullptr};

  /// \brief Widget to hold and modify the visualization status of lane and
  /// branch point text labels.
  LabelSelectionWidget* labelSelectionWidget{nullptr};

  /// \brief Widget to load yaml files.
  MaliputFileSelectionWidget* maliputFileSelectionWidget{nullptr};

  /// \brief World render widget.
  RenderMaliputWidget* renderWidget{nullptr};

  /// \brief Rules visualizer widget.
  RulesVisualizerWidget* rulesVisualizerWidget{nullptr};

  /// \brief Model that holds the meshes and the visualization status.
  std::unique_ptr<MaliputViewerModel> model{};

  // \brief A map that contains the default of the checkbox meshes.
  std::map<std::string, bool> meshDefaults;

  /// \brief Flag used to record the first paint event and sync on the road
  /// meshes when required.
  bool first_run_{false};

  // TODO(anyone): The below string constants are used as keys for the mesh map
  // that contains all of the meshes of the loaded road geometry.  These keys are
  // currently in use in three separate files: `maliput_viewer_widget.cc`,
  // `layer_selection_widget.cc`, and `generate_obj.cc` within maliput.  These keys
  // should be consolidated into a singular location to prevent any accidental
  // disruption of functionality going forward.

  /// \brief Key used to detect an asphalt checkbox event.
  const std::string kAsphalt{"asphalt"};

  /// \brief Keyword used by the checkboxes to indicate the new default for all
  /// of the provided mesh.
  const std::string kAll{"all"};

  /// \brief Key for the marker mesh in the default map.
  const std::string kMarker{"marker"};

  /// \brief Key for the lane mesh in the default map.
  const std::string kLane{"lane"};

  /// \brief Used to detect the below label keys upon a checkbox event as text is the
  /// unique identifier for enabling or disabling all labels.
  const std::string kText{"text"};

  /// \brief Key sent by a lane label checkbox click event.
  const std::string kLaneTextLabel{"lane_" + kText + "_label"};

  /// \brief Key sent by a branch point label checkbox click event.
  const std::string kBranchPointTextLabel{"branch_point_" + kText + "_label"};

  /// \brief Key for the branch point mesh in the default map.
  const std::string kBranchPoint{"branch_point"};

  /// \brief Key for the branch point label mesh in the default map.
  const std::string kBranchPointLabels{"branch_point_labels"};

  /// \brief Key for the lane label mesh in the default map.
  const std::string kLaneLabels{"lane_labels"};
};

}  // namespace gui
}  // namespace delphyne

#endif