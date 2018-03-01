// Copyright 2018 Open Source Robotics Foundation
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

#include "RenderMaliputWidget.hh"

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

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
RenderMaliputWidget::RenderMaliputWidget(QWidget* parent) :
    engine(nullptr) {
  this->setAttribute(Qt::WA_OpaquePaintEvent, true);
  this->setAttribute(Qt::WA_PaintOnScreen, true);
  this->setAttribute(Qt::WA_NoSystemBackground, true);

  this->setFocusPolicy(Qt::StrongFocus);
  this->setMouseTracking(true);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // The below block means that every time the updateTime expires, we do an
  // update on the widget. Later on, we call the start() method to start this
  // time at a fixed frequency.  Note that we do not start this timer until the
  // first time that showEvent() is called.
  this->updateTimer = new QTimer(this);
  QObject::connect(this->updateTimer, SIGNAL(timeout()), this, SLOT(update()));
}

/////////////////////////////////////////////////
RenderMaliputWidget::~RenderMaliputWidget() {
  if (this->engine != nullptr) {
    // TODO(clalancette): We need to call this->engine->Fini() to clean up
    // some engine resources, but this sometimes causes a hang on quit.
    // For right now, disable this, but we should debug this and re-enable this
    // cleanup.
    // this->engine->Fini();
  }
}

