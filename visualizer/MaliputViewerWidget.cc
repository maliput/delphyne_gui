// Copyright 2017 Toyota Research Institute

#include <cstdlib>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>

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

#include <drake/automotive/maliput/monolane/loader.h>

#include "GlobalAttributes.hh"
#include "MaliputMeshBuilder.hh"
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

  this->setMinimumHeight(100);

  // Loads the maliput file path if any and parses it.
  if (GlobalAttributes::HasArgument("yaml_file")) {
    this->userSettings.maliputFilePath =
      GlobalAttributes::GetArgument("yaml_file");
    ignmsg << "About to load [" << this->userSettings.maliputFilePath
      << "] monolane file." << std::endl;
    this->roadGeometry = drake::maliput::monolane::LoadFile(
      this->userSettings.maliputFilePath);
    ignmsg << "Loaded [" << this->userSettings.maliputFilePath
      << "] monolane file." << std::endl;
    ignmsg << "Loading RoadGeometry meshes..." << std::endl;
    std::map<std::string, drake::maliput::mesh::GeoMesh> geoMeshes =
      drake::maliput::mesh::BuildMeshes(this->roadGeometry.get(),
        drake::maliput::mesh::Features());
    ignmsg << "Meshes loaded." << std::endl;
    this->ConvertMeshes(geoMeshes);
    ignmsg << "Meshes converted to ignition type." << std::endl;
  }
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

  // Render road meshes.
  this->RenderRoadMeshes();

  this->orbitViewControl.reset(new OrbitViewControl(this->camera));
}

/////////////////////////////////////////////////
void MaliputViewerWidget::RenderRoadMeshes() {
  ignition::rendering::VisualPtr roadRootVisual = this->CreateRoadRootVisual();

  for (const auto& it : this->meshes) {
    ignition::rendering::VisualPtr visual = this->scene->CreateVisual();
    if (!visual) {
      ignerr << "Failed to create visual.\n";
      return;
    }
    ignition::rendering::MaterialPtr material = this->scene->CreateMaterial();
    if (!material) {
      ignerr << "Failed to create material.\n";
      return;
    }
    if (!this->FillMaterial(it.first, material)) {
      ignerr << "Failed to retrieve " << it.first << " information.\n";
      return;
    }

    // Loads the mesh into the visual.
    ignition::rendering::MeshDescriptor descriptor(it.second.get());
    descriptor.Load();
    ignition::rendering::MeshPtr meshGeom = this->scene->CreateMesh(descriptor);
    visual->AddGeometry(meshGeom);

    // Sets the pose of the mesh.
    visual->SetLocalPose(ignition::math::Pose3d(0, 0, 0, 1, 0, 0, 0));
    visual->SetMaterial(material);
    roadRootVisual->AddChild(visual);
  }
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr MaliputViewerWidget::CreateRoadRootVisual() {
  ignition::rendering::VisualPtr rootVisual = this->scene->CreateVisual();
  // The visual's root pose is initialized to zero because maliput's roads don't
  // have a center.
  const ignition::math::Pose3d origin(0, 0, 0, 1, 0, 0, 0);
  rootVisual->SetLocalPose(origin);
  this->scene->RootVisual()->AddChild(rootVisual);
  return rootVisual;
}

/////////////////////////////////////////////////
bool MaliputViewerWidget::FillMaterial(
  const std::string& materialName,
  ignition::rendering::MaterialPtr& material) const {

  std::unique_ptr<drake::maliput::mesh::Material> maliputMaterial =
    drake::maliput::mesh::GetMaterialByName(materialName);
  if (!maliputMaterial) {
    return false;
  }

  material->SetDiffuse(maliputMaterial->diffuse.X(),
    maliputMaterial->diffuse.Y(), maliputMaterial->diffuse.Z());
  material->SetAmbient(maliputMaterial->ambient.X(),
    maliputMaterial->ambient.Y(), maliputMaterial->ambient.Z());
  material->SetSpecular(maliputMaterial->specular.X(),
    maliputMaterial->specular.Y(), maliputMaterial->specular.Z());
  material->SetShininess(maliputMaterial->shinines);
  material->SetTransparency(maliputMaterial->transparency);

  return true;
}

/////////////////////////////////////////////////
void MaliputViewerWidget::ConvertMeshes(
  const std::map<std::string, drake::maliput::mesh::GeoMesh>& geoMeshes) {
  for (const auto& it : geoMeshes) {
    std::unique_ptr<ignition::common::Mesh> mesh =
      drake::maliput::mesh::Convert(it.first, it.second);
    if (mesh == nullptr) {
      ignmsg << "Skipping mesh [" << it.first << "] because it is empty.\n";
      continue;
    }
    this->meshes[it.first] = std::move(mesh);
  }
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
