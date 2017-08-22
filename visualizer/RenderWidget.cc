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
#include <map>
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

static void set_local_position_from_pose(ignition::rendering::VisualPtr _shape, ignition::msgs::Visual &_vis)
{
  double x = 2.0;
  double y = 0.0;
  double z = 0.0;

  if (_vis.has_pose()) {
    // The default Ogre coordinate system is X left/right,
    // Y up/down, and Z in/out (of the screen).  However,
    // ignition-rendering switches that to be consistent with
    // Gazebo.  Thus, the coordinate system is X in/out, Y
    // left/right, and Z up/down.
    x += _vis.pose().position().z();
    y += -_vis.pose().position().x();
    z += _vis.pose().position().y();
  }

  _shape->SetLocalPosition(x, y, z);
}

ignition::rendering::VisualPtr RenderWidget::renderBox(ignition::msgs::Visual &_vis)
{
  ignmsg << "Has a box!" << std::endl;
  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr box = this->scene->CreateVisual();
  if (box == nullptr) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }

  ignition::rendering::MaterialPtr green = this->scene->CreateMaterial();
  if (green == nullptr) {
    ignerr << "Failed to create green material" << std::endl;
    return nullptr;
  }

  double x_scale = 1.0;
  double y_scale = 1.0;
  double z_scale = 1.0;

  auto geom_box = _vis.geometry().box();
  if (geom_box.has_size()) {
    x_scale = geom_box.size().z();
    y_scale = geom_box.size().x();
    z_scale = geom_box.size().y();
  }

  green->SetAmbient(0.0, 0.5, 0.0);
  green->SetDiffuse(0.0, 0.7, 0.0);
  green->SetSpecular(0.5, 0.5, 0.5);
  green->SetShininess(50);
  green->SetReflectivity(0);
  box->AddGeometry(scene->CreateBox());
  set_local_position_from_pose(box, _vis);
  box->SetLocalScale(x_scale, y_scale, z_scale);
  box->SetMaterial(green);
  root->AddChild(box);

  return box;
}

ignition::rendering::VisualPtr RenderWidget::renderSphere(ignition::msgs::Visual &_vis)
{
  ignmsg << "Has a sphere!" << std::endl;
  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr sphere = this->scene->CreateVisual();
  if (sphere == nullptr) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }
  ignition::rendering::MaterialPtr red = scene->CreateMaterial();
  if (red == nullptr) {
    ignerr << "Failed to create red material" << std::endl;
    return nullptr;
  }

  double x_scale = 1.0;
  double y_scale = 1.0;
  double z_scale = 1.0;

  auto geom_sphere = _vis.geometry().sphere();
  if (geom_sphere.has_radius()) {
    x_scale *= geom_sphere.radius();
    y_scale *= geom_sphere.radius();
    z_scale *= geom_sphere.radius();
  }

  red->SetAmbient(0.5, 0.0, 0.0);
  red->SetDiffuse(1.0, 0.0, 0.0);
  red->SetSpecular(0.5, 0.5, 0.5);
  red->SetShininess(50);
  red->SetReflectivity(0);
  sphere->AddGeometry(scene->CreateSphere());
  set_local_position_from_pose(sphere, _vis);
  sphere->SetLocalScale(x_scale, y_scale, z_scale);
  sphere->SetMaterial(red);
  root->AddChild(sphere);

  return sphere;
}

ignition::rendering::VisualPtr RenderWidget::renderCylinder(ignition::msgs::Visual &_vis)
{
  ignmsg << "Has a cylinder!" << std::endl;

  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr cylinder = this->scene->CreateVisual();
  if (cylinder == nullptr) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }
  ignition::rendering::MaterialPtr blue = scene->CreateMaterial();
  if (blue == nullptr) {
    ignerr << "Failed to create blue material" << std::endl;
    return nullptr;
  }

  double x_scale = 1.0;
  double y_scale = 1.0;
  double z_scale = 1.0;

  auto geom_cylinder = _vis.geometry().cylinder();
  if (geom_cylinder.has_radius()) {
    x_scale *= geom_cylinder.radius();
    y_scale *= geom_cylinder.radius();
  }
  if (geom_cylinder.has_length()) {
    z_scale = geom_cylinder.length();
  }

  blue->SetAmbient(0.0, 0.0, 0.3);
  blue->SetDiffuse(0.0, 0.0, 0.8);
  blue->SetSpecular(0.8, 0.8, 0.8);
  blue->SetShininess(50);
  blue->SetReflectivity(0);
  cylinder->AddGeometry(scene->CreateCylinder());
  set_local_position_from_pose(cylinder, _vis);
  cylinder->SetLocalScale(x_scale, y_scale, z_scale);
  cylinder->SetMaterial(blue);
  root->AddChild(cylinder);

  return cylinder;
}

