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
  Outliner(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ, int _poolSize,
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

  /// \brief Get a new tolerance between cubes based on the lane's distance and the amount of cubes available for each
  /// side
  /// \param[in] _langeLength Length of the lane in s coordinate
  /// \param[in] _cubesUsedForSide Amount of cubes available to populate a side of the lane.
  double GetNewToleranceToPopulateLane(double _laneLength, int _cubesUsedForSide);

  /// \brief Sets cubes world position and rotation in the middle of two points considering only the r coordinate
  /// and assuming a straight line.
  /// \param[in] _minRGeoPos World position of the left extreme point.
  /// \param[in] _maxRGeoPos World position of the right extreme point.
  /// \param[in] _maxAmountOfCubesToUse Amount permitted of cubes to place in the lane.
  void MoveCubeAtMidPointInR(const maliput::api::GeoPosition& _minRGeoPos, const maliput::api::GeoPosition& _maxRGeoPos,
                             int* _cubesUsed, int* _maxAmountOfCubesToUse);

  /// \brief Sets cubes world position and rotation in the maximum given bound of a given lane for a given range in the
  /// s coordinate.
  /// \param[in] _lane Lane to be covered in the left side by red cubes.
  /// \param[in] _min_s min s coordinate to get the mid s point.
  /// \param[in] _max_s max s coordinate to get the mid s point.
  /// \param[in] _left_side which side of the lane should be populated.
  /// \param[in] _maxAmountOfCubesToUse Amount permitted of cubes to place in the lane.
  void MoveCubeAtMidPointInS(const maliput::api::Lane* _lane, double _min_s, double _max_s, bool _left_side,
                             int* _cubesUsed, int* _maxAmountOfCubesToUse);

  /// \brief Sets cubes visibility from and to a given point.
  /// \param[in] _startFrom From which cube should we start changing the visibility.
  /// \param[in] _to Until which cube should we change the visibility.
  /// \param[in] _visible Boolean that determines the visibility of the cubes.
  void SetVisibilityOfCubesStartingFromTo(int _startFrom, int _to, bool _visible);

  /// \brief Verifies if two points are so close that they violate the tolerance.
  /// \param[in] _first_point First point to verify distance.
  /// \param[in] _second_point Second point to verify distance.
  /// \returns Boolean that determines if the two points are close to each other.
  bool DoPointsViolateTolerance(const maliput::api::GeoPosition& _first_point,
                                const maliput::api::GeoPosition& _second_point);

  /// \brief Cubes used for rendering the outline in roads.
  std::vector<ignition::rendering::VisualPtr> cubes;

  /// \brief Cache lane pointer to avoid setting the outline twice.
  const maliput::api::Lane* lastLaneOutlined;
  /// \brief Cache cubes used for the last lane so we hide the leftovers only.
  int lastCubesUsed;

  /// \brief Tolerance used for cubes positioning.
  double minTolerance;
};
}  // namespace gui
}  // namespace delphyne

#endif  // DELPHYNE_GUI_OUTLINER_HH