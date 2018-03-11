// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH
#define DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH

#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <drake/automotive/maliput/api/road_geometry.h>

#include <ignition/gui/Plugin.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/transport.hh>

#include "MaliputMesh.hh"
#include "OrbitViewControl.hh"

namespace delphyne {
namespace gui {

/// \class MaliputViewerWidget
/// \brief This is a class that implements a simple ign-gui widget for
/// rendering a scene, using the ign-rendering functionality so as to visualize
/// a Drake's Maliput RoadGeometry.
class MaliputViewerWidget : public ignition::gui::Plugin {
  Q_OBJECT

  /// \def VisualPtr_V
  /// \brief A vector of visual pointers.
  using VisualPtr_V = std::vector<ignition::rendering::VisualPtr>;

  /// \brief All user options that can be configured.
  class UserSettings {
   public:
    /// \brief Default user camera pose.
    ignition::math::Pose3d userCameraPose = {4.0, 4.0, 1.0, 0.0, 0.0, -2.35619};
    /// \brief File path to the maliput map.
    std::string maliputFilePath;
  };

 public:
  /// \brief Default constructor.
  explicit MaliputViewerWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~MaliputViewerWidget();

 protected:
  /// \brief Overridden method to receive Qt paint event.
  /// \param[in] _e The event that happened.
  virtual void paintEvent(QPaintEvent* _e);

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

  /// \brief Creates the root visual of the meshes, all the materials and
  ///        visuals for each mesh and then adds it to the root visual.
  void RenderRoadMeshes();

  /// \brief Fills @p _material with @p _materialName material properties.
  /// \param[in] _materialName The name of the material to fill in. @see
  ///            drake::maliput::mesh::GetMaterialByName for a list of available
  ///            names.
  /// \param[in] _material A valid ignition::rendering::MaterialPtr.
  /// \return True when @p _materialName is valid and @p _material can be
  /// filled.
  bool FillMaterial(
    const std::string& _materialName,
    ignition::rendering::MaterialPtr& _material) const;

  /// \brief Converts @p _geoMeshes into a
  ///        std::map<std::string, std::unique_ptr<ignition::common::Mesh>>
  ///        filling the instance variable meshes.
  /// \param[in] _geoMeshes A map of std::string <--> GeoMesh objects to
  ///            cd /convert.
  void ConvertMeshes(
    const std::map<std::string, drake::maliput::mesh::GeoMesh>& _geoMeshes);

  /// \brief Creates a bare visual and adds it as a child of the scene's root
  ///        visual.
  ignition::rendering::VisualPtr CreateRoadRootVisual();

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

  /// \brief Maliput RoadGeometry pointer.
  std::unique_ptr<const drake::maliput::api::RoadGeometry> roadGeometry;

  /// \brief Map of meshes to hold all the ignition meshes.
  std::map<std::string, std::unique_ptr<ignition::common::Mesh>> meshes;
};
}
}

#endif
