// Copyright 2018 Toyota Research Institute

#include "arrow_mesh.hh"
#include <ignition/common/MeshManager.hh>
#include <ignition/math/Helpers.hh>
#include <ignition/rendering/Material.hh>
#include <ignition/rendering/Mesh.hh>
#include <ignition/rendering/Scene.hh>

namespace delphyne {
namespace gui {

ArrowMesh::ArrowMesh(ignition::rendering::ScenePtr& _scene, double _zOffset, double _scaleFactor)
    : zOffset(_zOffset),
      scaleFactor(_scaleFactor),
      distanceToMove(zOffset / 4.0),
      step(0.005),
      totalTicks(distanceToMove / step),
      currentTick(0),
      currentDirection(-1) {
  ignition::rendering::MaterialPtr material = _scene->CreateMaterial();
  material->SetDiffuse(255.0, 0.0, 0.0, 1.0);
  material->SetAmbient(255.0, 0.0, 0.0, 1.0);
  this->arrow = _scene->CreateVisual();
  this->arrow->AddGeometry(_scene->CreateCone());
  this->arrow->SetMaterial(material);
  this->arrow->SetVisible(false);
  this->arrow->SetWorldPosition(0., 0., 0.);
  this->arrow->SetWorldRotation(0, IGN_PI, 0.);
  _scene->RootVisual()->AddChild(this->arrow);
  this->CreateCylinder(_scene);
  ignition::common::MeshManager* meshManager = ignition::common::MeshManager::Instance();
  const ignition::common::Mesh* unitConeMesh = meshManager->MeshByName("unit_cone");
  minArrowBoundingBox = unitConeMesh->Min();
}

void ArrowMesh::SelectAt(double _distanceFromCamera, const ignition::math::Vector3d& _worldPosition) {
  const double scaleIncrement = 1.0 + scaleFactor * _distanceFromCamera;
  const double newMinArrowBBZAxis = scaleIncrement * minArrowBoundingBox.Z();
  this->arrow->SetWorldScale(scaleIncrement, scaleIncrement, scaleIncrement);
  this->arrow->SetWorldPosition(_worldPosition.X(), _worldPosition.Y(),
                                _worldPosition.Z() + std::abs(newMinArrowBBZAxis) + zOffset);
  this->cylinder->SetWorldPosition(_worldPosition.X(), _worldPosition.Y(),
                                   _worldPosition.Z() + std::abs(minCylinderBoundingBox.Z()));
  currentDirection = -1;
  currentTick = 0;
}

void ArrowMesh::SetVisibility(bool _visible) {
  this->arrow->SetVisible(_visible);
  this->cylinder->SetVisible(_visible);
}

void ArrowMesh::Update() {
  ignition::math::Vector3d worldPosition = this->arrow->WorldPosition();
  this->arrow->SetWorldPosition(worldPosition.X(), worldPosition.Y(), worldPosition.Z() + currentDirection * step);
  ++currentTick;
  if (currentTick == totalTicks) {
    currentDirection = currentDirection * -1;
    currentTick = 0;
  }
}

void ArrowMesh::CreateCylinder(ignition::rendering::ScenePtr& _scene) {
  ignition::rendering::MaterialPtr material = _scene->CreateMaterial();
  material->SetDiffuse(255.0, 0.0, 0.0, 1.0);
  material->SetAmbient(255.0, 0.0, 0.0, 1.0);
  this->cylinder = _scene->CreateVisual();
  this->cylinder->AddGeometry(_scene->CreateCylinder());
  this->cylinder->SetMaterial(material);
  this->cylinder->SetVisible(false);
  this->cylinder->SetWorldPosition(0., 0., 0.);
  // This size is enough for visualization.
  this->cylinder->SetWorldScale(0.5, 0.5, 0.2);
  _scene->RootVisual()->AddChild(this->cylinder);
  ignition::common::MeshManager* meshManager = ignition::common::MeshManager::Instance();
  const ignition::common::Mesh* unitCylinderMesh = meshManager->MeshByName("unit_cylinder");
  minCylinderBoundingBox = unitCylinderMesh->Min();
  minCylinderBoundingBox.X() = minCylinderBoundingBox.X() * 0.5;
  minCylinderBoundingBox.Y() = minCylinderBoundingBox.Y() * 0.5;
  minCylinderBoundingBox.Z() = minCylinderBoundingBox.Z() * 0.2;
}

}  // namespace gui
}  // namespace delphyne
