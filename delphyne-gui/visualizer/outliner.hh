// Copyright 2019 Toyota Research Institute

#ifndef DELPHYNE_GUI_OUTLINER_HH
#define DELPHYNE_GUI_OUTLINER_HH

#include <ignition/math/Vector3.hh>
#include <ignition/rendering/Scene.hh>
#include <maliput/api/lane.h>

#include <vector>

namespace delphyne {
namespace gui {

/// \brief Outline lanes with red cubes along the sides of the lane.
class Outliner {
 public:
  /// \brief Initializes the pool of red cubes to be used for outlining a lane.
  /// \param[in] _scene Pointer to the scene where cubes will live.
  /// \param[in] _scaleX Scale factor for the X axis. This value multiplied by two will be used if tolerance is lower
  /// than the length of the cube in the X axis of each cube.
  /// \param[in] _scaleY Scale factor for the Y axis of each cube.
  /// \param[in] _scaleZ Scale fator for the Z axis of each cube.
  /// \param[in] _poolSize Amount of cubes to be used for outlining.
  /// \param[in] _minTolerance Distance between cubes.
  Outliner(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ, size_t _poolSize,
           double _minTolerance);
  /// \brief Destructor. Cube's destruction will be in charge of the scene's destructor.
  ~Outliner() = default;

  /// \brief Outlines a given lane if and only if is a new lane.
  /// \param[in] _lane Lane to be outlined.
  void OutlineLane(const maliput::api::Lane* _lane);

  /// \brief Hides every cube used and resets the cache.
  /// \param[in] _visible Boolean that determines the visibility of the outlining.
  void SetVisibility(bool _visible);

 private:
  /// \brief Creates the pool of cubes.
  /// \param[in] _scene Pointer to the scene where cubes will live.
  /// \param[in] _scaleX Scale factor for the X axis of each cube.
  /// \param[in] _scaleY Scale factor for the Y axis of each cube.
  /// \param[in] _scaleZ Scale fator for the Z axis of each cube.
  /// \param[in] _material Red material that it's created in the constructor.
  void CreateCubes(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ,
                   ignition::rendering::MaterialPtr& _material);

  /// \brief Sets cubes world position and rotation in the middle of two points considering only the r coordinate
  /// and assuming a straight line.
  /// \param[in] _minRGeoPos World position of the left extreme point.
  /// \param[in] _maxRGeoPos World position of the right extreme point.
  /// \param[in] _maxAmountOfCubesToUse Amount permitted of cubes to place in the lane.
  void MoveCubeAtMidPointInR(const maliput::api::GeoPosition& _minRGeoPos, const maliput::api::GeoPosition& _maxRGeoPos,
                             size_t& _maxAmountOfCubesToUse);

  /// \brief Sets cubes world position and rotation in the minimun left bound of a given lane for a given range in the
  /// s coordinate.
  /// \param[in] _lane Lane to be covered in the left side by red cubes.
  /// \param[in] _min_s min s coordinate to get the mid s point.
  /// \param[in] _max_s max s coordinate to get the mid s point.
  /// \param[in] _maxAmountOfCubesToUse Amount permitted of cubes to place in the lane.
  void MoveCubeAtMidPointInSLeftSide(const maliput::api::Lane* _lane, double _min_s, double _max_s,
                                     size_t& _maxAmountOfCubesToUse);

  /// \brief Sets cubes world position and rotation in the maximum right bound of a given lane for a given range in the
  /// s coordinate.
  /// \param[in] _lane Lane to be covered in the left side by red cubes.
  /// \param[in] _min_s min s coordinate to get the mid s point.
  /// \param[in] _max_s max s coordinate to get the mid s point.
  /// \param[in] _maxAmountOfCubesToUse Amount permitted of cubes to place in the lane.
  void MoveCubeAtMidPointInSRightSide(const maliput::api::Lane* _lane, double _min_s, double _max_s,
                                      size_t& _maxAmountOfCubesToUse);

  /// \brief Cubes used for rendering the outline in roads.
  std::vector<ignition::rendering::VisualPtr> cubes;

  /// \brief Cache lane pointer to avoid setting the outline twice.
  const maliput::api::Lane* lastLaneOutlined;
  /// \brief Keep track of cubes used for the outlining.
  size_t cubesUsed;
  /// \brief Cache cubes used for the last lane so we hide the leftovers only.
  size_t lastCubesUsed;

  /// \brief Tolerance used for cubes positioning.
  double minTolerance;
};
}  // namespace gui
}  // namespace delphyne

#endif  // DELPHYNE_GUI_OUTLINER_HH