// Copyright 2017 Toyota Research Institute

#include <array>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <tinyxml2.h>

#include <delphyne/utility/package.h>

#include <ignition/common/Console.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/StringUtils.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/gui/Iface.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/math/Angle.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Helpers.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Vector4.hh>
#include <ignition/msgs.hh>
#include <ignition/msgs/Utility.hh>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/Light.hh>
#include <ignition/rendering/Material.hh>
#include <ignition/rendering/MeshDescriptor.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderEngineManager.hh>
#include <ignition/rendering/RenderTarget.hh>
#include <ignition/rendering/RenderTypes.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>

#include "render_widget.hh"

Q_DECLARE_METATYPE(ignition::msgs::Model_V)
Q_DECLARE_METATYPE(ignition::msgs::Scene)

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
static void setVisualPose(const ignition::msgs::Pose& _pose, ignition::rendering::VisualPtr _shape) {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double qw = 1.0;
  double qx = 0.0;
  double qy = 0.0;
  double qz = 0.0;

  if (_pose.has_position()) {
    x += _pose.position().x();
    y += _pose.position().y();
    z += _pose.position().z();
  }
  if (_pose.has_orientation()) {
    qw = _pose.orientation().w();
    qx = _pose.orientation().x();
    qy = _pose.orientation().y();
    qz = _pose.orientation().z();
  }

  ignition::math::Pose3d newpose(x, y, z, qw, qx, qy, qz);

  _shape->SetLocalPose(newpose);
}

/////////////////////////////////////////////////
static void setPoseFromMessage(const ignition::msgs::Visual& _vis, ignition::rendering::VisualPtr _shape) {
  if (_vis.has_pose()) {
    setVisualPose(_vis.pose(), _shape);
  }
}

/////////////////////////////////////////////////
static void setPoseFromMessage(const ignition::msgs::Link& _link, ignition::rendering::VisualPtr _shape) {
  if (_link.has_pose()) {
    setVisualPose(_link.pose(), _shape);
  }
}

/////////////////////////////////////////////////
RenderWidget::RenderWidget(QWidget*) : Plugin(), initializedScene(false), engine(nullptr) {
  qRegisterMetaType<ignition::msgs::Scene>();
  qRegisterMetaType<ignition::msgs::Model_V>();

  this->setAttribute(Qt::WA_OpaquePaintEvent, true);
  this->setAttribute(Qt::WA_PaintOnScreen, true);
  this->setAttribute(Qt::WA_NoSystemBackground, true);

  this->setFocusPolicy(Qt::StrongFocus);
  this->setMouseTracking(true);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->title = "RenderWidget";

  this->CreateRenderWindow();

  // The below block means that every time the updateTime expires, we do an
  // update on the widget. Later on, we call the start() method to start this
  // time at a fixed frequency.  Note that we do not start this timer until the
  // first time that showEvent() is called.
  this->updateTimer = new QTimer(this);
  QObject::connect(this->updateTimer, SIGNAL(timeout()), this, SLOT(update()));
  this->updateTimer->start(this->kUpdateTimeFrequency);

  QObject::connect(this, SIGNAL(NewInitialScene(const ignition::msgs::Scene&)), this,
                   SLOT(SetInitialScene(const ignition::msgs::Scene&)));
  QObject::connect(this, SIGNAL(NewDraw(const ignition::msgs::Model_V&)), this,
                   SLOT(UpdateScene(const ignition::msgs::Model_V&)));

  // Setting up a unique-named service name
  // i.e: Scene_8493201843;
  int randomId = ignition::math::Rand::IntUniform(1, ignition::math::MAX_I32);
  std::string sceneServiceName = "Scene_" + std::to_string(randomId);
  sceneRequestMsg.set_response_topic(sceneServiceName);

  // Advertise the service with the unique name generated above
  if (!node.Advertise(sceneServiceName, &RenderWidget::OnSetScene, this)) {
    ignerr << "Error advertising service [" << sceneServiceName << "]" << std::endl;
  }

  ignition::msgs::Boolean response;
  unsigned int timeout = 100;
  bool result;

  // Request a scene to be published into the unique-named channel
  this->node.Request("/get_scene", sceneRequestMsg, timeout, response, result);
}

