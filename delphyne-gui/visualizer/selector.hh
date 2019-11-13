// Copyright 2019 Toyota Research Institute

#ifndef DELPHYNE_GUI_SELECTOR_HH
#define DELPHYNE_GUI_SELECTOR_HH

#include <ignition/math/Vector3.hh>
#include <ignition/rendering/Scene.hh>
#include <maliput/api/lane.h>
#include <maliput/api/road_geometry.h>

#include <vector>

namespace delphyne {
namespace gui {

/// \brief Selects lanes and outlines them with red cubes along the sides of the lane.
class Selector {
 public:
  /// \brief Initializes the pool of red cubes to be used for selecting lanes.
  /// \param[in] _scene Pointer to the scene where cubes will live.
  /// \param[in] _scaleX Scale factor for the X axis. This value multiplied by two will be used if tolerance is lower
  /// than the length of the cube in the X axis of each cube.
  /// \param[in] _scaleY Scale factor for the Y axis of each cube.
  /// \param[in] _scaleZ Scale fator for the Z axis of each cube.
  /// \param[in] _poolSize Amount of cubes to be used for outlining.
  /// \param[in] _numLanes The number of lanes to auto-initialize visuals for
  /// \param[in] _minTolerance Distance between cubes.
  Selector(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ, int _poolSize,
           int _numLanes, double _minTolerance);
  /// \brief Destructor. Cube's destruction will be in charge of the scene's destructor.
  ~Selector() = default;

  /// \brief Selects `_lane`'s mesh when it is not selected, or deselects it when it is selected.
  /// \param[in] _lane Lane to be outlined.
  void SelectLane(const maliput::api::Lane* _lane);

  /// \brief Hides every cube used and resets the cache.
  /// \param[in] _visible Boolean that determines the visibility of the outlining.
  void SetVisibility(bool _visible);

  /// \brief Deselects all selected regions by calling `ResetPopulationMap()`,
  /// `ClearSelectedBranchPoints()` and `ClearSelectedLanes()`
  void DeselectAll();

  /// \brief Gets the currently selected lanes as a string vector.
  /// \returns A vector of the lane_id's which are selected.
  std::vector<std::string> GetSelectedLanes();

  /// \brief Gets the currently selected BranchPoints as a string vector.
  /// \returns A vector of the branch_point_id's which are selected.
  std::vector<std::string> GetSelectedBranchPoints();

  /// \brief Finds if the passed in lane is currently selected or not.
  /// \param[in] _lane Lane to be evaluated.
  /// \returns Boolean that determines whether the lane is selected or not.
  bool IsSelected(const maliput::api::Lane* _lane);

  /// \brief Finds if mesh corresponding to the id is currently selected or not.
  /// \param[in] _id The id to be evaluated.
  /// \returns Boolean that determines whether the mesh is selected or not.
  bool IsSelected(const std::string& _id);

 private:
  /// \brief Creates the pool of cubes.
  /// \param[in] _scene Pointer to the scene where cubes will live.
  /// \param[in] _scaleX Scale factor for the X axis of each cube.
  /// \param[in] _scaleY Scale factor for the Y axis of each cube.
  /// \param[in] _scaleZ Scale fator for the Z axis of each cube.
  /// \param[in] _material Red material that it's created in the constructor.
  void CreateCubes(ignition::rendering::ScenePtr& _scene, double _scaleX, double _scaleY, double _scaleZ,
                   ignition::rendering::MaterialPtr& _material, unsigned int _numCubes);

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

  /// \brief Sets the complete segment population map to false, indicating no cube segments are currently in use
  void ResetPopulationMap();

  /// \brief Clears the currently selected branchpoint group.
  void ClearSelectedBranchPoints();

  /// \brief Clears the currently selected lane group.
  void ClearSelectedLanes();

  /// \brief Finds the first empty segment of visuals in the cubes vector
  /// \returns The index of the segment, -1 if no segment is found
  int FindFirstEmpty();

  /// \brief Calculates the lane index which the markers for the corresponding lanes
  /// are located at.  This index is dependent upon the `cubesPerLane` variable.
  /// \param[in] _slot The slot within the cubes vector that holds a lane's outline markers
  int GetLaneIndex(int _slot);

  /// \brief Gets the index that the pre-initialized cubes start at.  Adds 4 due to the 4 corners of the lanes
  /// being populated beforehand
  int GetCubeIndex(int _laneIndex);

  /// \brief Gets the remaining number of cubes to populate for a lane.  Subtracts 4 as 4 cubes have already
  /// been set before this call for the corners of the lane.
  int GetRemainingCubes();

  /// \brief Cubes used for rendering the outline in roads.
  std::vector<ignition::rendering::VisualPtr> cubes;

  /// \brief Pointer to the scene for dynamic cube visual creation.
  ignition::rendering::ScenePtr scene;

  /// \brief Material of the cubes surrounding a selected lane.
  ignition::rendering::MaterialPtr cubeMaterial;

  /// \brief A vector containing which segments of the cubes vector are currently in use.
  std::vector<bool> populationMap;

  /// \brief A map of all of the currently selected branch points.
  /// The branch point id is the key and the amount of times it has been
  /// selected (to account for two lanes being able to select the same
  /// branch point) is the value
  std::map<std::string, int> branchPointsSelected;

  /// \brief A map of all of the currently selected lanes.
  /// The lane id is the key and the corresponding slot number of its visuals is the value
  std::map<std::string, int> lanesSelected;

  /// \brief The dimension scales of the world
  ignition::math::Vector3d dimensionScale;

  /// \brief The number of lanes to extend the cubes vector by when space is exhausted.
  const unsigned int numLanes;

  /// \brief The number of cubes that will be displayed per lane selected.
  const unsigned int cubesPerLane;

  /// \brief Tolerance used for cubes positioning.
  double minTolerance;
};
}  // namespace gui
}  // namespace delphyne

#endif  // DELPHYNE_GUI_SELECTOR_HH
