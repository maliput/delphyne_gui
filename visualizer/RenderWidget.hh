// Copyright 2017 Open Source Robotics Foundation
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef DELPHYNE_GUI_RENDERWIDGET_HH
#define DELPHYNE_GUI_RENDERWIDGET_HH

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/transport.hh>

// Forward declarations.
namespace ignition {
  namespace msgs {
    class Model;
    class PosesStamped;
    class Visual;
  }
}

namespace delphyne {
namespace gui {

/// \class RenderWidget
/// \brief This is a class that implements a simple ign-gui widget for
/// rendering a scene, using the ign-rendering functionality.
class RenderWidget: public ignition::gui::Plugin
{
  Q_OBJECT

  /// \def VisualPtr_V
  /// \brief A vector of visual pointers.
  using VisualPtr_V = std::vector<ignition::rendering::VisualPtr>;

  public:
    /// \brief Default constructor.
    explicit RenderWidget(QWidget *parent = 0);

    /// \brief Default Destructor.
    virtual ~RenderWidget();

  /// \brief Callback to set a model for the first time.
  /// \param[in] _msg The new model.
  public slots: void SetInitialModel(const ignition::msgs::Model &_msg);

  /// \brief Callback to update the scene.
  /// \param[in] _msg Message containing an update.
  public slots: void UpdateScene(const ignition::msgs::PosesStamped &_msg);

  /// \brief Notify that there's a new model.
  /// \param[in] _msg The new model.
  signals: void NewInitialModel(const ignition::msgs::Model &_msg);

  /// \brief Notify that there's a new draw update.
  /// \param[in] _msg Message contining the update.
  signals: void NewDraw(const ignition::msgs::PosesStamped &_msg);

  protected:
    /// \brief Overridden method to receive Qt paint event.
    /// \param[in] _e The event that happened.
    virtual void paintEvent(QPaintEvent *_e);

    /// \brief Overridden method to receive Qt show event.
    /// \param[in] _e The event that happened.
    virtual void showEvent(QShowEvent *_e);

    /// \brief Overridden method to receive Qt resize event.
    /// \param[in] _e The event that happened.
    virtual void resizeEvent(QResizeEvent *_e);

    /// \brief Overridden method to receive Qt move event.
    /// \param[in] _e The event that happened.
    virtual void moveEvent(QMoveEvent *_e);

    /// \brief Override paintEngine to stop Qt From trying to draw on top of
    /// render window.
    /// \return NULL.
    virtual QPaintEngine *paintEngine() const;

  private:
    /// \brief Internal method to create the render window the first time
    /// RenderWidget::showEvent is called.
    void CreateRenderWindow();

    /// \brief Load a new model.
    /// \param[in] _msg The message containing the model.
    void OnInitialModel(const ignition::msgs::Model &_msg);

    /// \brief Update an existing visual.
    /// \param[in] _msg The pose of the new visual.
    void OnUpdateScene(const ignition::msgs::PosesStamped &_msg);

    /// \brief Create a visual and material before rendering.
    /// \param[in] _vis The input message containing the visual specs.
    /// \param[out] _visual The new visual.
    /// \param[out] _material The new material.
    bool CreateVisual(const ignition::msgs::Visual &_vis,
                      ignition::rendering::VisualPtr &_visual,
                      ignition::rendering::MaterialPtr &_material) const;

    /// \brief Helper function used during the last phase of rendering.
    /// \param[in] _vis The input message containing the visual specs.
    /// \param[in] _scale The scale vector.
    /// \param[in] _material The material.
    /// \param[in, out] _visual The visual that is going to be rendered.
    ignition::rendering::VisualPtr Render(const ignition::msgs::Visual &_vis,
                                         const ignition::math::Vector3d &_scale,
                              const ignition::rendering::MaterialPtr &_material,
                                       ignition::rendering::VisualPtr &_visual);

    /// \brief Render a new box.
    /// \param[in] _vis the visual containing the properties of the object.
    /// \return A pointer to the new visual.
    ignition::rendering::VisualPtr RenderBox(
      const ignition::msgs::Visual &_vis,
      ignition::rendering::VisualPtr &_visual,
      ignition::rendering::MaterialPtr &_material);

    /// \brief Render a new sphere.
    /// \param[in] _vis the visual containing the properties of the object.
    /// \return A pointer to the new visual.
    ignition::rendering::VisualPtr RenderSphere(
      const ignition::msgs::Visual &_vis,
      ignition::rendering::VisualPtr &_visual,
      ignition::rendering::MaterialPtr &_material);

    /// \brief Render a new cylinder.
    /// \param[in] _vis the visual containing the properties of the object.
    /// \return A pointer to the new visual.
    ignition::rendering::VisualPtr RenderCylinder(
      const ignition::msgs::Visual &_vis,
      ignition::rendering::VisualPtr &_visual,
      ignition::rendering::MaterialPtr &_material);

    /// \brief Render a new mesh.
    /// \param[in] _vis the visual containing the properties of the object.
    /// \return A pointer to the new visual.
    ignition::rendering::VisualPtr RenderMesh(
      const ignition::msgs::Visual &_vis);

    /// \brief The frequency at which we'll do an update on the widget.
    const int kUpdateTimeFrequency =
      static_cast<int>(std::round(1000.0 / 60.0));

    /// \brief Pointer to timer to call update on a periodic basis.
    QTimer *updateTimer = nullptr;

    /// \brief Pointer to the renderWindow created by this class.
    ignition::rendering::RenderWindowPtr renderWindow;

    /// \brief Pointer to the camera created by this class.
    ignition::rendering::CameraPtr camera;

    /// \brief A transport node.
    ignition::transport::Node node;

    /// \brief The scene.
    ignition::rendering::ScenePtr scene;

    /// \brief Is the scene initialized?.
    bool initializedScene;

    /// \brief This the data structure that stores the pointers to all visuals.
    /// The key is the model Id.
    /// The value is another map, where the key is the link name, and the
    /// value is the vector of visuals associated to the link.
    std::map<uint32_t, std::map<std::string, VisualPtr_V>> allVisuals;
};

}
}

#endif