/////////////////////////////////////////////////
RenderWidget::~RenderWidget() {
  if (this->engine != nullptr) {
    // TODO(clalancette): We need to call this->engine->Fini() to clean up
    // some engine resources, but this sometimes causes a hang on quit.
    // For right now, disable this, but we should debug this and re-enable this
    // cleanup.
    // this->engine->Fini();
    this->orbitViewControl.reset();
    this->camera->RemoveChildren();
    this->camera.reset();
    this->mainDirectionalLight.reset();
    this->scene->DestroyNodes();
    this->scene->DestroySensors();
    this->scene.reset();
  }
}

/////////////////////////////////////////////////
void RenderWidget::LoadConfig(const tinyxml2::XMLElement* _pluginElem) {
  tinyxml2::XMLPrinter printer;
  if (!_pluginElem->Accept(&printer)) {
    ignwarn << "There was an error parsing the plugin element for [" << this->title << "]." << std::endl;
    return;
  }

  // Configure if we need to cast shadows.
  bool castShadows = kCastShadowsByDefault;

  if (auto castShadowsElem = _pluginElem->FirstChildElement("cast_shadows")) {
    castShadowsElem->QueryBoolText(&castShadows);
  }

  this->mainDirectionalLight->SetCastShadows(castShadows);

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

        if (this->camera) {
          auto position = this->userSettings.userCameraPose.Pos();
          auto rotation = this->userSettings.userCameraPose.Rot().Euler();
          this->camera->SetLocalRotation(rotation.X(), rotation.Y(), rotation.Z());
          this->camera->SetLocalPosition(position.X(), position.Y(), position.Z());
        }
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
std::string RenderWidget::ConfigStr() const {
  tinyxml2::XMLElement* pluginXML;
  tinyxml2::XMLDocument xmlDoc;

  if (configStr.empty()) {
    // If we have no defined plugin configuration, create the XML doc with the
    // plugin element and initialize it with the basic properties.
    pluginXML = xmlDoc.NewElement("plugin");
    pluginXML->SetAttribute("filename", "RenderWidget");
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
  stream << pos.X() << " " << pos.Y() << " " << pos.Z() << " " << rot.X() << " " << rot.Y() << " " << rot.Z();
  poseXML->SetText(stream.str().c_str());
  userCameraXML->InsertEndChild(poseXML);

  tinyxml2::XMLPrinter printer;
  xmlDoc.Print(&printer);

  return printer.CStr();
}

/////////////////////////////////////////////////
void RenderWidget::OnSetScene(const ignition::msgs::Scene& request) { emit this->NewInitialScene(request); }

/////////////////////////////////////////////////
void RenderWidget::OnUpdateScene(const ignition::msgs::Model_V& _msg) { emit this->NewDraw(_msg); }

/////////////////////////////////////////////////
bool RenderWidget::CreateVisual(const ignition::msgs::Visual& _vis, ignition::rendering::VisualPtr& _visual,
                                ignition::rendering::MaterialPtr& _material) const {
  _visual = this->scene->CreateVisual();
  if (!_visual) {
    ignerr << "Failed to create visual" << std::endl;
    return false;
  }

  _material = this->scene->CreateMaterial();
  if (!_material) {
    ignerr << "Failed to create material" << std::endl;
    return false;
  }

  if (_vis.has_material()) {
    const auto& material = _vis.material();
    if (material.has_diffuse()) {
      const auto& diffuse = material.diffuse();
      _material->SetDiffuse(diffuse.r(), diffuse.g(), diffuse.b());

      const auto& ambient = material.ambient();
      _material->SetAmbient(ambient.r(), ambient.g(), ambient.b());

      const auto& specular = material.specular();
      _material->SetSpecular(specular.r(), specular.g(), specular.b());
    }
  }

  _material->SetTransparency(_vis.transparency());

  _material->SetShininess(50);
  _material->SetReflectivity(0);

  return true;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::Render(const ignition::msgs::Visual& _vis,
                                                    const ignition::math::Vector3d& _scale,
                                                    const ignition::rendering::MaterialPtr& _material,
                                                    ignition::rendering::VisualPtr& _visual) {
  setPoseFromMessage(_vis, _visual);
  _visual->SetLocalScale(_scale.X(), _scale.Y(), _scale.Z());
  _visual->SetMaterial(_material);
  return _visual;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderBox(const ignition::msgs::Visual& _vis,
                                                       ignition::rendering::VisualPtr& _visual,
                                                       ignition::rendering::MaterialPtr& _material) {
  ignition::math::Vector3d scale = ignition::math::Vector3d::One;
  auto geomBox = _vis.geometry().box();
  if (geomBox.has_size()) {
    scale.X() = geomBox.size().x();
    scale.Y() = geomBox.size().y();
    scale.Z() = geomBox.size().z();
  }

  _visual->AddGeometry(this->scene->CreateBox());
  this->Render(_vis, scale, _material, _visual);
  return _visual;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderSphere(const ignition::msgs::Visual& _vis,
                                                          ignition::rendering::VisualPtr& _visual,
                                                          ignition::rendering::MaterialPtr& _material) {
  ignition::math::Vector3d scale = ignition::math::Vector3d::One;
  auto geomSphere = _vis.geometry().sphere();
  scale.X() *= geomSphere.radius();
  scale.Y() *= geomSphere.radius();
  scale.Z() *= geomSphere.radius();

  _visual->AddGeometry(this->scene->CreateSphere());
  this->Render(_vis, scale, _material, _visual);
  return _visual;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderCylinder(const ignition::msgs::Visual& _vis,
                                                            ignition::rendering::VisualPtr& _visual,
                                                            ignition::rendering::MaterialPtr& _material) {
  ignition::math::Vector3d scale = ignition::math::Vector3d::One;
  auto geomCylinder = _vis.geometry().cylinder();
  scale.X() *= 2 * geomCylinder.radius();
  scale.Y() *= 2 * geomCylinder.radius();
  scale.Z() = geomCylinder.length();

  _visual->AddGeometry(this->scene->CreateCylinder());
  this->Render(_vis, scale, _material, _visual);
  return _visual;
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::RenderMesh(const ignition::msgs::Visual& _vis) {
  // Sanity check: Make sure that the message contains all required fields.
  if (!_vis.has_geometry()) {
    ignerr << "Unable to find geometry in message" << std::endl;
  }

  if (!_vis.geometry().has_mesh()) {
    ignerr << "Unable to find mesh in message" << std::endl;
  }

  if (_vis.geometry().mesh().filename().empty()) {
    ignerr << "Unable to find filename in message" << std::endl;
  }

  ignition::rendering::VisualPtr mesh = this->scene->CreateVisual();
  if (!mesh) {
    ignerr << "Failed to create visual" << std::endl;
    return nullptr;
  }

  auto filename = _vis.geometry().mesh().filename();
  delphyne::utility::PackageManager* package_manager = delphyne::utility::PackageManager::Instance();
  const utility::Package& package_in_use = package_manager->package_in_use();
  ignition::rendering::MeshDescriptor descriptor;
  ignition::common::URI mesh_uri = package_in_use.Resolve(filename);
  if (!mesh_uri.Valid()) {
    ignerr << "Unable to locate mesh [" << filename << "]" << std::endl;
    return nullptr;
  }
  descriptor.meshName = "/" + mesh_uri.Path().Str();
  ignition::common::MeshManager* meshManager = ignition::common::MeshManager::Instance();
  descriptor.mesh = meshManager->Load(descriptor.meshName);
  if (!descriptor.mesh) {
    return nullptr;
  }

  ignition::rendering::MeshPtr meshGeom = this->scene->CreateMesh(descriptor);
  mesh->AddGeometry(meshGeom);
  ignition::math::Vector3d center;
  ignition::math::Vector3d minXYZ;
  ignition::math::Vector3d maxXYZ;
  descriptor.mesh->AABB(center, minXYZ, maxXYZ);
  /* Position from messages have the world coordinates of the mesh. */
  minBBScene = ignition::math::Vector3d(std::min(minXYZ.X() + _vis.pose().position().x(), minBBScene.X()),
                                        std::min(minXYZ.Y() + _vis.pose().position().y(), minBBScene.Y()),
                                        std::min(minXYZ.Z() + _vis.pose().position().z(), minBBScene.Z()));
  maxBBScene = ignition::math::Vector3d(std::max(maxXYZ.X() + _vis.pose().position().x(), maxBBScene.X()),
                                        std::max(maxXYZ.Y() + _vis.pose().position().y(), maxBBScene.Y()),
                                        std::max(maxXYZ.Z() + _vis.pose().position().z(), maxBBScene.Z()));

  if (mesh_uri.Query().Str().find("culling=off") != std::string::npos) {
    DELPHYNE_VALIDATE(mesh->GeometryCount() == 1, std::runtime_error, "Expected one geometry.");
    auto subMesh = mesh->GeometryByIndex(0);
    /* TODO: Enable once ignition-rendering2 supports culling
    subMesh->Material()->SetCulling(ignition::rendering::CullMode::CM_NONE);*/
  }

  setPoseFromMessage(_vis, mesh);

  return mesh;
}

/////////////////////////////////////////////////
void RenderWidget::SetInitialScene(const ignition::msgs::Scene& _msg) {
  if (this->initializedScene) {
    return;
  }

  for (int i = 0; i < _msg.model_size(); ++i) {
    LoadModel(_msg.model(i));
  }

  // clang-format off
  ignition::math::Vector3d center((minBBScene.X() + maxBBScene.X()) / 2.0,
                                  (minBBScene.Y() + maxBBScene.Y()) / 2.0,
                                  (minBBScene.Z() + maxBBScene.Z()) / 2.0);
  // clang-format on

  const double sphereRadius = center.Distance(minBBScene);
  const double fov = this->camera->HFOV().Radian();
  // Angle from the origin of the sphere.
  constexpr double kInclinationAngle = IGN_PI / 180.0 * 60.0;
  // Get camera distance required for a sphere circumscribing the scene
  // bounding box to fit within the camera horizontal FOV.
  const double distance = sphereRadius / sin(fov / 2.0);
  const double azimuthAngle = atan((maxBBScene.X() - minBBScene.X()) / (maxBBScene.Y() - minBBScene.Y()));
  this->camera->SetWorldPosition(
      ignition::math::Vector3d(center.X() + distance * sin(kInclinationAngle) * cos(azimuthAngle),
                               center.Y() + distance * sin(kInclinationAngle) * sin(azimuthAngle),
                               center.Z() + distance * cos(kInclinationAngle)));
  this->camera->SetWorldRotation(ignition::math::Matrix4d::LookAt(this->camera->WorldPosition(), center).Pose().Rot());

  this->node.Subscribe("visualizer/scene_update", &RenderWidget::OnUpdateScene, this);
}

/////////////////////////////////////////////////
ignition::rendering::VisualPtr RenderWidget::CreateLinkRootVisual(ignition::msgs::Link& _link, uint32_t _robotID) {
  ignition::rendering::VisualPtr linkRootVisual = this->scene->CreateVisual();
  // The visual's root pose is initialized to zero because the load message
  // from LCM doesn't include the link pose but only the poses of the inner
  // visuals.
  // This value will be updated on each new draw message received.
  ignition::math::Pose3d newpose(0, 0, 0, 1, 0, 0, 0);
  linkRootVisual->SetLocalPose(newpose);
  this->scene->RootVisual()->AddChild(linkRootVisual);

  // Assign the visual created above as the root visual of the link
  auto& links = this->allVisuals[_robotID];
  links.insert(std::make_pair(_link.name(), linkRootVisual));
  return linkRootVisual;
}

/////////////////////////////////////////////////
void RenderWidget::LoadModel(const ignition::msgs::Model& _msg) {
  for (int i = 0; i < _msg.link_size(); ++i) {
    auto link = _msg.link(i);

    // Sanity check: Verify that the link contains the required name.
    if (link.name().empty()) {
      ignerr << "No name on link, skipping" << std::endl;
      continue;
    }

    // Sanity check: Verify that the visual doesn't exist already.
    const auto& modelIt = this->allVisuals.find(_msg.id());
    if (modelIt != this->allVisuals.end()) {
      if (modelIt->second.find(link.name()) != modelIt->second.end()) {
        ignerr << "Duplicated link [" << link.name() << "] for model " << _msg.id() << ". Skipping" << std::endl;
        continue;
      }
    }

    if (link.visual_size() == 0) {
      continue;
    }

    igndbg << "Rendering: [" << link.name() << "] (" << _msg.id() << ")" << std::endl;

    // Create a single root visual per link which will serve only
    // for hierarchical purposes, so that it can become the parent
    // of each of the "real" visuals of the link.
    ignition::rendering::VisualPtr linkRootVisual = RenderWidget::CreateLinkRootVisual(link, _msg.id());

    // Iterate through all the visuals of the link and add
    // them as childs of the link's root visual
    for (int j = 0; j < link.visual_size(); ++j) {
      const auto& vis = link.visual(j);
      if (!vis.has_geometry()) {
        ignerr << "No geometry in link [" << link.name() << "][" << j << "]. Skipping" << std::endl;
        continue;
      }

      ignition::rendering::VisualPtr visual;
      ignition::rendering::MaterialPtr material;
      if (!this->CreateVisual(vis, visual, material)) {
        continue;
      }

      // A real visual that is going to become
      // one of the link's root-visual childs
      ignition::rendering::VisualPtr ignvis;

      if (vis.geometry().has_box()) {
        ignvis = this->RenderBox(vis, visual, material);
      } else if (vis.geometry().has_sphere()) {
        ignvis = this->RenderSphere(vis, visual, material);
      } else if (vis.geometry().has_cylinder()) {
        ignvis = this->RenderCylinder(vis, visual, material);
      } else if (vis.geometry().has_mesh()) {
        ignvis = this->RenderMesh(vis);
      } else {
        ignerr << "Invalid shape for [" << link.name() << "]. Skipping" << std::endl;
        continue;
      }

      linkRootVisual->AddChild(ignvis);

      setPoseFromMessage(link, linkRootVisual);
    }
  }

  this->initializedScene = true;
}

/////////////////////////////////////////////////
void RenderWidget::UpdateScene(const ignition::msgs::Model_V& _msg) {
  for (int j = 0; j < _msg.models_size(); ++j) {
    auto model = _msg.models(j);

    for (int i = 0; i < model.link_size(); ++i) {
      auto link = model.link(i);

      // Sanity check: It's required to have a link name.
      if (link.name().empty()) {
        ignerr << "Skipping link without name" << std::endl;
        continue;
      }

      // Sanity check: Make sure that the model Id exists.
      auto robotIt = this->allVisuals.find(model.id());
      if (robotIt == this->allVisuals.end()) {
        ignerr << "Could not find model Id [" << model.id() << "]. Skipping" << std::endl;
        continue;
      }

      // Sanity check: Make sure that the link name exists.
      auto visualsIt = robotIt->second.find(link.name());
      if (visualsIt == robotIt->second.end()) {
        ignerr << "Could not find link name [" << link.name() << "]. Skipping" << std::endl;
        continue;
      }
      // Update the pose of the root visual only;
      // the relative poses of the children remain the same
      auto& visual = visualsIt->second;

      setPoseFromMessage(link, visual);
    }
  }
}

/////////////////////////////////////////////////
void RenderWidget::CreateRenderWindow() {
  std::string engineName = "ogre";
  ignition::rendering::RenderEngineManager* manager = ignition::rendering::RenderEngineManager::Instance();
  this->engine = manager->Engine(engineName);
  if (!this->engine) {
    ignerr << "Engine '" << engineName << "' is not supported" << std::endl;
    return;
  }

  // Try to reutilize the scene if exists
  this->scene = engine->SceneByName("scene");
  if (!this->scene) {
    // Create a scene
    this->scene = engine->CreateScene("scene");
    if (!this->scene) {
      ignerr << "Failed to create scene" << std::endl;
      return;
    }
  }

  // Lights.
  this->scene->SetAmbientLight(0.9, 0.9, 0.9);
  ignition::rendering::VisualPtr root = this->scene->RootVisual();
  if (!root) {
    ignerr << "Failed to find the root visual" << std::endl;
    return;
  }
  this->mainDirectionalLight = this->scene->CreateDirectionalLight();
  if (!this->mainDirectionalLight) {
    ignerr << "Failed to create a directional light" << std::endl;
    return;
  }
  this->mainDirectionalLight->SetDirection(-0.5, -0.5, -1);
  this->mainDirectionalLight->SetDiffuseColor(0.9, 0.9, 0.9);
  this->mainDirectionalLight->SetSpecularColor(0.9, 0.9, 0.9);
  this->mainDirectionalLight->SetCastShadows(kCastShadowsByDefault);
  root->AddChild(this->mainDirectionalLight);

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
  this->camera->SetAspectRatio(static_cast<double>(this->width()) / this->height());
  this->camera->SetHFOV(IGN_DTOR(60));
  root->AddChild(this->camera);

  // Set a gradient background color from white (top) to black (bottom)
  std::array<ignition::math::Color, 4> gradientBackgroundColor;
  gradientBackgroundColor[0].Set(1.0, 1.0, 1.0);
  gradientBackgroundColor[1].Set(0.0, 0.0, 0.0);
  gradientBackgroundColor[2].Set(1.0, 1.0, 1.0);
  gradientBackgroundColor[3].Set(0.0, 0.0, 0.0);
  this->scene->SetGradientBackgroundColor(gradientBackgroundColor);

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

  this->orbitViewControl.reset(new OrbitViewControl(this->camera));
}

/////////////////////////////////////////////////
void RenderWidget::showEvent(QShowEvent* _e) {
  QApplication::flush();

  QWidget::showEvent(_e);

  this->raise();
  this->setFocus();
}

/////////////////////////////////////////////////
QPaintEngine* RenderWidget::paintEngine() const { return nullptr; }

/////////////////////////////////////////////////
// Replace inherited implementation with a do-nothing one, so that the
// context menu doesn't appear and we get back the zoom in/out using the
// right mouse button.
void RenderWidget::ShowContextMenu(const QPoint&) {}

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
  this->camera->SetAspectRatio(static_cast<double>(this->width()) / this->height());
  // This is a bit janky. We need to update ign-rendering so that the
  // vertical FOV is auto updated when the aspect ratio is changed
  this->camera->SetHFOV(IGN_DTOR(60));
}

/////////////////////////////////////////////////
void RenderWidget::moveEvent(QMoveEvent* _e) {
  QWidget::moveEvent(_e);

  if (!_e->isAccepted() || !this->renderWindow) {
    return;
  }
  this->renderWindow->OnMove();
}

/////////////////////////////////////////////////
void RenderWidget::mousePressEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMousePress(_e);
}

/////////////////////////////////////////////////
void RenderWidget::mouseReleaseEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseRelease(_e);
}

/////////////////////////////////////////////////
void RenderWidget::mouseMoveEvent(QMouseEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseMove(_e);
}

/////////////////////////////////////////////////
void RenderWidget::wheelEvent(QWheelEvent* _e) {
  if (!this->orbitViewControl) {
    return;
  }

  this->orbitViewControl->OnMouseWheel(_e);
}

IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::RenderWidget, ignition::gui::Plugin)
