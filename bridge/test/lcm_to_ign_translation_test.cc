#include "bridge/lcm_to_ign_translation.hh"
#include <iostream>
#include <gtest/gtest.h>
#include <ignition/msgs.hh>

namespace delphyne {
namespace bridge {

//////////////////////////////////////////////////
/// \brief Test that an LCM draw message is properly
/// translated to an ignition Geometry message.
class ViewerDrawTest : public ::testing::Test {
 protected:
  drake::lcmt_viewer_draw drawMsg;
  ignition::msgs::PosesStamped ignPosesStamped;
  int lastIndex;

  virtual void SetUp() override {
    drawMsg.timestamp = 123456;
    drawMsg.num_links = 2;
    drawMsg.link_name.resize(drawMsg.num_links);
    drawMsg.robot_num.resize(drawMsg.num_links);
    drawMsg.position.resize(drawMsg.num_links);
    drawMsg.quaternion.resize(drawMsg.num_links);

    drawMsg.link_name[0] = "robot_one_link_one";
    drawMsg.robot_num[0] = 1;
    drawMsg.position[0] = {1.0, 2.0, 3.0};
    drawMsg.quaternion[0] = {4.0, 3.0, 2.0, 1.0};

    drawMsg.link_name[1] = "robot_two_link_one";
    drawMsg.robot_num[1] = 2;
    drawMsg.position[1] = {4.0, 5.0, 6.0};
    drawMsg.quaternion[1] = {8.0, 7.0, 6.0, 5.0};
    lastIndex = drawMsg.num_links - 1;
  }

