// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_RENDERMALIPUTWIDGET_HH
#define DELPHYNE_GUI_RENDERMALIPUTWIDGET_HH

#include <map>
#include <memory>
#include <string>

#include <ignition/math/Pose3.hh>
#include <ignition/rendering/RayQuery.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <maliput-utilities/generate_obj.h>
#include <maliput-utilities/mesh.h>
#include <maliput/api/rules/traffic_lights.h>

#include <QtWidgets/QWidget>

#include "arrow_mesh.hh"
#include "maliput_viewer_model.hh"
#include "orbit_view_control.hh"
#include "selector.hh"
#include "traffic_light_manager.hh"

namespace delphyne {
namespace gui {

/// \brief Renders a group of meshes to allow the correct
class RenderMaliputWidget : public QWidget {
  Q_OBJECT

  /// \brief All user options that can be configured.
  class UserSettings {
   public:
    /// \brief Default user camera pose.
    ignition::math::Pose3d userCameraPose = {4.0, 4.0, 1.0, 0.0, 0.0, -2.35619};
  };

 public:
  /// \brief Constructor.
  explicit RenderMaliputWidget(QWidget* parent = 0);

  /// \brief Destructor.
  virtual ~RenderMaliputWidget();

  /// \brief Builds visuals for each mesh inside @p _maliputMeshes that is
  /// enabled.
  /// \details When meshes are disabled but were previously created, their
  /// material are set to transparent. The same happens to those which
  /// are both enabled and visibility flag is false. Otherwise, the mesh is
  /// created (if necessary) with its appropriate material.
  /// \param[in] _maliputMeshes A map of meshes to render.
  void RenderRoadMeshes(const std::map<std::string, std::unique_ptr<MaliputMesh>>& _maliputMeshes);

  /// \brief Builds visuals for each label inside @p _labels that is enabled.
  /// \details When labels are disabled but were previously created, their
  /// materials are set to transparent. The same happens to those which are
  /// both enabled and visibility flag is false. Otherwise, the text visual is
  /// created (if necessary) and its appropriate material is assigned.
  /// \param[in] _labels A map of labels to render.
  void RenderLabels(const std::map<std::string, MaliputLabel>& _labels);

  /// \brief Create and render an arrow that will be positioned slightly above the selected road.
  void RenderArrow();

  /// \brief Render all traffic lights for a given vector of them.
  /// \param[in] _traffic_lights Vector containing a set of traffic lights for the current road network.
  void RenderTrafficLights(const std::vector<const maliput::api::rules::TrafficLight*>& _trafficLights);

  /// \brief Set the state of all the bulbs by the traffic light manager.
  /// \param[in] _bulb_states Unordered map containing the new state for each bulb in the road network.
  void SetStateOfTrafficLights(const maliput::api::rules::BulbStates& _bulbStates);

  /// \brief Move the arrow based on the distance travelled by the camera's ray distance.
  /// \param[in] _distance Distance travelled from the camera's world position to the clicked object.
  /// \param[in] _worldPosition Position where the camera's ray hit.
  void PutArrowAt(double _distance, const ignition::math::Vector3d& _worldPosition);

  /// \brief Change visibility of the arrow mesh.
  /// \param[in] _visible Visibility of the arrow.
  void SetArrowVisibility(bool _visible);

  /// \brief Deselects the currently selected region.
  void DeselectAll();

  /// \brief Gets the currently selected lanes.
  /// \returns A vector of the lane_id's which are selected.
  std::vector<std::string> GetSelectedLanes();
  
  /// \brief Gets the currently selected branch pointss.
  /// \returns A vector of the branch_point_id's which are selected.
  std::vector<std::string> GetSelectedBranchPoints();

  /// \brief Clears all the references to text labels, meshes and the scene.
  void Clear();

  /// \brief Selects a lane if it is not selected and deselects a lane if it is selected.
  /// \param[in] _lane Lane to select or deselect.
  void SelectLane(const maliput::api::Lane* _lane);

  /// \brief Finds if the passed in lane corresponding to the lane id is currently selected or not.
  /// \param[in] _laneId Lane id of the lane to be evaluated.
  /// \returns Boolean that determines whether the lane is selected or not.
  bool IsSelected(const std::string& _laneId);

  /// \brief Finds if the passed in lane is currently selected or not.
  /// \param[in] _lane Lane to be evaluated.
  /// \returns Boolean that determines whether the lane is selected or not.
  bool IsSelected(const maliput::api::Lane* _lane);

  /// \brief Overridden method to receive Qt paint event.
  /// \param[in] _e The event that happened.
  virtual void paintEvent(QPaintEvent* _e);

 signals:
  /// \brief Signal that gets fired when a click happens on a visual (mesh)
  void VisualClicked(ignition::rendering::RayQueryResult rayResult);

  /// \brief Signal that gets fired upon a call to deselect everything
  void SetAllToDefault();

 protected:
  /// \brief Overridden method to receive Qt show event.
  /// \param[in] _e The event that happened.
  virtual void showEvent(QShowEvent* _e);

  /// \brief Overridden method to receive Qt resize event.
  /// \param[in] _e The event that happened.
  virtual void resizeEvent(QResizeEvent* _e);

  /// \brief Overridden method to receive Qt move event.
  /// \param[in] _e The event that happened.
  virtual void moveEvent(QMoveEvent* _e);

  /// \brief Overridden method to receive Qt mouse press event.
  /// \param[in] _e The mouse event that happened.
  virtual void mousePressEvent(QMouseEvent* _event);

