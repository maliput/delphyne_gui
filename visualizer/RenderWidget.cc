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

#include <functional>
#include <map>
#include <string>
#include <utility>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/gui/Iface.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/msgs.hh>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderTarget.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>

#include "RenderWidget.hh"

Q_DECLARE_METATYPE(ignition::msgs::Model)
Q_DECLARE_METATYPE(ignition::msgs::PosesStamped)

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
static void setLocalPositionFromPose(ignition::rendering::VisualPtr _shape,
  ignition::msgs::Visual &_vis)
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

/////////////////////////////////////////////////
RenderWidget::RenderWidget(QWidget *parent)
  : Plugin(), initializedScene(false)
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
  QObject::connect(this, SIGNAL(NewInitialModel(const ignition::msgs::Model &)),
    this, SLOT(SetInitialModel(const ignition::msgs::Model &)));
  QObject::connect(this, SIGNAL(NewDraw(const ignition::msgs::PosesStamped &)),
    this, SLOT(UpdateScene(const ignition::msgs::PosesStamped &)));

  this->node.Subscribe("/DRAKE_VIEWER_LOAD_ROBOT", &RenderWidget::LoadRobotCb,
    this);
  this->node.Subscribe("/DRAKE_VIEWER_DRAW", &RenderWidget::DrawCb, this);

  this->setMinimumHeight(100);
}

/////////////////////////////////////////////////
RenderWidget::~RenderWidget()
{
}

/////////////////////////////////////////////////
void RenderWidget::LoadRobotCb(const ignition::msgs::Model &_msg)
{
  ignmsg << "Saw new model msg on subscription" << std::endl;
  emit this->NewInitialModel(_msg);
}

/////////////////////////////////////////////////
void RenderWidget::DrawCb(const ignition::msgs::PosesStamped &_msg)
{
  ignmsg << "Saw new draw msg on subscription" << std::endl;
  emit this->NewDraw(_msg);
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderBox(
  ignition::msgs::Visual &_vis)
{
  ignmsg << "Has a box!" << std::endl;
  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr box = this->scene->CreateVisual();
  if (!box) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }

  ignition::rendering::MaterialPtr green = this->scene->CreateMaterial();
  if (!green) {
    ignerr << "Failed to create green material" << std::endl;
    return nullptr;
  }

  double xScale = 1.0;
  double yScale = 1.0;
  double zScale = 1.0;

  auto geomBox = _vis.geometry().box();
  if (geomBox.has_size()) {
    xScale = geomBox.size().z();
    yScale = geomBox.size().x();
    zScale = geomBox.size().y();
  }

  green->SetAmbient(0.0, 0.5, 0.0);
  green->SetDiffuse(0.0, 0.7, 0.0);
  green->SetSpecular(0.5, 0.5, 0.5);
  green->SetShininess(50);
  green->SetReflectivity(0);
  box->AddGeometry(scene->CreateBox());
  setLocalPositionFromPose(box, _vis);
  box->SetLocalScale(xScale, yScale, zScale);
  box->SetMaterial(green);
  root->AddChild(box);

  return box;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderSphere(
  ignition::msgs::Visual &_vis)
{
  ignmsg << "Has a sphere!" << std::endl;
  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr sphere = this->scene->CreateVisual();
  if (!sphere) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }
  ignition::rendering::MaterialPtr red = scene->CreateMaterial();
  if (!red) {
    ignerr << "Failed to create red material" << std::endl;
    return nullptr;
  }

  double xScale = 1.0;
  double yScale = 1.0;
  double zScale = 1.0;

  auto geomSphere = _vis.geometry().sphere();
  if (geomSphere.has_radius()) {
    xScale *= geomSphere.radius();
    yScale *= geomSphere.radius();
    zScale *= geomSphere.radius();
  }

  red->SetAmbient(0.5, 0.0, 0.0);
  red->SetDiffuse(1.0, 0.0, 0.0);
  red->SetSpecular(0.5, 0.5, 0.5);
  red->SetShininess(50);
  red->SetReflectivity(0);
  sphere->AddGeometry(scene->CreateSphere());
  setLocalPositionFromPose(sphere, _vis);
  sphere->SetLocalScale(xScale, yScale, zScale);
  sphere->SetMaterial(red);
  root->AddChild(sphere);

  return sphere;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderCylinder(
  ignition::msgs::Visual &_vis)
{
  ignmsg << "Has a cylinder!" << std::endl;

  ignition::rendering::VisualPtr root = this->scene->RootVisual();

  ignition::rendering::VisualPtr cylinder = this->scene->CreateVisual();
  if (!cylinder) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }
  ignition::rendering::MaterialPtr blue = scene->CreateMaterial();
  if (!blue) {
    ignerr << "Failed to create blue material" << std::endl;
    return nullptr;
  }

  double xScale = 1.0;
  double yScale = 1.0;
  double zScale = 1.0;

  auto geomCylinder = _vis.geometry().cylinder();
  if (geomCylinder.has_radius()) {
    xScale *= geomCylinder.radius();
    yScale *= geomCylinder.radius();
  }
  if (geomCylinder.has_length()) {
    zScale = geomCylinder.length();
  }

  blue->SetAmbient(0.0, 0.0, 0.3);
  blue->SetDiffuse(0.0, 0.0, 0.8);
  blue->SetSpecular(0.8, 0.8, 0.8);
  blue->SetShininess(50);
  blue->SetReflectivity(0);
  cylinder->AddGeometry(scene->CreateCylinder());
  setLocalPositionFromPose(cylinder, _vis);
  cylinder->SetLocalScale(xScale, yScale, zScale);
  cylinder->SetMaterial(blue);
  root->AddChild(cylinder);

  return cylinder;
}

