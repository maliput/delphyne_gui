// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_ORBITVIEWCONTROL_HH
#define DELPHYNE_GUI_ORBITVIEWCONTROL_HH

#include <mutex>

#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/OrbitViewController.hh>
#include <ignition/rendering/RayQuery.hh>

namespace delphyne {
namespace gui {

/// \def ButtonState_t Possible button states.
enum class ButtonState_t {
  /// \brief Button has been pressed.
  PRESSED,
  /// \brief Button has been released.
  RELEASED,
};

/// \brief Stores the mouse state.
struct MouseButton {
  /// \brief The active button.
  Qt::MouseButton button = Qt::NoButton;
  /// \brief Pressed or released.
  ButtonState_t state = ButtonState_t::RELEASED;
  /// \brief X position of the cursor relative to the widget that received the
  /// event after a press or release.
  int x = 0;
  /// \brief Y position of the cursor relative to the widget that received the
  /// event after a press or release.
  int y = 0;
  /// \brief X position of the cursor relative to the widget that received the
  /// event after a move.
  int motionX = 0;
  /// \brief Y position of the cursor relative to the widget that received the
  /// event after a move.
  int motionY = 0;
  /// \brief Number of pixels in X position during dragging.
  int dragX = 0;
  /// \brief Number of pixels in Y position during dragging.
  int dragY = 0;
  /// \bried When true, a button action needs to be processed.
  bool buttonDirty = false;
  /// \brief When true, a move action needs to be processed.
  bool motionDirty = false;
};

/// \class OrbitViewControl
/// \brief Modifies the camera view according to the mouse events received.
/// Note that the mouse events need to be captured outside of this class.
class OrbitViewControl {
 public:
  /// \brief Default constructor.
  /// \param[in] _cam The camera to be controlled by this class.
  explicit OrbitViewControl(const ignition::rendering::CameraPtr& _cam);

  /// \brief Default Destructor.
  virtual ~OrbitViewControl();

  /// \brief Callback executed after a mouse press event.
  /// \param[in] _e The Qt mouse event.
  void OnMousePress(const QMouseEvent* _e);

  /// \brief Callback executed after a mouse release event.
  /// \param[in] _e The Qt mouse event.
  void OnMouseRelease(const QMouseEvent* _e);

  /// \brief Callback executed after a mouse move event.
  /// \param[in] _e The Qt mouse event.
  void OnMouseMove(const QMouseEvent* _e);

  /// \brief Callback executed after a mouse wheel event.
  /// \param[in] _e The Qt mouse event.
  void OnMouseWheel(const QWheelEvent* _e);

  /// \brief Getter for the ray cast result obtained when the user
  /// clicks any part of the screen.
  const ignition::rendering::RayQueryResult& GetQueryResult() const;

 private:
  /// \brief Update the camera view.
  void Update();

  /// \brief Callback executed after a mouse click event.
  /// \param[in] _e The Qt mouse event.
  /// \param[in] _state The state of the button.
  void OnMouseButtonAction(const QMouseEvent* _e, const ButtonState_t& _state);

  /// \brief Allows to pan, rotate and zoom the camera view.
  ignition::rendering::OrbitViewController viewControl;

  /// \brief The camera to control.
  ignition::rendering::CameraPtr camera;

  /// \brief Let us cast some rays.
  ignition::rendering::RayQueryPtr rayQuery;

  /// \brief Stores the result of a ray casting.
  ignition::rendering::RayQueryResult target;

  /// \brief Stores the mouse state.
  struct MouseButton mouse;

  /// \brief Protect the MouseButton struct from being simultaneously modified
  /// and read.
  std::mutex mouseMutex;
};
}  // namespace gui
}  // namespace delphyne

#endif
