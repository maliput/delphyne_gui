// Copyright 2018 Toyota Research Institute

#ifndef DELPHYNE_GUI_RENDERMALIPUTWIDGET_HH
#define DELPHYNE_GUI_RENDERMALIPUTWIDGET_HH

#include <map>
#include <memory>
#include <string>

#include <ignition/math/Pose3.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>

#include <QtWidgets/QWidget>

#include "MaliputMesh.hh"
#include "MaliputViewerModel.hh"
#include "OrbitViewControl.hh"

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

  /// \brief Builds visuals for each mesh inside @p _maliputMeshes whose state
  /// is State::kOn.
  /// \param[in] _maliputMeshes A map of meshes to render.
  // TODO(agalbachicar): In order to properly modify the visibility of the
  // meshes, we should query visuals for their mesh rather than creating a new
  // one each time. That API is available on this commit:
  // https://bitbucket.org/ignitionrobotics/ign-rendering/commits/5accdc88afc557afc03c811d9e892ccb7f99951a
  // ign-cmake dependency should be switched to 'Components' branch. So, once
  // eveything is stable on default or in a release branch, we should modify
  // this method to properly set the transparency.
  void RenderRoadMeshes(
    const std::map<std::string, std::unique_ptr<MaliputMesh>>& _maliputMeshes);

  /// \brief Overridden method to receive Qt paint event.
  /// \param[in] _e The event that happened.
  virtual void paintEvent(QPaintEvent* _e);

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
  protected slots: void ShowContextMenu(const QPoint &_pos);

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
  void RenderGrid(
      const unsigned int _cellCount,
      const double _cellLength,
      const unsigned int _verticalCellCount,
      const ignition::rendering::MaterialPtr& _material,
      const ignition::math::Pose3d& _pose);

  /// \brief Render a 50x50 grid over the ground plane.
  void RenderGroundPlaneGrid();

  /// \brief Render the origin reference frame.
  void RenderOrigin();

  /// \brief Fills @p _ignitionMaterial with @p _maliputMaterial properties.
  /// \param[in] _maliputMaterial Material properties.
  /// \param[in] _ignitionMaterial A valid ignition::rendering::MaterialPtr.
  /// \return True when @p _maliputMaterial is valid and @p _ignitionMaterial
  /// can be filled.
  bool FillMaterial(
    const drake::maliput::mesh::Material* _maliputMaterial,
    ignition::rendering::MaterialPtr& _ignitionMaterial) const;

  /// \brief Fills a material to be transparent.
  /// \param[in] _material Material to be transparent.
  void CreateTransparentMaterial(
    ignition::rendering::MaterialPtr& _material) const;

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

  /// \brief Pointer to the renderWindow created by this class.
  ignition::rendering::RenderWindowPtr renderWindow;

  /// \brief Pointer to the camera created by this class.
  ignition::rendering::CameraPtr camera;

  /// \brief The scene.
  ignition::rendering::ScenePtr scene;

  /// \brief Controls the view of the scene.
  std::unique_ptr<OrbitViewControl> orbitViewControl;

  /// \brief A pointer to the rendering engine
  ignition::rendering::RenderEngine* engine;

  /// \brief Store all the user settings.
  UserSettings userSettings;

  /// \brief Root visual where all the child mesh visuals are added.
  ignition::rendering::VisualPtr rootVisual;

  /// \brief Map of mesh visual pointers.
  std::map<std::string, ignition::rendering::VisualPtr> meshes;
};

}
}

#endif