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

#include <cmath>
#include <functional>
#include <iostream>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>

#include <ignition/gui/Iface.hh>
#include <ignition/gui/Plugin.hh>

#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderTarget.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>

#include "RenderWidget.hh"

Q_DECLARE_METATYPE(ignition::msgs::Model)
Q_DECLARE_METATYPE(ignition::msgs::PosesStamped)

namespace delphyne {
namespace gui {

void RenderWidget::load_robot_cb(const ignition::msgs::Model &_msg)
{
  ignmsg << "Saw new model msg on subscription" << std::endl;
  emit newInitialModel(_msg);
}

void RenderWidget::draw_cb(const ignition::msgs::PosesStamped &_msg)
{
  ignmsg << "Saw new draw msg on subscription" << std::endl;
  emit newDraw(_msg);
}

RenderWidget::RenderWidget(QWidget *parent) : Plugin(), initialized_scene(false)
{
  qRegisterMetaType<ignition::msgs::Model>();
  qRegisterMetaType<ignition::msgs::PosesStamped>();

  this->setAttribute(Qt::WA_OpaquePaintEvent, true);
  this->setAttribute(Qt::WA_PaintOnScreen, true);
  this->setAttribute(Qt::WA_NoSystemBackground, true);

  this->setFocusPolicy(Qt::StrongFocus);
  this->setMouseTracking(true);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // The below block means that every time the updateTime expires, we do an
  // update on the widget.  Later on, we call the start() method to start this
  // time at a fixed frequency.  Note that we do not start this timer until the
  // first time that showEvent() is called.
  this->updateTimer = new QTimer(this);
  QObject::connect(this->updateTimer, SIGNAL(timeout()), this, SLOT(update()));
  QObject::connect(this, SIGNAL(newInitialModel(const ignition::msgs::Model &)), this, SLOT(setInitialModel(const ignition::msgs::Model &)));
  QObject::connect(this, SIGNAL(newDraw(const ignition::msgs::PosesStamped &)), this, SLOT(updateScene(const ignition::msgs::PosesStamped &)));

  this->node.Subscribe("/DRAKE_VIEWER_LOAD_ROBOT", &RenderWidget::load_robot_cb, this);
  this->node.Subscribe("/DRAKE_VIEWER_DRAW", &RenderWidget::draw_cb, this);

  this->setMinimumHeight(100);
}

RenderWidget::~RenderWidget() {}

bool RenderWidget::renderBox(ignition::msgs::Link &_link)
{
  ignmsg << "Has a box!" << std::endl;
  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr box = this->scene->CreateVisual();
  if (box == nullptr) {
    ignerr << "Failed to create visual" << std::endl;
    return false;
  }

  ignition::rendering::MaterialPtr green = this->scene->CreateMaterial();
  if (green == nullptr) {
    ignerr << "Failed to create green material" << std::endl;
    return false;
  }

  double x = 2.0;
  double y = 0.0;
  double z = 0.0;

  if (_link.visual(0).has_pose()) {
    // The default Ogre coordinate system is X left/right,
    // Y up/down, and Z in/out (of the screen).  However,
    // ignition-rendering switches that to be consistent with
    // Gazebo.  Thus, the coordinate system is X in/out, Y
    // left/right, and Z up/down.
    x += _link.visual(0).pose().position().z();
    y += -_link.visual(0).pose().position().x();
    z += _link.visual(0).pose().position().y();
  }

  double x_scale = 1.0;
  double y_scale = 1.0;
  double z_scale = 1.0;

  if (_link.visual(0).has_geometry()) {
    auto geom = _link.visual(0).geometry();
    if (geom.has_box()) {
      auto box = geom.box();
      if (box.has_size()) {
        x_scale = _link.visual(0).geometry().box().size().z();
        y_scale = _link.visual(0).geometry().box().size().x();
        z_scale = _link.visual(0).geometry().box().size().y();
      }
    }
  }

  green->SetAmbient(0.0, 0.5, 0.0);
  green->SetDiffuse(0.0, 0.7, 0.0);
  green->SetSpecular(0.5, 0.5, 0.5);
  green->SetShininess(50);
  green->SetReflectivity(0);
  box->AddGeometry(scene->CreateBox());
  box->SetLocalPosition(x, y, z);
  box->SetLocalScale(x_scale, y_scale, z_scale);
  box->SetMaterial(green);
  root->AddChild(box);

  return true;
}

bool RenderWidget::renderSphere(ignition::msgs::Link &_link)
{
  ignmsg << "Has a sphere!" << std::endl;
  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr sphere = this->scene->CreateVisual();
  if (sphere == nullptr) {
    ignerr << "Failed to create visual" << std::endl;
    return false;
  }
  ignition::rendering::MaterialPtr red = scene->CreateMaterial();
  if (red == nullptr) {
    ignerr << "Failed to create red material" << std::endl;
    return false;
  }

  double x = 2.0;
  double y = 0.0;
  double z = 0.0;

  if (_link.visual(0).has_pose()) {
    // The default Ogre coordinate system is X left/right,
    // Y up/down, and Z in/out (of the screen).  However,
    // ignition-rendering switches that to be consistent with
    // Gazebo.  Thus, the coordinate system is X in/out, Y
    // left/right, and Z up/down.
    x += _link.visual(0).pose().position().z();
    y += -_link.visual(0).pose().position().x();
    z += _link.visual(0).pose().position().y();
  }

  double x_scale = 1.0;
  double y_scale = 1.0;
  double z_scale = 1.0;

  if (_link.visual(0).has_geometry()) {
    auto geom = _link.visual(0).geometry();
    if (geom.has_sphere()) {
      auto sphere = geom.sphere();
      if (sphere.has_radius()) {
        ignmsg << "Setting radius to " << sphere.radius() << std::endl;
        x_scale *= sphere.radius();
        y_scale *= sphere.radius();
        z_scale *= sphere.radius();
      }
    }
  }

  red->SetAmbient(0.5, 0.0, 0.0);
  red->SetDiffuse(1.0, 0.0, 0.0);
  red->SetSpecular(0.5, 0.5, 0.5);
  red->SetShininess(50);
  red->SetReflectivity(0);
  sphere->AddGeometry(scene->CreateSphere());
  sphere->SetLocalPosition(x, y, z);
  sphere->SetLocalScale(x_scale, y_scale, z_scale);
  sphere->SetMaterial(red);
  root->AddChild(sphere);

  return true;
}

bool RenderWidget::renderCylinder(ignition::msgs::Link &_link)
{
  ignmsg << "Has a cylinder!" << std::endl;

  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr cylinder = this->scene->CreateVisual();
  if (cylinder == nullptr) {
    ignerr << "Failed to create visual" << std::endl;
    return false;
  }
  ignition::rendering::MaterialPtr blue = scene->CreateMaterial();
  if (blue == nullptr) {
    ignerr << "Failed to create blue material" << std::endl;
    return false;
  }

  double x = 2.0;
  double y = 0.0;
  double z = 0.0;

  if (_link.visual(0).has_pose()) {
    // The default Ogre coordinate system is X left/right,
    // Y up/down, and Z in/out (of the screen).  However,
    // ignition-rendering switches that to be consistent with
    // Gazebo.  Thus, the coordinate system is X in/out, Y
    // left/right, and Z up/down.
    x += _link.visual(0).pose().position().z();
    y += -_link.visual(0).pose().position().x();
    z += _link.visual(0).pose().position().y();
  }

  double x_scale = 1.0;
  double y_scale = 1.0;
  double z_scale = 1.0;

  if (_link.visual(0).has_geometry()) {
    auto geom = _link.visual(0).geometry();
    if (geom.has_cylinder()) {
      auto cylinder = geom.cylinder();
      if (cylinder.has_radius()) {
        ignmsg << "Setting radius to " << cylinder.radius() << std::endl;
        x_scale *= cylinder.radius();
        y_scale *= cylinder.radius();
        z_scale = cylinder.length();
      }
    }
  }

  blue->SetAmbient(0.0, 0.0, 0.3);
  blue->SetDiffuse(0.0, 0.0, 0.8);
  blue->SetSpecular(0.8, 0.8, 0.8);
  blue->SetShininess(50);
  blue->SetReflectivity(0);
  cylinder->AddGeometry(scene->CreateCylinder());
  cylinder->SetLocalPosition(x, y, z);
  cylinder->SetLocalScale(x_scale, y_scale, z_scale);
  cylinder->SetMaterial(blue);
  root->AddChild(cylinder);

  return true;
}

void RenderWidget::setInitialModel(const ignition::msgs::Model &_msg)
{
  ignmsg << "Saw new graphic!" << std::endl;

  if (this->initialized_scene) {
    return;
  }

  for (int i = 0; i < _msg.link_size(); i++) {
    auto link = _msg.link(i);
    ignmsg << "Rendering: " << link.name() << std::endl;

    if (link.visual(0).geometry().has_box()) {
      renderBox(link);
    }
    else if (link.visual(0).geometry().has_sphere()) {
      renderSphere(link);
    }
    else if (link.visual(0).geometry().has_cylinder()) {
      renderCylinder(link);
    }
  }

  this->initialized_scene = true;
}

void RenderWidget::updateScene(const ignition::msgs::PosesStamped &_msg)
{
  ignmsg << "Saw updateScene" << std::endl;

  for (int i = 0; i < _msg.pose_size(); i++) {
    auto pose = _msg.pose(i);
    if (pose.has_name()) {
      ignmsg << "Name is " << pose.name() << std::endl;
    }
  }
}

void RenderWidget::CreateRenderWindow()
{
  std::string engineName = "ogre";
  ignition::rendering::RenderEngine* engine =
      ignition::rendering::get_engine(engineName);
  if (!engine) {
    ignerr << "Engine '" << engineName << "' is not supported" << std::endl;
    return;
  }

  // Create sample scene
  this->scene = engine->CreateScene("scene");
  if (this->scene == nullptr) {
    ignerr << "Failed to create scene" << std::endl;
    return;
  }

  this->scene->SetAmbientLight(0.3, 0.3, 0.3);
  ignition::rendering::VisualPtr root = this->scene->RootVisual();
  if (root == nullptr) {
    ignerr << "Failed to find the root visual" << std::endl;
    return;
  }
  ignition::rendering::DirectionalLightPtr light0 =
      this->scene->CreateDirectionalLight();
  if (light0 == nullptr) {
    ignerr << "Failed to create a directional light" << std::endl;
    return;
  }
  light0->SetDirection(-0.5, -0.5, -1);
  light0->SetDiffuseColor(0.5, 0.5, 0.5);
  light0->SetSpecularColor(0.5, 0.5, 0.5);
  root->AddChild(light0);

  // create user camera
  this->camera = this->scene->CreateCamera("user_camera");
  if (this->camera == nullptr) {
    ignerr << "Failed to create camera" << std::endl;
    return;
  }
  this->camera->SetLocalPosition(0.0, 0.0, 0.0);
  this->camera->SetLocalRotation(0.0, 0.0, 0.0);
  this->camera->SetAspectRatio(1.333);
  this->camera->SetHFOV(M_PI / 2.0);
  root->AddChild(this->camera);

  // create render window
  std::string winHandle = std::to_string(static_cast<uint64_t>(this->winId()));
  this->renderWindow = this->camera->CreateRenderWindow();
  if (this->renderWindow == nullptr) {
    ignerr << "Failed to create camera render window" << std::endl;
    return;
  }
  this->renderWindow->SetHandle(winHandle);
  this->renderWindow->SetWidth(this->width());
  this->renderWindow->SetHeight(this->height());

  // render once to create the window.
  this->camera->Update();
}

/////////////////////////////////////////////////
void RenderWidget::showEvent(QShowEvent* _e) {
  QApplication::flush();

  if (!this->renderWindow) {
    this->CreateRenderWindow();
    this->updateTimer->start(this->kUpdateTimeFrequency);
  }

  QWidget::showEvent(_e);

  this->raise();
  this->setFocus();
}

/////////////////////////////////////////////////
QPaintEngine* RenderWidget::paintEngine() const { return nullptr; }

/////////////////////////////////////////////////
void RenderWidget::paintEvent(QPaintEvent* _e) {
  if (this->renderWindow && this->camera) {
    this->camera->Update();
  }

  _e->accept();
}

/////////////////////////////////////////////////
void RenderWidget::resizeEvent(QResizeEvent* _e) {
  if (!this->renderWindow) {
    return;
  }
  this->renderWindow->OnResize(_e->size().width(), _e->size().height());
}

/////////////////////////////////////////////////
void RenderWidget::moveEvent(QMoveEvent* _e) {
  QWidget::moveEvent(_e);

  if (!_e->isAccepted() || !this->renderWindow) {
    return;
  }
  this->renderWindow->OnMove();
}

}  // namespace gui
}  // namespace delphyne

IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::RenderWidget,
                                  ignition::gui::Plugin);