/////////////////////////////////////////////////
void RenderMaliputWidget::RenderGrid(
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
void RenderMaliputWidget::RenderGroundPlaneGrid() {
  auto gridMaterial = this->scene->CreateMaterial();
  if (gridMaterial) {
    // Light blue color.
    const double lightRed = 0.404;
    const double lightGreen = 0.623;
    const double lightBlue = 0.8;
    gridMaterial->SetAmbient(lightRed, lightGreen, lightBlue);
    gridMaterial->SetDiffuse(lightRed, lightGreen, lightBlue);
    gridMaterial->SetSpecular(lightRed, lightGreen, lightBlue);

    const unsigned int kCellCount = 50u;
    const double       kCellLength = 1;
    const unsigned int kVerticalCellCount = 0u;

    this->RenderGrid(kCellCount, kCellLength, kVerticalCellCount,
      gridMaterial, ignition::math::Pose3d::Zero);
  } else {
    ignerr << "Failed to create material for the grid" << std::endl;
  }
}

/////////////////////////////////////////////////
void RenderMaliputWidget::RenderOrigin() {
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
void RenderMaliputWidget::CreateRenderWindow() {
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
  const double lightRed = 0.88;
  const double lightGreen = 0.88;
  const double lightBlue = 0.95;
  this->scene->SetAmbientLight(lightRed, lightGreen, lightBlue);
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
  directionalLight->SetDiffuseColor(lightRed, lightGreen, lightBlue);
  directionalLight->SetSpecularColor(lightRed, lightGreen, lightBlue);
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

  this->scene->SetBackgroundColor(lightRed, lightGreen, lightBlue);

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

  // Render the grid over the ground plane.
  this->RenderGroundPlaneGrid();

  // Render the origin reference frame.
  this->RenderOrigin();

  this->CreateRoadRootVisual();

  this->orbitViewControl.reset(new OrbitViewControl(this->camera));
}

/////////////////////////////////////////////////
void RenderMaliputWidget::CreateRoadRootVisual() {
  this->rootVisual = this->scene->CreateVisual();
  // The visual's root pose is initialized to zero because maliput's roads don't
  // have a center.
  const ignition::math::Pose3d origin(0, 0, 0, 1, 0, 0, 0);
  this->rootVisual->SetLocalPose(origin);
  this->scene->RootVisual()->AddChild(this->rootVisual);
}

/////////////////////////////////////////////////
bool RenderMaliputWidget::FillMaterial(
  const drake::maliput::mesh::Material* _maliputMaterial,
  ignition::rendering::MaterialPtr& _ignitionMaterial) const {
  if (!_maliputMaterial) {
    return false;
  }

  _ignitionMaterial->SetDiffuse(_maliputMaterial->diffuse.X(),
    _maliputMaterial->diffuse.Y(), _maliputMaterial->diffuse.Z());
  _ignitionMaterial->SetAmbient(_maliputMaterial->ambient.X(),
    _maliputMaterial->ambient.Y(), _maliputMaterial->ambient.Z());
  _ignitionMaterial->SetSpecular(_maliputMaterial->specular.X(),
    _maliputMaterial->specular.Y(), _maliputMaterial->specular.Z());
  _ignitionMaterial->SetShininess(_maliputMaterial->shinines);
  _ignitionMaterial->SetTransparency(_maliputMaterial->transparency);

  return true;
}

/////////////////////////////////////////////////
void RenderMaliputWidget::CreateTransparentMaterial(
  ignition::rendering::MaterialPtr& _material) const {
  _material->SetDiffuse(0., 0., 0.);
  _material->SetAmbient(0., 0., 0.);
  _material->SetSpecular(0., 0., 0.);
  _material->SetShininess(0.);
  _material->SetTransparency(1.);
}

/////////////////////////////////////////////////
void RenderMaliputWidget::RenderRoadMeshes(
  const std::map<std::string, std::unique_ptr<MaliputMesh>>& _maliputMeshes) {
  for (const auto& it : _maliputMeshes) {
    // Creates a material for the visual.
    ignition::rendering::MaterialPtr material = this->scene->CreateMaterial();
    if (!material) {
      ignerr << "Failed to create material.\n";
      return;
    }
    // Checks if the mesh to be rendered already exists or not.
    const auto meshExists = this->meshes.find(it.first);
    // If the mesh is disabled, there is no mesh for it so it must be set to
    // transparent.
    if (it.second->state == MaliputMesh::State::kDisabled) {
      // If the mesh already exits, a new transparent material is set.
      if (meshExists != this->meshes.end()) {
        this->CreateTransparentMaterial(material);
        this->meshes[it.first]->SetMaterial(material);
      }
    } else {
      // If the mesh doesn't exist, it creates new one. Otherwise, just gathers
      // the pointer to set the correct material.
      ignition::rendering::VisualPtr visual;
      if (meshExists == this->meshes.end()) {
        visual = this->scene->CreateVisual();
        if (!visual) {
          ignerr << "Failed to create visual.\n";
          return;
        }
        // Adds the visual to the map for later reference.
        this->meshes[it.first] = visual;
        // Sets the pose of the mesh.
        visual->SetLocalPose(ignition::math::Pose3d(0, 0, 0, 1, 0, 0, 0));
        // Loads the mesh into the visual.
        ignition::rendering::MeshDescriptor descriptor(it.second->mesh.get());
        descriptor.Load();
        ignition::rendering::MeshPtr meshGeom =
          this->scene->CreateMesh(descriptor);
        visual->AddGeometry(meshGeom);
        // Adds the mesh to the parent root visual.
        this->rootVisual->AddChild(visual);
      } else {
        visual = this->meshes[it.first];
      }

      // Applies the correct material to the mesh.
      if (it.second->visualState == MaliputMesh::VisualState::kOff) {
        this->CreateTransparentMaterial(material);
      } else if (!this->FillMaterial(it.second->material.get(), material)) {
        ignerr << "Failed to fill " << it.first << " material information.\n";
        return;
      }
      visual->SetMaterial(material);
    }
  }
}

/////////////////////////////////////////////////
void RenderMaliputWidget::showEvent(QShowEvent* _e) {
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
QPaintEngine* RenderMaliputWidget::paintEngine() const { return nullptr; }

/////////////////////////////////////////////////
// Replace inherited implementation with a do-nothing one, so that the
// context menu doesn't appear and we get back the zoom in/out using the
// right mouse button.
void RenderMaliputWidget::ShowContextMenu(const QPoint &_pos) {}

/////////////////////////////////////////////////
void RenderMaliputWidget::paintEvent(QPaintEvent* _e) {
  if (this->renderWindow && this->camera) {
    this->camera->Update();
  }

  _e->accept();
}

/////////////////////////////////////////////////
void RenderMaliputWidget::resizeEvent(QResizeEvent* _e) {
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
void RenderMaliputWidget::moveEvent(QMoveEvent* _e) {
  QWidget::moveEvent(_e);

  if (!_e->isAccepted() || !this->renderWindow) {
    return;
  }
  this->renderWindow->OnMove();
}

/////////////////////////////////////////////////
void RenderMaliputWidget::mousePressEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMousePress(_e);
}

/////////////////////////////////////////////////
void RenderMaliputWidget::mouseReleaseEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseRelease(_e);
}

/////////////////////////////////////////////////
void RenderMaliputWidget::mouseMoveEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseMove(_e);
}

/////////////////////////////////////////////////
void RenderMaliputWidget::wheelEvent(QWheelEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseWheel(_e);
}
