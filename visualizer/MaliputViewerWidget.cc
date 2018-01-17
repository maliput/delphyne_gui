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

#include <cstdlib>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <tinyxml2.h>

#include <ignition/common/Console.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/StringUtils.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/gui/Iface.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/math/Helpers.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/msgs.hh>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/Grid.hh>
#include <ignition/rendering/Light.hh>
#include <ignition/rendering/Material.hh>
#include <ignition/rendering/MeshDescriptor.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderEngineManager.hh>
#include <ignition/rendering/RenderTarget.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>

#include "MaliputViewerWidget.hh"

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
MaliputViewerWidget::MaliputViewerWidget(QWidget* parent)
    : Plugin(), engine(nullptr) {
  this->setAttribute(Qt::WA_OpaquePaintEvent, true);
  this->setAttribute(Qt::WA_PaintOnScreen, true);
  this->setAttribute(Qt::WA_NoSystemBackground, true);

  this->setFocusPolicy(Qt::StrongFocus);
  this->setMouseTracking(true);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->title = "MaliputViewerWidget";

  // The below block means that every time the updateTime expires, we do an
  // update on the widget. Later on, we call the start() method to start this
  // time at a fixed frequency.  Note that we do not start this timer until the
  // first time that showEvent() is called.
  this->updateTimer = new QTimer(this);
  QObject::connect(this->updateTimer, SIGNAL(timeout()), this, SLOT(update()));

  auto paths =
      ignition::common::SystemPaths::PathsFromEnv("DELPHYNE_PACKAGE_PATH");
  if (paths.empty()) {
    ignerr << "DELPHYNE_PACKAGE_PATH environment variable is not set"
           << std::endl;
  }
  std::copy(paths.begin(), paths.end(), std::back_inserter(this->packagePaths));

  this->setMinimumHeight(100);
}

