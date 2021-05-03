// Copyright 2021 Toyota Research Institute
#pragma once

#include <memory>
#include <string>

#include <maliput/api/road_geometry.h>

#include <ignition/common/MouseEvent.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/RayQuery.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/Scene.hh>

#include "arrow_mesh.hh"
#include "maliput_viewer_model.hh"
#include "selector.hh"

namespace delphyne {
namespace gui {

/// \brief Loads a road geometry out of a xodr file or a yaml file.
///        Meshes are created and displayed in the scene.
///
/// ## Configuration
///
/// * \<main_scene_plugin_title\> : Title of the Scene3D plugin instance that manages the main scene.
///                                 Defaults to '3D Scene'.
class MaliputViewerPlugin : public ignition::gui::Plugin {
  Q_OBJECT

  /// Property used to load the default state of layers visualization in its correspondant UI's checkboxes.
  Q_PROPERTY(QList<bool> layerCheckboxes READ LayerCheckboxes NOTIFY LayerCheckboxesChanged)

  /// Property used to load the default state of labels visualization in its correspondant UI's checkboxes.
  Q_PROPERTY(QList<bool> labelCheckboxes READ LabelCheckboxes NOTIFY LabelCheckboxesChanged)

 public:
  /// \brief Default constructor.
  MaliputViewerPlugin();

  /// Called by Ignition GUI when plugin is instantiated.
  /// \param[in] _pluginElem XML configuration for this plugin.
  void LoadConfig(const tinyxml2::XMLElement* _pluginElem) override;

  /// Called when a new RoadNetwork is loaded to default the checkboxes' state
  /// in the layers selection panel for the meshes.
  Q_INVOKABLE QList<bool> LayerCheckboxes() const;

  /// Called when a new RoadNetwork is loaded to default the checkboxes' state
  /// in the label selection panel.
  Q_INVOKABLE QList<bool> LabelCheckboxes() const;

 signals:
  /// \brief Signal emitted to reset the checkboxes' state for the layers visualization
  ///        when a new RoadNetwork is loaded.
  void LayerCheckboxesChanged();

  /// \brief Signal emitted to reset the checkboxes' state for the label visualization
  ///        when a new RoadNetwork is loaded.
  void LabelCheckboxesChanged();

 protected:
  /// \brief Timer event callback which handles the logic to load the meshes when
  ///        the scene is not ready yet.
  void timerEvent(QTimerEvent* _event) override;

  /// \brief Filters QMouseEvents from a Scene3D plugin whose title matches with <main_scene_plugin_title>.
  ///        Filters ignition::gui::events::Render events to update the meshes and labels of the roads and the animation
  ///        of the arrow mesh.
  /// \details To make this method be called by Qt Event System, install the event filter in target object.
  ///          \see QObject::installEventFilter() method.
  bool eventFilter(QObject* _obj, QEvent* _event) override;

 protected slots:
  /// \brief Clears the visualizer, loads a RoadNetwork and update the GUI with meshes and labels.
  /// \param[in] _mapFile The path to the map file to load and visualize.
  /// \param[in] _roadRulebookFile The path to the road rulebook file.
  /// \param[in] _trafficLightBookFile The path to the traffic light book file.
  /// \param[in] _phaseRingBookFile The path to the phase ring book file.
  void OnNewRoadNetwork(const QString& _mapFile, const QString& _roadRulebookFile, const QString& _trafficLightBookFile,
                        const QString& _phaseRingBookFile);

  /// \brief Change the visibility of the layers.
  /// \param[in] _layer The layer to change its visibility.
  /// \param[in] _state The state of the visibility checkbox.
  void OnNewMeshLayerSelection(const QString& _layer, bool _state);

  /// \brief Change the visibility of the labels.
  /// \param[in] _label Name of the label.
  /// \param[in] _state The state of the visibility checkbox.
  void OnNewTextLabelSelection(const QString& _label, bool _state);

 private:
  /// @brief The period in milliseconds of the timer to try to load the meshes.
  static constexpr int kTimerPeriodInMs{500};

  /// @brief The scene name.
  static constexpr char const* kSceneName = "scene";

  /// @brief The Scene3D instance holding the main scene.
  static constexpr char const* kMainScene3dPlugin = "Main3DScene";

  /// @brief The rendering engine name.
  static constexpr char const* kEngineName = "ogre";

  /// \brief Fills a material for a lane label.
  /// \param[in] _material Material to be filled.
  static void CreateLaneLabelMaterial(ignition::rendering::MaterialPtr& _material);

  /// \brief Fills a material for a branch point label.
  /// \param[in] _material Material to be filled.
  static void CreateBranchPointLabelMaterial(ignition::rendering::MaterialPtr& _material);

  /// \brief Fills @p _ignitionMaterial with @p _maliputMaterial properties.
  /// \param[in] _maliputMaterial Material properties.
  /// \param[in] _ignitionMaterial A valid ignition::rendering::MaterialPtr.
  /// \return True when @p _maliputMaterial is valid and @p _ignitionMaterial
  /// can be filled.
  static bool FillMaterial(const maliput::utility::Material* _maliputMaterial,
                           ignition::rendering::MaterialPtr& _ignitionMaterial);

