#!/usr/bin/env python3
#
# Copyright 2019 Toyota Research Institute
#
"""
The mali demo.
```
"""
##############################################################################
# Imports
##############################################################################

import os.path

import delphyne.trees
import delphyne.behaviours
import delphyne.blackboard.providers
import delphyne.roads as delphyne_roads
import delphyne.utilities as utilities

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################

KNOWN_ROADS = {
    'LineSingleLane': {
        'description': 'Single line lane of 100m length',
        'file_path': 'odr/SingleLane.xodr',
        'lane_id': '1_0_-1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'ArcSingleLane': {
        'description': ('Single arc lane of 100m length '
                        'and 40m of radius'),
        'file_path': 'odr/ArcLane.xodr',
        'lane_id': '1_0_1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'Roundabout': {
        'description': ('Single lane roundabout of 200m '
                        'length and ~31.83m of radius'),
        'lane_id': '1_0_1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'LShapeSection': {
        'description': ('Single road with 3 lane sections '
                        'with line-arc-line geometry'),
        'lane_id': '1_0_1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'LShapeRoad': {
        'description': ('3 roads connected each with line, '
                        'arc and line geometry respectively'),
        'lane_id': '1_0_1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'TShapeRoad': {
        'description': 'T intersection road with double hand roads',
        'lane_id': '1_0_1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'Crossing8Course': {
        'description': 'Crossing with 8 shape',
        'lane_id': '514_0_-1',
        'lane_position': 0.,
        'moving_forward': True,
    },
    'RRFigure8': {
        'description': 'Crossing with 8 shape (another)',
        'lane_id': '4_0_-1',
        'lane_position': 80.,
        'moving_forward': True,
    },
    'RRLongRoad': {
        'description': 'Long road with turning lanes',
        'lane_id': '3_0_-1',
        'lane_position': 16.,
        'moving_forward': False,
    },
}


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        'Mali Racing!',
        """
An example of a railcar running in an OpenDrive based maliput road.
        """
    )

    argument_help = """\
The OpenDRIVE road description to drive on. Either a path
to an .xodr file or one of the following well known roads: {}.
All relative paths are resolved against MALIDRIVE_RESOURCE_ROOT if not
found in the current working directory.
""".format(', '.join(KNOWN_ROADS))

    parser.add_argument(
        '-n', '--road-name', default='Roundabout', help=argument_help
    )
    return parser.parse_args()


def get_malidrive_resource(path):
    """Resolve the path against malidrive resources root location."""
    root = utilities.get_from_env_or_fail('MALIDRIVE_RESOURCE_ROOT')
    for root in root.split(':'):
        resolved_path = os.path.join(root, 'resources', path)
        if os.path.exists(resolved_path):
            return resolved_path
    return ''


def create_mali_scenario_subtree(file_path, features,
                                 lane_position, direction_of_travel,
                                 lane_id):
    scenario_subtree = delphyne.behaviours.roads.Malidrive(
        file_path=file_path,
        features=features,
        name=os.path.splitext(os.path.basename(file_path))[0]
    )
    scenario_subtree.add_child(
        delphyne.behaviours.agents.RailCar(
            name='car',
            lane_id=lane_id,
            longitudinal_position=lane_position,
            lateral_offset=0.0,
            speed=15.0,
            direction_of_travel=direction_of_travel
        )
    )
    return scenario_subtree

##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    if os.path.isfile(args.road_name):
        road = {
            'description': 'Custom user-provided road',
            'file_path': args.road_name,
            'lane_position': 0.,
            'moving_forward': True,
        }
    elif args.road_name in KNOWN_ROADS:
        road = KNOWN_ROADS[args.road_name]
        if 'file_path' not in road:
            road['file_path'] = os.path.join(
                'odr', args.road_name + '.xodr'
            )
        if not os.path.isabs(road['file_path']):
            road['file_path'] = get_malidrive_resource(road['file_path'])
    else:
        print("Unknown road {}.".format(args.road_name))
        quit()

    if 'lane_id' in road:
        lane_id = road['lane_id']
    else:
        lane_provider = delphyne.blackboard.providers.LaneLocationProvider(distance_between_agents=1.0)
        lane_id = lane_provider.random_lane

    features = delphyne_roads.ObjFeatures()
    features.draw_arrows = True
    features.draw_elevation_bounds = False
    features.draw_stripes = True
    features.draw_lane_haze = False
    features.draw_branch_points = False

    simulation_tree = delphyne.trees.BehaviourTree(
        root=create_mali_scenario_subtree(road['file_path'], features,
            road['lane_position'], road['moving_forward'], lane_id))

    sim_runner_time_step = 0.015
    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name,
        time_step=sim_runner_time_step
    )

    tree_time_step = 0.03
    with launch_interactive_simulation(
        simulation_tree.runner, bare=args.bare, ign_visualizer="visualizer0"
    ) as launcher:
        if args.duration < 0:
            # run indefinitely
            print("Running simulation indefinitely.")
            simulation_tree.tick_tock(period=tree_time_step)
        else:
            # run for a finite time
            print("Running simulation for {0} seconds.".format(args.duration))
            simulation_tree.tick_tock(
                period=tree_time_step, number_of_iterations=int(args.duration / tree_time_step)
            )
        launcher.terminate()