/////////////////////////////////////////////////
void RenderWidget::SetInitialModel(const ignition::msgs::Model &_msg)
{
  if (this->initializedScene) {
    return;
  }

  for (int i = 0; i < _msg.link_size(); ++i) {
    auto link = _msg.link(i);

    if (!link.has_name()) {
      ignerr << "No name on link, skipping" << std::endl;
      continue;
    }
    if (!link.has_id()) {
      ignerr << "No robot number on link " << link.name() << ", skipping"
             << std::endl;
      continue;
    }
    if (link.visual_size() != 1) {
      ignerr << "Expected exactly 1 visual for " << link.name() << ", saw "
             << link.visual_size() << "; skipping" << std::endl;
      continue;
    }

    bool duplicate = false;
    auto range = robotToLink.equal_range(link.id());
    for (auto i = range.first; i != range.second; ++i) {
      if (i->second.first == link.name()) {
        ignerr << "Duplicate link " << link.name() << " for robot "
               << link.id() << ", skipping" << std::endl;
        duplicate = true;
        break;
      }
    }
    if (duplicate) {
      continue;
    }

    ignmsg << "Rendering: " << link.name() << " (" << link.id() << ")"
           << std::endl;

    auto vis = link.visual(0);
    if (!vis.has_geometry()) {
      ignerr << "No geometry in link " << link.name() << ", skipping"
             << std::endl;
      continue;
    }

    ignition::rendering::VisualPtr ignvis;

    if (vis.geometry().has_box()) {
      ignvis = this->RenderBox(vis);
    }
    else if (vis.geometry().has_sphere()) {
      ignvis = this->RenderSphere(vis);
    }
    else if (vis.geometry().has_cylinder()) {
      ignvis = this->RenderCylinder(vis);
    }
    else {
      ignerr << "Invalid shape for " << link.name() << ", skipping"
             << std::endl;
      continue;
    }

    auto nameAndVisual = std::pair<std::string,
      ignition::rendering::VisualPtr>(link.name(), ignvis);
    robotToLink.insert(std::pair<uint32_t, std::pair<std::string,
      ignition::rendering::VisualPtr>>(link.id(), nameAndVisual));
  }

  this->initializedScene = true;
}

/////////////////////////////////////////////////
void RenderWidget::UpdateScene(const ignition::msgs::PosesStamped &_msg)
{
  ignmsg << "Saw updateScene" << std::endl;

  for (int i = 0; i < _msg.pose_size(); ++i) {
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
      ignerr << "Could not find link " << pose.name() << " on robot "
             << pose.id() << ", skipping" << std::endl;
      continue;
    }

    // The setLocalPositionFromPose() assumes an ignition::msgs::Visual
    // message here, so we setup a dummy one to please it.
    ignition::msgs::Visual tmpvis;
    *tmpvis.mutable_pose() = pose;
    setLocalPositionFromPose(ignvis, tmpvis);
  }
}

/////////////////////////////////////////////////
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
  if (!this->scene) {
    ignerr << "Failed to create scene" << std::endl;
    return;
  }

  this->scene->SetAmbientLight(0.3, 0.3, 0.3);
  ignition::rendering::VisualPtr root = this->scene->RootVisual();
  if (!root) {
    ignerr << "Failed to find the root visual" << std::endl;
    return;
  }
  ignition::rendering::DirectionalLightPtr light0 =
      this->scene->CreateDirectionalLight();
  if (!light0) {
    ignerr << "Failed to create a directional light" << std::endl;
    return;
  }
  light0->SetDirection(-0.5, -0.5, -1);
  light0->SetDiffuseColor(0.5, 0.5, 0.5);
  light0->SetSpecularColor(0.5, 0.5, 0.5);
  root->AddChild(light0);

  // create user camera
  this->camera = this->scene->CreateCamera("user_camera");
  if (!this->camera) {
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
  if (!this->renderWindow) {
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
QPaintEngine* RenderWidget::paintEngine() const {
  return nullptr;
}

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

IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::RenderWidget,
                                  ignition::gui::Plugin)
