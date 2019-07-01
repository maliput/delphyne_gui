// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_RENDERWIDGET_HH
#define DELPHYNE_GUI_RENDERWIDGET_HH

#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <delphyne/protobuf/scene_request.pb.h>

#include <ignition/gui/Plugin.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Rand.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/transport.hh>

#include "orbit_view_control.hh"

// Forward declarations.
namespace tinyxml2 {
class XMLElement;
}
namespace ignition {
namespace msgs {
class Model;
class Model_V;
class Scene;
class Visual;
}
}

namespace delphyne {
namespace gui {

/// \class RenderWidget
/// \brief This is a class that implements a simple ign-gui widget for
/// rendering a scene, using the ign-rendering functionality.
class RenderWidget : public ignition::gui::Plugin {
  Q_OBJECT

  /// \def VisualPtr_V
  /// \brief A vector of visual pointers.
  using VisualPtr_V = std::vector<ignition::rendering::VisualPtr>;

  /// \brief All user options that can be configured.
  class UserSettings {
    /// \brief Default user camera pose.
   public:
    ignition::math::Pose3d userCameraPose = {4.0, 4.0, 1.0, 0.0, 0.0, -2.35619};
  };

 public:
  /// \brief Default constructor.
  explicit RenderWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~RenderWidget();

  /// \brief Overridden method to load user configuration.
  /// \param[in] _pluginElem The data containing the configuration.
  virtual void LoadConfig(const tinyxml2::XMLElement* _pluginElem);

  /// \brief Overridden method to get the configuration XML as a string.
  /// \return Config element.
  virtual std::string ConfigStr() const;

  /// \brief Callback to set the initial scene..
  /// \param[in] _msg The new scene.
 public slots:
  void SetInitialScene(const ignition::msgs::Scene& _msg);

  /// \brief Callback to update the scene.
  /// \param[in] _msg Message containing an update.
 public slots:
  void UpdateScene(const ignition::msgs::Model_V& _msg);

  /// \brief Notify that there's a new scene.
  /// \param[in] _msg The new scene.
 signals:
  void NewInitialScene(const ignition::msgs::Scene& _msg);

  /// \brief Notify that there's a new draw update.
  /// \param[in] _msg Message contining the update.
 signals:
  void NewDraw(const ignition::msgs::Model_V& _msg);

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
 protected slots:
  void ShowContextMenu(const QPoint& _pos);

 private:
  /// \brief Set the initial scene
  /// \param[in] request The scene to be loaded
  void OnSetScene(
      const ignition::msgs::Scene& request);

  /// \brief Internal method to create the render window the first time
  /// RenderWidget::showEvent is called.
  void CreateRenderWindow();

  /// \brief Load an entire model in the scene
  /// \param[in] _msg The new model.
  void LoadModel(const ignition::msgs::Model& _msg);

  /// \brief Update an existing visual.
  /// \param[in] _msg The pose of the new visual.
  void OnUpdateScene(const ignition::msgs::Model_V& _msg);

  /// \brief Create a visual and material before rendering.
  /// \param[in] _vis The input message containing the visual specs.
  /// \param[out] _visual The new visual.
  /// \param[out] _material The new material.
  bool CreateVisual(const ignition::msgs::Visual& _vis,
                    ignition::rendering::VisualPtr& _visual,
                    ignition::rendering::MaterialPtr& _material) const;

  /// \brief Creates a root visual for a given link
  /// and adds it as a child of the scene
  /// \param[in] _link The robot link
  /// \param[in] _robotID The robot uuid
  /// \return The root visual
  ignition::rendering::VisualPtr CreateLinkRootVisual(
      ignition::msgs::Link& _link, const uint32_t _robotID);

  /// \brief Helper function used during the last phase of rendering.
  /// \param[in] _vis The input message containing the visual specs.
  /// \param[in] _scale The scale vector.
  /// \param[in] _material The material.
  /// \param[in, out] _visual The visual that is going to be rendered.
  ignition::rendering::VisualPtr Render(
      const ignition::msgs::Visual& _vis,
      const ignition::math::Vector3d& _scale,
      const ignition::rendering::MaterialPtr& _material,
      ignition::rendering::VisualPtr& _visual);

  /// \brief Render a new box.
  /// \param[in] _vis the visual containing the properties of the object.
  /// \return A pointer to the new visual.
  ignition::rendering::VisualPtr RenderBox(
      const ignition::msgs::Visual& _vis,
      ignition::rendering::VisualPtr& _visual,
      ignition::rendering::MaterialPtr& _material);

  /// \brief Render a new sphere.
  /// \param[in] _vis the visual containing the properties of the object.
  /// \return A pointer to the new visual.
  ignition::rendering::VisualPtr RenderSphere(
      const ignition::msgs::Visual& _vis,
      ignition::rendering::VisualPtr& _visual,
      ignition::rendering::MaterialPtr& _material);

  /// \brief Render a new cylinder.
  /// \param[in] _vis the visual containing the properties of the object.
  /// \return A pointer to the new visual.
  ignition::rendering::VisualPtr RenderCylinder(
      const ignition::msgs::Visual& _vis,
      ignition::rendering::VisualPtr& _visual,
      ignition::rendering::MaterialPtr& _material);

  /// \brief Render a new mesh.
  /// \param[in] _vis the visual containing the properties of the object.
  /// \return A pointer to the new visual.
  ignition::rendering::VisualPtr RenderMesh(const ignition::msgs::Visual& _vis);

  /// \brief The frequency at which we'll do an update on the widget.
  const int kUpdateTimeFrequency = static_cast<int>(std::round(1000.0 / 60.0));

  /// \brief Pointer to timer to call update on a periodic basis.
  QTimer* updateTimer = nullptr;

  /// \brief Pointer to the renderWindow created by this class.
  ignition::rendering::RenderWindowPtr renderWindow;

  /// \brief Pointer to the camera created by this class.
  ignition::rendering::CameraPtr camera;

  /// \brief Pointer to the main directional light created by this class.
  ignition::rendering::DirectionalLightPtr mainDirectionalLight;

  /// \brief A transport node.
  ignition::transport::Node node;

  /// \brief The scene.
  ignition::rendering::ScenePtr scene;

  /// \brief Is the scene initialized?.
  bool initializedScene;

  /// \brief The scene request message to be sent to the backend
  ignition::msgs::SceneRequest sceneRequestMsg;

  /// \brief Controls the view of the scene.
  std::unique_ptr<OrbitViewControl> orbitViewControl;

  /// \brief This the data structure that stores the pointers to all visuals.
  /// The key is the model Id.
  /// The value is another map, where the key is the link name, and the
  /// value is a root visual of which all the link's visuals are childs
  std::map<uint32_t, std::map<std::string, ignition::rendering::VisualPtr>>
      allVisuals;

  /// \brief A pointer to the rendering engine
  ignition::rendering::RenderEngine* engine;

  /// \brief Store all the user settings.
  UserSettings userSettings;

  /// \brief Min bounding box of the whole scene
  ignition::math::Vector3d minBBScene;

  /// \brief Max bounding box of the whole scene
  ignition::math::Vector3d maxBBScene;

  /// \brief Are we casting shadows by default?
  bool kCastShadowsByDefault{true};
};
}
}

#endif