  /// \brief Overridden method to receive Qt mouse release event.
  /// \param[in] _e The mouse event that happened.
  virtual void mouseReleaseEvent(QMouseEvent* _event);

  /// \brief Overridden method to receive Qt mouse move event.
  /// \param[in] _e The mouse event that happened.
  virtual void mouseMoveEvent(QMouseEvent* _event);

  /// \brief Overridden method to receive Qt mouse wheel event.
  /// \param[in] _e The mouse event that happened.
  virtual void wheelEvent(QWheelEvent* _event);

  /// \brief Override paintEngine to stop Qt From trying to draw on top of
  /// render window.
  /// \return NULL.
  virtual QPaintEngine* paintEngine() const;

  // Documentation inherited
 protected slots:
  void ShowContextMenu(const QPoint& _pos);

  void TickTrafficLights();

 private:
  /// \brief Internal method to create the render window the first time
  /// RenderWidget::showEvent is called.
  void CreateRenderWindow();

  /// \brief Render a squared grid.
  /// \param[in] _cellCount the number of cells per side.
  /// \param[in] _cellLength the size of each cell.
  /// \param[in] _verticalCellCount the number of vertical layers.
  /// \param[in] _material the material used to draw the lines.
  /// \param[in] _pose the pose of the grid.
  void RenderGrid(const unsigned int _cellCount, const double _cellLength, const unsigned int _verticalCellCount,
                  const ignition::rendering::MaterialPtr& _material, const ignition::math::Pose3d& _pose);

  /// \brief Render a 50x50 grid over the ground plane.
  void RenderGroundPlaneGrid();

  /// \brief Render the origin reference frame.
  void RenderOrigin();

  /// \brief Fills @p _ignitionMaterial with @p _maliputMaterial properties.
  /// \param[in] _maliputMaterial Material properties.
  /// \param[in] _ignitionMaterial A valid ignition::rendering::MaterialPtr.
  /// \return True when @p _maliputMaterial is valid and @p _ignitionMaterial
  /// can be filled.
  bool FillMaterial(const maliput::utility::Material* _maliputMaterial,
                    ignition::rendering::MaterialPtr& _ignitionMaterial) const;

  /// \brief Fills a material for a lane label.
  /// \param[in] _material Material to be filled.
  void CreateLaneLabelMaterial(ignition::rendering::MaterialPtr& _material) const;

  /// \brief Fills a material for a branch point label.
  /// \param[in] _material Material to be filled.
  void CreateBranchPointLabelMaterial(ignition::rendering::MaterialPtr& _material) const;

  /// \brief Creates a bare visual and adds it as a child of the scene's root
  /// visual.
  void CreateRoadRootVisual();

  /// \brief Refreshes the ignition rendering viewport.
  /// TODO(basicNew): For some reason I can't understand, the required repaint
  /// after a mouse event stopped being triggered. As a quick workaround I
  /// created this event an explicitly call it on every mouse event, but we
  /// should definitely research the underlying cause.
  void UpdateViewport();

  /// \brief The frequency at which we'll do an update on the widget.
  const int kUpdateTimeFrequency = static_cast<int>(std::round(1000.0 / 60.0));

  /// \brief Pointer to timer to call update on a periodic basis.
  QTimer* updateTimer = nullptr;
  QTimer* trafficLightsTickTimer = nullptr;

  /// \brief Pointer to the renderWindow created by this class.
  ignition::rendering::RenderWindowPtr renderWindow;

  /// \brief Pointer to the camera created by this class.
  ignition::rendering::CameraPtr camera;

  /// \brief The scene.
  ignition::rendering::ScenePtr scene;

  /// \brief Controls the view of the scene.
  std::unique_ptr<OrbitViewControl> orbitViewControl;

  /// \brief Arrow that points the location clicked in the visualizer.
  std::unique_ptr<ArrowMesh> arrow;

  /// \brief Selector used for selecting clicked lanes in the visualizer.
  std::unique_ptr<Selector> selector;

  /// \brief Manager of traffic lights visualization.
  std::unique_ptr<TrafficLightManager> trafficLightManager;

  /// \brief A pointer to the rendering engine
  ignition::rendering::RenderEngine* engine;

  /// \brief Store all the user settings.
  UserSettings userSettings;

  /// \brief Root visual where all the child mesh visuals are added.
  ignition::rendering::VisualPtr rootVisual;

  /// \brief Map of mesh visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> meshes;

  /// \brief Map of text labels visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> textLabels;

  /// \brief Scale in the X axis for the cubes used in the selector.
  static constexpr double kSelectorScaleX = 0.3;
  /// \brief Scale in the Y axis for the cubes used in the selector.
  static constexpr double kSelectorScaleY = 0.5;
  /// \brief Scale in the Z axis for the cubes used in the selector.
  static constexpr double kSelectorScaleZ = 0.1;
  /// \brief Tolerance used for the selector.
  static constexpr double kSelectorMinTolerance = 0.6;
  /// \brief Max amount of cubes used for the selector.
  static constexpr int kSelectorPoolSize = 50;
  /// \brief Number of lanes to pre-initialize in selector cubes vector.
  static constexpr int kNumLanes = 15;
  /// \brief Every how much time should the lights blink in miliseconds.
  static constexpr int kTrafficLightsTickPeriod = 500;
};

}  // namespace gui
}  // namespace delphyne

#endif