/////////////////////////////////////////////////
MaliputViewerWidget::~MaliputViewerWidget() {
  if (this->engine != nullptr) {
    // TODO(clalancette): We need to call this->engine->Fini() to clean up
    // some engine resources, but this sometimes causes a hang on quit.
    // For right now, disable this, but we should debug this and re-enable this
    // cleanup.
    // this->engine->Fini();
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  tinyxml2::XMLPrinter printer;
  if (!_pluginElem->Accept(&printer)) {
    ignwarn << "There was an error parsing the plugin element for ["
            << this->title << "]." << std::endl;
    return;
  }

  // Load the user camera options.
  auto userCameraXML = _pluginElem->FirstChildElement("camera");
  while (userCameraXML) {
    std::string cameraName;
    if (userCameraXML->Attribute("name", "user_camera")) {
      auto poseXML = userCameraXML->FirstChildElement("pose");
      if (poseXML) {
        auto poseStr = poseXML->GetText();
        std::istringstream stream(poseStr);
        stream >> this->userSettings.userCameraPose;
      } else {
        ignerr << "Unable to parse <pose> element within <camera>" << std::endl;
      }
      break;
    }

    // Not a user camera, skip to the next camera.
    userCameraXML = userCameraXML->NextSiblingElement("camera");
  }
}

/////////////////////////////////////////////////
std::string MaliputViewerWidget::ConfigStr() const {

  tinyxml2::XMLElement* pluginXML;
  tinyxml2::XMLDocument xmlDoc;

  if (configStr.empty()) {
    // If we have no defined plugin configuration, create the XML doc with the
    // plugin element and initialize it with the basic properties.
    pluginXML = xmlDoc.NewElement("plugin");
    pluginXML->SetAttribute("filename", "MaliputViewerWidget");
    xmlDoc.InsertFirstChild(pluginXML);
  } else {
    // In case we do have an existing config, parse it.
    xmlDoc.Parse(configStr.c_str());
    pluginXML = xmlDoc.FirstChildElement("plugin");

    // If there is a camera element, remove it as we will be overriding it below
    auto cameraXML = pluginXML->FirstChildElement("camera");
    if (cameraXML) {
      pluginXML->DeleteChild(cameraXML);
    }
  }

  // User camera options.
  tinyxml2::XMLElement* userCameraXML = xmlDoc.NewElement("camera");
  userCameraXML->SetAttribute("name", "user_camera");
  pluginXML->InsertEndChild(userCameraXML);
  tinyxml2::XMLElement* poseXML = xmlDoc.NewElement("pose");
  auto pos = this->camera->LocalPose().Pos();
  auto rot = this->camera->LocalPose().Rot().Euler();
  std::stringstream stream;
  stream << pos.X() << " " << pos.Y() << " " << pos.Z() << " " << rot.X() << " "
         << rot.Y() << " " << rot.Z();
  poseXML->SetText(stream.str().c_str());
  userCameraXML->InsertEndChild(poseXML);

  tinyxml2::XMLPrinter printer;
  xmlDoc.Print(&printer);

  return printer.CStr();
}

/////////////////////////////////////////////////
void MaliputViewerWidget::RenderGroundPlane() {
  auto material = this->scene->CreateMaterial();
  if (!material) {
    ignerr << "Failed to create ground plane material" << std::endl;
    return;
  }

  material->SetShininess(50);
  material->SetReflectivity(0);

  auto groundPlaneVisual = this->scene->CreateVisual();
  if (!groundPlaneVisual) {
    ignerr << "Failed to create ground plane visual" << std::endl;
    return;
  }

  // 100 x 100 ground plane.
  groundPlaneVisual->SetLocalScale(100, 100, 1);
  groundPlaneVisual->AddGeometry(scene->CreatePlane());
  groundPlaneVisual->SetMaterial(material);
  this->scene->RootVisual()->AddChild(groundPlaneVisual);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::RenderGrid(
      const unsigned int _cellCount,
      const double _cellLength,
      const unsigned int _verticalCellCount,
      const ignition::rendering::MaterialPtr& _material,
      const ignition::math::Pose3d& _pose) {
  auto gridGeom = this->scene->CreateGrid();
  if (!gridGeom) {
    ignerr << "Unable to create grid geometry" << std::endl;
    return;
  }
  gridGeom->SetCellCount(_cellCount);
  gridGeom->SetCellLength(_cellLength);
  gridGeom->SetVerticalCellCount(_verticalCellCount);

  auto grid = this->scene->CreateVisual();
  if (!grid) {
    ignerr << "Unable to create grid visual" << std::endl;
    return;
  }

  grid->AddGeometry(gridGeom);
  grid->SetLocalPose(_pose);
  grid->SetMaterial(_material);
  this->scene->RootVisual()->AddChild(grid);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::RenderGroundPlaneGrid() {
  auto gray = this->scene->CreateMaterial();
  if (gray) {
    gray->SetAmbient(0.7, 0.7, 0.7);
    gray->SetDiffuse(0.7, 0.7, 0.7);
    gray->SetSpecular(0.7, 0.7, 0.7);

    const unsigned int kCellCount = 50u;
    const double       kCellLength = 1;
    const unsigned int kVerticalCellCount = 0u;

    this->RenderGrid(kCellCount, kCellLength, kVerticalCellCount,
      gray, ignition::math::Pose3d::Zero);
  } else {
    ignerr << "Failed to create material for the grid" << std::endl;
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::RenderOrigin() {
  const double kAxisRadius = 0.02;
  const double kAxisLength = 10000;
  const double kAxisHalfLength = kAxisLength / 2.0;

  // Create the visual axes.
  std::array<ignition::rendering::VisualPtr, 3> axes;
  for (auto& axis : axes) {
    axis = this->scene->CreateVisual();
    if (!axis) {
      ignerr << "Failed to create axis visual" << std::endl;
      return;
    }
    axis->SetLocalScale(kAxisRadius, kAxisRadius, kAxisLength);
    axis->AddGeometry(scene->CreateCylinder());
  }

  const ignition::math::Pose3d kAxisPoseX(kAxisHalfLength, 0, 0, 0, IGN_PI_2,
                                          0);
  axes[0]->SetLocalPose(kAxisPoseX);
  axes[0]->SetMaterial("Default/TransRed");
  const ignition::math::Pose3d kAxisPoseY(0, kAxisHalfLength, 0, IGN_PI_2, 0,
                                          0);
  axes[1]->SetLocalPose(kAxisPoseY);
  axes[1]->SetMaterial("Default/TransGreen");
  const ignition::math::Pose3d kAxisPoseZ(0, 0, kAxisHalfLength, 0, 0, 0);
  axes[2]->SetLocalPose(kAxisPoseZ);
  axes[2]->SetMaterial("Default/TransBlue");

  for (auto& axis : axes) {
    this->scene->RootVisual()->AddChild(axis);
  }
}

/////////////////////////////////////////////////
void MaliputViewerWidget::CreateRenderWindow() {
  std::string engineName = "ogre";
  ignition::rendering::RenderEngineManager* manager =
      ignition::rendering::RenderEngineManager::Instance();
  this->engine = manager->Engine(engineName);
  if (!this->engine) {
    ignerr << "Engine '" << engineName << "' is not supported" << std::endl;
    return;
  }

  // Create sample scene
  this->scene = engine->CreateScene("scene");
  if (!this->scene) {
    ignerr << "Failed to create scene" << std::endl;
    return;
  }

  // Lights.
  this->scene->SetAmbientLight(0.9, 0.9, 0.9);
  ignition::rendering::VisualPtr root = this->scene->RootVisual();
  if (!root) {
    ignerr << "Failed to find the root visual" << std::endl;
    return;
  }
  auto directionalLight = this->scene->CreateDirectionalLight();
  if (!directionalLight) {
    ignerr << "Failed to create a directional light" << std::endl;
    return;
  }
  directionalLight->SetDirection(-0.5, -0.5, -1);
  directionalLight->SetDiffuseColor(0.9, 0.9, 0.9);
  directionalLight->SetSpecularColor(0.9, 0.9, 0.9);
  root->AddChild(directionalLight);

  // create user camera
  this->camera = this->scene->CreateCamera("user_camera");
  if (!this->camera) {
    ignerr << "Failed to create camera" << std::endl;
    return;
  }
  // Rotate 135 deg on the Z axis
  auto rotation = this->userSettings.userCameraPose.Rot().Euler();
  this->camera->SetLocalRotation(rotation.X(), rotation.Y(), rotation.Z());
  // Step away from the center of the scene
  auto position = this->userSettings.userCameraPose.Pos();
  this->camera->SetLocalPosition(position.X(), position.Y(), position.Z());
  this->camera->SetAspectRatio(static_cast<double>(this->width()) /
                               this->height());
  this->camera->SetHFOV(IGN_DTOR(60));
  root->AddChild(this->camera);

  this->scene->SetBackgroundColor(0.9, 0.9, 0.9);

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

  // Render the ground plane.
  this->RenderGroundPlane();

  // Render the grid over the ground plane.
  this->RenderGroundPlaneGrid();

  // Render the origin reference frame.
  this->RenderOrigin();

  this->orbitViewControl.reset(new OrbitViewControl(this->camera));
}

/////////////////////////////////////////////////
void MaliputViewerWidget::showEvent(QShowEvent* _e) {
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
QPaintEngine* MaliputViewerWidget::paintEngine() const { return nullptr; }

/////////////////////////////////////////////////
// Replace inherited implementation with a do-nothing one, so that the
// context menu doesn't appear and we get back the zoom in/out using the
// right mouse button.
void MaliputViewerWidget::ShowContextMenu(const QPoint &_pos) {}

/////////////////////////////////////////////////
void MaliputViewerWidget::paintEvent(QPaintEvent* _e) {
  if (this->renderWindow && this->camera) {
    this->camera->Update();
  }

  _e->accept();
}

/////////////////////////////////////////////////
void MaliputViewerWidget::resizeEvent(QResizeEvent* _e) {
  if (!this->renderWindow) {
    return;
  }

  this->renderWindow->OnResize(_e->size().width(), _e->size().height());
  this->camera->SetAspectRatio(static_cast<double>(this->width()) /
                               this->height());
  // This is a bit janky. We need to update ign-rendering so that the
  // vertical FOV is auto updated when the aspect ratio is changed
  this->camera->SetHFOV(IGN_DTOR(60));
}

/////////////////////////////////////////////////
void MaliputViewerWidget::moveEvent(QMoveEvent* _e) {
  QWidget::moveEvent(_e);

  if (!_e->isAccepted() || !this->renderWindow) {
    return;
  }
  this->renderWindow->OnMove();
}

/////////////////////////////////////////////////
void MaliputViewerWidget::mousePressEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMousePress(_e);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::mouseReleaseEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseRelease(_e);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::mouseMoveEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseMove(_e);
}

/////////////////////////////////////////////////
void MaliputViewerWidget::wheelEvent(QWheelEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseWheel(_e);
}

IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::MaliputViewerWidget,
                                  ignition::gui::Plugin)