  //////////////////////////////////////////////////
  /// \brief Checks that all the array-iterable values from
  /// lcmt_viewer_draw are matching their ignition counterpart
  void checkMsgTranslation(drake::lcmt_viewer_draw* lcmMsg,
                           ignition::msgs::PosesStamped* ignPoses) {
    for (int i = 0; i < lcmMsg->num_links; i++) {
      ASSERT_EQ(ignPoses->pose(i).name(), lcmMsg->link_name[i]);
      ASSERT_EQ(ignPoses->pose(i).id(), lcmMsg->robot_num[i]);
      ASSERT_EQ(ignPoses->pose(i).position().x(), lcmMsg->position[i][0]);
      ASSERT_EQ(ignPoses->pose(i).position().y(), lcmMsg->position[i][1]);
      ASSERT_EQ(ignPoses->pose(i).position().z(), lcmMsg->position[i][2]);
      ASSERT_EQ(ignPoses->pose(i).orientation().x(),
                lcmMsg->quaternion[i][0]);
      ASSERT_EQ(ignPoses->pose(i).orientation().y(),
                lcmMsg->quaternion[i][1]);
      ASSERT_EQ(ignPoses->pose(i).orientation().z(),
                lcmMsg->quaternion[i][2]);
      ASSERT_EQ(ignPoses->pose(i).orientation().w(),
                lcmMsg->quaternion[i][3]);
    }
  }
};

//////////////////////////////////////////////////
/// \brief Check that the translation of a PosesStamp
/// message with zero poses behaves as expected
TEST_F(ViewerDrawTest, TestZeroPosesInPosesStamp) {
  // Resize drawMsg defined in SetUp to zero poses
  drawMsg.num_links = 0;
  drawMsg.link_name.resize(0);
  drawMsg.robot_num.resize(0);
  drawMsg.position.resize(0);
  drawMsg.quaternion.resize(0);
  ignition::msgs::PosesStamped ignMsg;
  lcmToIgn(drawMsg, &ignMsg);
  ASSERT_EQ(ignMsg.pose_size(), 0);
}

//////////////////////////////////////////////////
/// \brief Check that the translation of a PosesStamp
/// message with only one pose behaves as expected
TEST_F(ViewerDrawTest, TestOnePoseInPosesStamp) {
  // Resize drawMsg defined in SetUp to single pose
  drawMsg.num_links = 1;
  drawMsg.link_name.resize(1);
  drawMsg.robot_num.resize(1);
  drawMsg.position.resize(1);
  drawMsg.quaternion.resize(1);
  ignition::msgs::PosesStamped ignMsg;
  lcmToIgn(drawMsg, &ignMsg);
  checkMsgTranslation(&drawMsg, &ignMsg);
}

//////////////////////////////////////////////////
/// \brief Check that the translation of a PosesStamp message
/// with more than one pose behaves as expected
TEST_F(ViewerDrawTest, TestThreePosesInPosesStamp) {
  // Resize drawMsg defined in SetUp to three poses
  drawMsg.num_links = 3;
  drawMsg.link_name.resize(3);
  drawMsg.robot_num.resize(3);
  drawMsg.position.resize(3);
  drawMsg.quaternion.resize(3);
  // Fill third link values
  drawMsg.link_name[2] = "robot_three_link_one";
  drawMsg.robot_num[2] = 3;
  drawMsg.position[2] = {7.0, 8.0, 9.0};
  drawMsg.quaternion[2] = {12.0, 11.0, 10.0, 9.0};
  ignition::msgs::PosesStamped ignMsg;
  lcmToIgn(drawMsg, &ignMsg);
  checkMsgTranslation(&drawMsg, &ignMsg);
}

//////////////////////////////////////////////////
/// \brief Test that a header's sec and nsec were
/// correctly calculated
TEST_F(ViewerDrawTest, TestTimeStamp) {
  lcmToIgn(drawMsg, &ignPosesStamped);
  int secs = 123;
  int nsecs = 456000000;
  ASSERT_EQ(secs, ignPosesStamped.time().sec());
  ASSERT_EQ(nsecs, ignPosesStamped.time().nsec());
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if at least one of the
///  defined position vectors doesn't have the correct size
TEST_F(ViewerDrawTest, TestExceptionInPosition) {
  drawMsg.position[1] = {1.0, 2.0, 3.0, 4.0};
  EXPECT_THROW(lcmToIgn(drawMsg, &ignPosesStamped), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if at least
/// one of the defined orientation vectors doesn't
/// have the correct size
TEST_F(ViewerDrawTest, TestExceptionInOrientation) {
  drawMsg.quaternion[1] = {4.0, 2.0, 1.0};
  EXPECT_THROW(lcmToIgn(drawMsg, &ignPosesStamped), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if the
/// link_name vector doesn't have the correct size
TEST_F(ViewerDrawTest, TestExceptionNumberOfLinkNames) {
  drawMsg.link_name.resize(drawMsg.num_links + 1);
  EXPECT_THROW(lcmToIgn(drawMsg, &ignPosesStamped), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if the
/// robot_num vector doesn't have the correct size
TEST_F(ViewerDrawTest, TestExceptionNumberOfRobotNum) {
  drawMsg.robot_num.resize(drawMsg.num_links - 1);
  EXPECT_THROW(lcmToIgn(drawMsg, &ignPosesStamped), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if the
/// position vector doesn't have the correct size
TEST_F(ViewerDrawTest, TestExceptionNumberOfPositions) {
  drawMsg.position.resize(drawMsg.num_links - 1);
  EXPECT_THROW(lcmToIgn(drawMsg, &ignPosesStamped), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if the
/// quaternion vector doesn't have the correct size
TEST_F(ViewerDrawTest, TestExceptionNumberOfQuaternions) {
  drawMsg.position.resize(drawMsg.num_links + 1);
  EXPECT_THROW(lcmToIgn(drawMsg, &ignPosesStamped), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that an LCM geometry message describing a
/// box is properly translated to an ignition Geometry message.
GTEST_TEST(BoxTest, TestBoxTranslation) {
  // Define LCM box geometry message
  drake::lcmt_viewer_geometry_data boxMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignBoxGeometry;
  // Fill LCM data
  boxMsg.type = boxMsg.BOX;
  boxMsg.num_float_data = 3;
  boxMsg.float_data.resize(boxMsg.num_float_data);
  boxMsg.float_data[0] = 3.888;
  boxMsg.float_data[1] = 0.753;
  boxMsg.float_data[2] = 0.031;

  // Translate from LCM to ignition
  lcmToIgn(boxMsg, &ignBoxGeometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ignBoxGeometry.has_box());
  ASSERT_EQ(ignition::msgs::Geometry::BOX, ignBoxGeometry.type());
  ASSERT_EQ(boxMsg.float_data[0], ignBoxGeometry.box().size().x());
  ASSERT_EQ(boxMsg.float_data[1], ignBoxGeometry.box().size().y());
  ASSERT_EQ(boxMsg.float_data[2], ignBoxGeometry.box().size().z());
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if an LCM geometry
/// message describing a box was not properly filled.
GTEST_TEST(BoxTest, TestExceptionInBoxTranslation) {
  // Define LCM box geometry message
  drake::lcmt_viewer_geometry_data boxMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignBoxGeometry;
  // Fill LCM data, but with missing z information
  boxMsg.type = boxMsg.BOX;
  boxMsg.num_float_data = 2;
  boxMsg.float_data.resize(boxMsg.num_float_data);
  boxMsg.float_data[0] = 1;
  boxMsg.float_data[1] = 2;

  EXPECT_THROW(lcmToIgn(boxMsg, &ignBoxGeometry), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that an LCM geometry message describing a cylinder
/// was properly translated to an ignition Geometry message.
GTEST_TEST(CylinderTest, TestCylinderTranslation) {
  // Define LCM cylinder geometry message
  drake::lcmt_viewer_geometry_data cylinderMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignCylinderGeometry;
  // Fill LCM data
  cylinderMsg.type = cylinderMsg.CYLINDER;
  cylinderMsg.num_float_data = 2;
  cylinderMsg.float_data.resize(cylinderMsg.num_float_data);
  cylinderMsg.float_data[0] = 1.368;  // radius
  cylinderMsg.float_data[1] = 4.246;  // length

  // Translate from LCM to ignition
  lcmToIgn(cylinderMsg, &ignCylinderGeometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ignCylinderGeometry.has_cylinder());
  ASSERT_EQ(ignition::msgs::Geometry::CYLINDER, ignCylinderGeometry.type());
  ASSERT_EQ(cylinderMsg.float_data[0],
            ignCylinderGeometry.cylinder().radius());
  ASSERT_EQ(cylinderMsg.float_data[1],
            ignCylinderGeometry.cylinder().length());
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if an LCM geometry
/// message describing a cylinder was not properly filled.
GTEST_TEST(CylinderTest, TestExceptionInCylinderTranslation) {
  // Define LCM cylinder geometry message
  drake::lcmt_viewer_geometry_data cylinderMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignCylinderGeometry;
  // Fill LCM data
  cylinderMsg.type = cylinderMsg.CYLINDER;
  cylinderMsg.num_float_data = 1;
  cylinderMsg.float_data.resize(cylinderMsg.num_float_data);
  cylinderMsg.float_data[0] = 1.368;  // radius

  EXPECT_THROW(lcmToIgn(cylinderMsg, &ignCylinderGeometry),
               TranslateException);
}

//////////////////////////////////////////////////
// \brief Test that an LCM geometry message describing a sphere
// was properly translated to an ignition Geometry message.
GTEST_TEST(SphereTest, TestSphereTranslation) {
  // Define LCM sphere geometry message
  drake::lcmt_viewer_geometry_data sphereMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignSphereGeometry;
  // Fill LCM data
  sphereMsg.type = sphereMsg.SPHERE;
  sphereMsg.num_float_data = 1;
  sphereMsg.float_data.resize(sphereMsg.num_float_data);
  sphereMsg.float_data[0] = 2.534;  // radius

  // Translate from LCM to ignition
  lcmToIgn(sphereMsg, &ignSphereGeometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ignSphereGeometry.has_sphere());
  ASSERT_EQ(ignition::msgs::Geometry::SPHERE, ignSphereGeometry.type());
  ASSERT_EQ(sphereMsg.float_data[0], ignSphereGeometry.sphere().radius());
}

//////////////////////////////////////////////////
/// \brief Test that translation fails if an LCM geometry
/// message describing a sphere was not properly filled.
GTEST_TEST(SphereTest, TestExceptionInCylinderTranslation) {
  // Define LCM sphere geometry message
  drake::lcmt_viewer_geometry_data sphereMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignSphereGeometry;
  // Fill LCM data
  sphereMsg.type = sphereMsg.SPHERE;
  sphereMsg.num_float_data = 0;
  sphereMsg.float_data.resize(sphereMsg.num_float_data);

  EXPECT_THROW(lcmToIgn(sphereMsg, &ignSphereGeometry), TranslateException);
}

//////////////////////////////////////////////////
/// \brief Test that an LCM geometry message describing a mesh with
/// scaling data was properly translated to an ignition Geometry message.
GTEST_TEST(MeshTest, TestMeshTranslationWithScale) {
  // Define LCM mesh geometry message
  drake::lcmt_viewer_geometry_data meshMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignMeshGeometry;
  // Fill LCM data
  meshMsg.type = meshMsg.MESH;
  meshMsg.string_data = "/path/to/mesh.obj";
  meshMsg.num_float_data = 3;
  meshMsg.float_data.resize(meshMsg.num_float_data);
  meshMsg.float_data[0] = 3.4;  // scale
  meshMsg.float_data[1] = 4.2;
  meshMsg.float_data[2] = 6.7;

  // Translate from LCM to ignition
  lcmToIgn(meshMsg, &ignMeshGeometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ignMeshGeometry.has_mesh());
  ASSERT_EQ(ignition::msgs::Geometry::MESH, ignMeshGeometry.type());
  ASSERT_EQ(meshMsg.string_data, ignMeshGeometry.mesh().filename());
  ASSERT_TRUE(ignMeshGeometry.mesh().has_scale());
  ASSERT_EQ(meshMsg.float_data[0], ignMeshGeometry.mesh().scale().x());
  ASSERT_EQ(meshMsg.float_data[1], ignMeshGeometry.mesh().scale().y());
  ASSERT_EQ(meshMsg.float_data[2], ignMeshGeometry.mesh().scale().z());
}

//////////////////////////////////////////////////
/// \brief Test that an LCM geometry message describing a mesh without
/// scaling data is properly translated to an ignition Geometry message.
GTEST_TEST(MeshTest, TestMeshTranslationWithoutScale) {
  // Define LCM mesh geometry message
  drake::lcmt_viewer_geometry_data meshMsg;
  // Define the expected ignition geometry
  ignition::msgs::Geometry ignMeshGeometry;
  // Fill LCM data
  meshMsg.type = meshMsg.MESH;
  meshMsg.string_data = "/path/to/mesh.obj";
  meshMsg.num_float_data = 0;
  meshMsg.float_data.resize(meshMsg.num_float_data);

  // Translate from LCM to ignition
  lcmToIgn(meshMsg, &ignMeshGeometry);

  // Verify generated ignition geometry
  ASSERT_EQ(true, ignMeshGeometry.has_mesh());
  ASSERT_EQ(ignition::msgs::Geometry::MESH, ignMeshGeometry.type());
  ASSERT_EQ(meshMsg.string_data, ignMeshGeometry.mesh().filename());
  ASSERT_FALSE(ignMeshGeometry.mesh().has_scale());
}

//////////////////////////////////////////////////
/// \brief Test that an LCM viewer link message describing a link
/// properly translates the robot_num to an ignition Geometry message.
GTEST_TEST(MeshTest, TestLinkTranslationWithRobotNum) {
  drake::lcmt_viewer_link_data linkMsg;
  ignition::msgs::Link ignLink;

  linkMsg.name = "test_link";
  linkMsg.robot_num = 1234;
  linkMsg.num_geom = 0;

  translate(linkMsg, &ignLink);

  ASSERT_EQ(1234, ignLink.id());
  ASSERT_EQ("test_link", ignLink.name());
}

}  // namespace bridge
}  // namespace delphyne