  /// \brief Holds flags used to render meshes.
  struct RenderMeshesOption {
    /// \brief Set both #executeMeshRendering and #executeLabelRendering to true;
    void RenderAll() {
      executeMeshRendering = true;
      executeLabelRendering = true;
    }
    /// Indicates whether meshes must be rendered.
    bool executeMeshRendering{false};
    /// Indicates whether labels must be rendered.
    bool executeLabelRendering{false};
  } renderMeshesOption;

  /// \brief Builds visuals for each mesh inside @p _maliputMeshes that is
  /// enabled.
  /// \param[in] _maliputMeshes A map of meshes to render.
  void RenderRoadMeshes(const std::map<std::string, std::unique_ptr<MaliputMesh>>& _maliputMeshes);

  /// \brief Builds visuals for each label inside @p _labels that is enabled.
  /// \param[in] _labels A map of labels to render.
  void RenderLabels(const std::map<std::string, MaliputLabel>& _labels);

  /// \brief Clears all the references to text labels, meshes and the scene.
  void Clear();

  /// \brief Configurate scene and install event filter for filtering QMouseEvents.
  /// \details To install the event filter the Scene3D plugin hosting the scene
  ///          is expected to be titled as #kMainScene3dPlugin.
  void Initialize();

  /// \brief Handles the left click mouse event.
  /// @param[in] _mouseEvent QMouseEvent pointer.
  void MouseClickHandler(const QMouseEvent* _mouseEvent);

  /// \brief Performs a raycast on the screen.
  /// \param[in] screenX X screen's coordinate.
  /// \param[in] screenY Y screen's coordinate.
  /// \return A ignition::rendering::RayQueryResult.
  ignition::rendering::RayQueryResult ScreenToScene(int _screenX, int _screenY) const;

  /// \brief Filters by title all the children of the parent plugin.
  /// \param _pluginTitle Title of the ignition::gui::plugin.
  /// \return A pointer to the plugin if found, nullptr otherwise.
  ignition::gui::Plugin* FilterPluginsByTitle(const std::string& _pluginTitle);

  /// \brief Updates all stored visual defaults for the meshes and labels.
  /// \param[in] _key The key indicating which default to update.
  /// \param[in] _newValue The new value to set the default value.
  void UpdateObjectVisualDefaults(const std::string& _key, bool _newValue);

  /// \brief Updates a lane so that if it is selected then all meshes are on, but if it is
  /// not selected, all meshes are set to the default values.
  /// \param[in] _id The id of the lane.
  void UpdateLane(const std::string& _id);

  /// \brief Updates a branch point so that if it is selected then all meshes are on, but if it is
  /// not selected, all meshes are set to the default values.
  /// \param[in] _id The id of the branch point.
  void UpdateBranchPoint(const std::string& _id);

  /// \brief Updates all of the selected regions to the default mesh values.
  void UpdateSelectedLanesWithDefault();

  /// Keys used by the checkbox logic to visualize different layers and by
  /// the default map #objectVisualDefaults.
  /// @{
  /// \brief Key used to detect an asphalt checkbox event.
  const std::string kAsphalt{"asphalt"};
  /// \brief Keyword used by the checkboxes to indicate the new default for all
  /// of the provided mesh.
  const std::string kAll{"all"};
  /// \brief Key for the marker mesh in the default map.
  const std::string kMarker{"marker"};
  /// \brief Key for the lane mesh in the default map.
  const std::string kLane{"lane"};
  /// \brief Key for the branch point mesh in the default map.
  const std::string kBranchPoint{"branch_point"};
  /// \brief Key used to identify labels from meshes.
  const std::string kLabels{"_labels"};
  /// \brief Key for the branch point label mesh in the default map.
  const std::string kBranchPointLabels{kBranchPoint + kLabels};
  /// \brief Key for the lane label mesh in the default map.
  const std::string kLaneLabels{kLane + kLabels};
  /// @}

  /// \brief Holds the map file path.
  std::string mapFile{""};

  /// \brief Holds the road rulebook file path.
  std::string roadRulebookFile{""};

  /// \brief Holds the traffic light book file path.
  std::string trafficLightBookFile{""};

  /// \brief Holds the phase ring book file path.
  std::string phaseRingBookFile{""};

  /// \brief Holds the title of the main Scene3D plugin.
  std::string mainScene3dPluginTitle{"3D Scene"};

  /// @brief Triggers an event every `kTimerPeriodInMs` to try to get the scene.
  QBasicTimer timer;

  /// \brief Root visual where all the child mesh visuals are added.
  ignition::rendering::VisualPtr rootVisual;

  /// \brief Map of mesh visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> meshes;

  /// \brief Map of text labels visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> textLabels;

  /// \brief A map that contains the default of the checkbox for meshes and labels.
  std::map<std::string, bool> objectVisualDefaults;

  /// \brief Holds a pointer to the scene.
  ignition::rendering::ScenePtr scene{nullptr};

  /// \brief Holds a pointer to a ray query.
  ignition::rendering::RayQueryPtr rayQuery{nullptr};

  /// \brief Holds a pointer to the camera.
  ignition::rendering::CameraPtr camera{};

  /// \brief Model that holds the meshes and the visualization status.
  std::unique_ptr<MaliputViewerModel> model{};

  /// \brief Arrow that points the location clicked in the visualizer.
  std::unique_ptr<ArrowMesh> arrow;

  /// \brief Selector used for selecting clicked lanes in the visualizer.
  std::unique_ptr<Selector> selector;
};

}  // namespace gui
}  // namespace delphyne
