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

#include "maliput_viewer_model.hh"

namespace delphyne {
namespace gui {

/// \brief Loads a road geometry out of a xodr file or a yaml file.
///        Meshes are created and displayed in the scene.
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
  /// @brief Timer event callback which handles the logic to load the meshes when
  ///        the scene is not ready yet.
  void timerEvent(QTimerEvent* _event) override;

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

  /// \brief Renders meshes for the road and the labels.
  void RenderMeshes();

  void MouseClickHandler(QMouseEvent* _mouseEvent);

  ignition::rendering::RayQueryResult ScreenToScene(int screenX, int screenY) const;

  /// \brief Builds visuals for each mesh inside @p _maliputMeshes that is
  /// enabled.
  /// \param[in] _maliputMeshes A map of meshes to render.
  void RenderRoadMeshes(const std::map<std::string, std::unique_ptr<MaliputMesh>>& _maliputMeshes);

  /// \brief Builds visuals for each label inside @p _labels that is enabled.
  /// \param[in] _labels A map of labels to render.
  void RenderLabels(const std::map<std::string, MaliputLabel>& _labels);

  /// \brief Clears all the references to text labels, meshes and the scene.
  void Clear();

  /// \brief Configurate scene and install event filter.
  void Initialize();

  /// \brief Holds the map file path.
  std::string mapFile{""};

  /// \brief Holds the road rulebook file path.
  std::string roadRulebookFile{""};

  /// \brief Holds the traffic light book file path.
  std::string trafficLightBookFile{""};

  /// \brief Holds the phase ring book file path.
  std::string phaseRingBookFile{""};

  /// @brief Triggers an event every `kTimerPeriodInMs` to try to get the scene.
  QBasicTimer timer;

  /// \brief Root visual where all the child mesh visuals are added.
  ignition::rendering::VisualPtr rootVisual;

  /// \brief Map of mesh visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> meshes;

  /// \brief Map of text labels visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> textLabels;

  /// \brief Holds a pointer to the scene.
  ignition::rendering::ScenePtr scene{nullptr};

  /// \brief Holds a pointer to a ray query.
  ignition::rendering::RayQueryPtr rayQuery{nullptr};

  /// \brief Holds a pointer to the camera.
  ignition::rendering::CameraPtr camera{};

  /// \brief Model that holds the meshes and the visualization status.
  std::unique_ptr<MaliputViewerModel> model{};
};

}  // namespace gui
}  // namespace delphyne