void RenderWidget::setInitialModel(const ignition::msgs::Model &_msg)
{
  if (this->initialized_scene) {
    return;
  }

  for (int i = 0; i < _msg.link_size(); i++) {
    auto link = _msg.link(i);

    if (!link.has_name()) {
      ignerr << "No name on link, skipping" << std::endl;
      continue;
    }
    if (!link.has_id()) {
      ignerr << "No robot number on link " << link.name() << ", skipping" << std::endl;
      continue;
    }
    if (link.visual_size() != 1) {
      ignerr << "Expected exactly 1 visual for " << link.name() << ", saw " << link.visual_size() << "; skipping" << std::endl;
      continue;
    }

    bool duplicate = false;
    auto range = robotToLink.equal_range(link.id());
    for (auto i = range.first; i != range.second; ++i) {
      if (i->second.first == link.name()) {
        ignerr << "Duplicate link " << link.name() << " for robot " << link.id() << ", skipping" << std::endl;
        duplicate = true;
        break;
      }
    }
    if (duplicate) {
      continue;
    }

    ignmsg << "Rendering: " << link.name() << " (" << link.id() << ")" << std::endl;

    auto vis = link.visual(0);
    if (!vis.has_geometry()) {
      ignerr << "No geometry in link " << link.name() << ", skipping" << std::endl;
      continue;
    }

    ignition::rendering::VisualPtr ignvis;

    if (vis.geometry().has_box()) {
      ignvis = renderBox(vis);
    }
    else if (vis.geometry().has_sphere()) {
      ignvis = renderSphere(vis);
    }
    else if (vis.geometry().has_cylinder()) {
      ignvis = renderCylinder(vis);
    }
    else {
      ignerr << "Invalid shape for " << link.name() << ", skipping" << std::endl;
      continue;
    }

    auto nameAndVisual = std::pair<std::string, ignition::rendering::VisualPtr>(link.name(), ignvis);
    robotToLink.insert(std::pair<uint32_t, std::pair<std::string, ignition::rendering::VisualPtr>>(link.id(), nameAndVisual));
  }

  this->initialized_scene = true;
}

void RenderWidget::updateScene(const ignition::msgs::PosesStamped &_msg)
{
  ignmsg << "Saw updateScene" << std::endl;

  for (int i = 0; i < _msg.pose_size(); i++) {
    auto pose = _msg.pose(i);
    if (!pose.has_name()) {
      ignerr << "Skipping pose without name" << std::endl;
      continue;
    }
    if (!pose.has_id()) {
      ignerr << "Skipping pose " << pose.name() << " without id" << std::endl;
      continue;
    }

    ignition::rendering::VisualPtr ignvis = nullptr;
    auto range = robotToLink.equal_range(pose.id());
    for (auto i = range.first; i != range.second; ++i) {
      if (i->second.first == pose.name()) {
        // found!
        ignmsg << "Found " << pose.id() << " " << pose.name() << std::endl;
        ignvis = i->second.second;
        break;
      }
    }

    if (ignvis == nullptr) {
      ignerr << "Could not find link " << pose.name() << " on robot " << pose.id() << ", skipping" << std::endl;
      continue;
    }

    // The set_local_position_from_pose() assumes an ignition::msgs::Visual message
    // here, so we setup a dummy one to please it.
    ignition::msgs::Visual tmpvis;
    *tmpvis.mutable_pose() = pose;
    set_local_position_from_pose(ignvis, tmpvis);
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
