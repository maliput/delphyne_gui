#include "bridge/lcm_to_ign_translation.hh"
#include <iostream>
#include <gtest/gtest.h>
#include <ignition/msgs.hh>

namespace delphyne {
namespace bridge {

// Test that an LCM geometry message describing a box is properly
// translated to an ignition Geometry message.
GTEST_TEST(BoxTest, TestBoxTranslation) {
  // Define LCM box geometry message
  drake::lcmt_viewer_geometry_data boxMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_box_geometry;
  // Fill LCM data
  boxMsg.type = boxMsg.BOX;
  boxMsg.num_float_data = 3;
  boxMsg.float_data.resize(boxMsg.num_float_data);
  boxMsg.float_data[0] = 3.888;
  boxMsg.float_data[1] = 0.753;
  boxMsg.float_data[2] = 0.031;

  // Translate from LCM to ignition
  translate(boxMsg, &ign_box_geometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ign_box_geometry.has_box());
  ASSERT_EQ(ignition::msgs::Geometry::BOX, ign_box_geometry.type());
  ASSERT_EQ(boxMsg.float_data[0], ign_box_geometry.box().size().x());
  ASSERT_EQ(boxMsg.float_data[1], ign_box_geometry.box().size().y());
  ASSERT_EQ(boxMsg.float_data[2], ign_box_geometry.box().size().z());
}

// Test that translation fails if an LCM geometry message describing a box is
// not properly filled.
GTEST_TEST(BoxTest, TestExceptionInBoxTranslation) {
  // Define LCM box geometry message
  drake::lcmt_viewer_geometry_data boxMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_box_geometry;
  // Fill LCM data, but with missing z information
  boxMsg.type = boxMsg.BOX;
  boxMsg.num_float_data = 2;
  boxMsg.float_data.resize(boxMsg.num_float_data);
  boxMsg.float_data[0] = 1;
  boxMsg.float_data[1] = 2;

  EXPECT_THROW(
      translate(boxMsg, &ign_box_geometry);,
      TranslateException);
}

// Test that an LCM geometry message describing a cylinder is properly
// translated to an ignition Geometry message.
GTEST_TEST(CylinderTest, TestCylinderTranslation) {
  // Define LCM cylinder geometry message
  drake::lcmt_viewer_geometry_data cylinderMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_cylinder_geometry;
  // Fill LCM data
  cylinderMsg.type = cylinderMsg.CYLINDER;
  cylinderMsg.num_float_data = 2;
  cylinderMsg.float_data.resize(cylinderMsg.num_float_data);
  cylinderMsg.float_data[0] = 1.368;  // radius
  cylinderMsg.float_data[1] = 4.246;  // length

  // Translate from LCM to ignition
  translate(cylinderMsg, &ign_cylinder_geometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ign_cylinder_geometry.has_cylinder());
  ASSERT_EQ(ignition::msgs::Geometry::CYLINDER, ign_cylinder_geometry.type());
  ASSERT_EQ(cylinderMsg.float_data[0],
            ign_cylinder_geometry.cylinder().radius());
  ASSERT_EQ(cylinderMsg.float_data[1],
            ign_cylinder_geometry.cylinder().length());
}

// Test that translation fails if an LCM geometry message describing a
// cylinder is not properly filled.
GTEST_TEST(CylinderTest, TestExceptionInCylinderTranslation) {
  // Define LCM cylinder geometry message
  drake::lcmt_viewer_geometry_data cylinderMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_cylinder_geometry;
  // Fill LCM data
  cylinderMsg.type = cylinderMsg.CYLINDER;
  cylinderMsg.num_float_data = 1;
  cylinderMsg.float_data.resize(cylinderMsg.num_float_data);
  cylinderMsg.float_data[0] = 1.368;  // radius

  EXPECT_THROW(
      translate(cylinderMsg, &ign_cylinder_geometry),
      TranslateException);
}

// Test that an LCM geometry message describing a sphere is properly
// translated to an ignition Geometry message.
GTEST_TEST(SphereTest, TestSphereTranslation) {
  // Define LCM sphere geometry message
  drake::lcmt_viewer_geometry_data sphereMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_sphere_geometry;
  // Fill LCM data
  sphereMsg.type = sphereMsg.SPHERE;
  sphereMsg.num_float_data = 1;
  sphereMsg.float_data.resize(sphereMsg.num_float_data);
  sphereMsg.float_data[0] = 2.534;  // radius

  // Translate from LCM to ignition
  translate(sphereMsg, &ign_sphere_geometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ign_sphere_geometry.has_sphere());
  ASSERT_EQ(ignition::msgs::Geometry::SPHERE, ign_sphere_geometry.type());
  ASSERT_EQ(sphereMsg.float_data[0], ign_sphere_geometry.sphere().radius());
}

// Test that translation fails if an LCM geometry message describing a
// sphere is not properly filled.
GTEST_TEST(SphereTest, TestExceptionInCylinderTranslation) {
  // Define LCM sphere geometry message
  drake::lcmt_viewer_geometry_data sphereMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_sphere_geometry;
  // Fill LCM data
  sphereMsg.type = sphereMsg.SPHERE;
  sphereMsg.num_float_data = 0;
  sphereMsg.float_data.resize(sphereMsg.num_float_data);

  EXPECT_THROW(
      translate(sphereMsg, &ign_sphere_geometry),
      TranslateException);
}

// Test that an LCM geometry message describing a mesh with scaling
// data is properly translated to an ignition Geometry message.
GTEST_TEST(MeshTest, TestMeshTranslationWithScale) {
  // Define LCM mesh geometry message
  drake::lcmt_viewer_geometry_data meshMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_mesh_geometry;
  // Fill LCM data
  meshMsg.type = meshMsg.MESH;
  meshMsg.string_data = "/path/to/mesh.obj";
  meshMsg.num_float_data = 3;
  meshMsg.float_data.resize(meshMsg.num_float_data);
  meshMsg.float_data[0] = 3.4;  // scale
  meshMsg.float_data[1] = 4.2;
  meshMsg.float_data[2] = 6.7;

  // Translate from LCM to ignition
  translate(meshMsg, &ign_mesh_geometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ign_mesh_geometry.has_mesh());
  ASSERT_EQ(ignition::msgs::Geometry::MESH, ign_mesh_geometry.type());
  ASSERT_EQ(meshMsg.string_data, ign_mesh_geometry.mesh().filename());
  ASSERT_TRUE(ign_mesh_geometry.mesh().has_scale());
  ASSERT_EQ(meshMsg.float_data[0], ign_mesh_geometry.mesh().scale().x());
  ASSERT_EQ(meshMsg.float_data[1], ign_mesh_geometry.mesh().scale().y());
  ASSERT_EQ(meshMsg.float_data[2], ign_mesh_geometry.mesh().scale().z());
}

// Test that an LCM geometry message describing a mesh without scaling
// data is properly translated to an ignition Geometry message.
GTEST_TEST(MeshTest, TestMeshTranslationWithoutScale) {
  // Define LCM mesh geometry message
  drake::lcmt_viewer_geometry_data meshMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ign_mesh_geometry;
  // Fill LCM data
  meshMsg.type = meshMsg.MESH;
  meshMsg.string_data = "/path/to/mesh.obj";
  meshMsg.num_float_data = 0;
  meshMsg.float_data.resize(meshMsg.num_float_data);

  // Translate from LCM to ignition
  translate(meshMsg, &ign_mesh_geometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ign_mesh_geometry.has_mesh());
  ASSERT_EQ(ignition::msgs::Geometry::MESH, ign_mesh_geometry.type());
  ASSERT_EQ(meshMsg.string_data, ign_mesh_geometry.mesh().filename());
  ASSERT_FALSE(ign_mesh_geometry.mesh().has_scale());
}

}  // namespace bridge
}  // namespace delphyne

