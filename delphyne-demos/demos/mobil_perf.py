#!/usr/bin/env python3
#
# Copyright 2018 Toyota Research Institute
#
"""
The performance benchmark demo.
"""
##############################################################################
# Imports
##############################################################################

import math

import delphyne.behaviours
import delphyne.blackboard
import delphyne.trees
import delphyne_gui.utilities

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def benchmark(setup_fn):
    """
    Helper decorator to register benchmark simulation
    setup functions.
    """
    if not hasattr(benchmark, 'register'):
        benchmark.register = {}
    benchmark.register[setup_fn.__name__] = setup_fn
    return setup_fn


@benchmark
def curved_lanes(args):
    """
    Sets up a simulation with `args.num_cars` MOBIL cars on a few
    lanes of a curved (arc) road.
    """

    # Loads Multilane road.
    scenario_subtree = delphyne.behaviours.roads.Multilane(
        file_path=delphyne_gui.utilities.get_delphyne_gui_resource(
            'roads/curved_lanes.yaml'
        )
    )

    # Adds the N MOBIL cars to the multilane.
    R0 = 320.  # m
    for i in range(args.num_cars):
        R = R0 - 4. * (i % 3)  # m
        # For a 6m distance between cars.
        theta = (12. / R0) * (i / 3)  # rads
        scenario_subtree.add_child(
            delphyne.behaviours.agents.MobilCar(
                name="mobil" + str(i),
                initial_pose=(
                    R * math.sin(theta),  # m
                    R0 - R * math.cos(theta),  # m
                    theta  # rads
                ),
                speed=1.,  # m/s
            )
        )

    # Adds the N*T rail cars to the multilane.
    num_traffic = int(args.traffic_density * args.num_cars)
    lane_provider = delphyne.blackboard.providers.LaneLocationProvider(distance_between_agents=6.)
    for i in range(num_traffic):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.RailCar(
                name="rail " + str(i),
                lane_id=lane_provider.random_lane,
                longitudinal_position=12. * (i / 3) + 6.,  # m
                lateral_offset=0.,  # m
                speed=1.  # m/s
            )
        )

    return scenario_subtree


@benchmark
def straight_lanes(args):
    """
    Sets up a simulation with `args.num_cars` MOBIL cars on a few
    lanes of a straight road.
    """

    # Loads Multilane road.
    scenario_subtree = delphyne.behaviours.roads.Multilane(
        file_path=delphyne_gui.utilities.get_delphyne_gui_resource(
            'roads/straight_lanes.yaml'
        )
    )

    # Adds the N MOBIL cars to the multilane.
    for i in range(args.num_cars):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.MobilCar(
                name="mobil" + str(i),
                initial_pose=(
                    12. * (i / 3),  # m
                    4. * (i % 3),  # m
                    0.  # rads
                ),
                speed=1.,  # m/s
            )
        )

    # Adds the N*T rail cars to the multilane.
    num_traffic = int(args.traffic_density * args.num_cars)
    lane_provider = delphyne.blackboard.providers.LaneLocationProvider(distance_between_agents=6.)
    for i in range(num_traffic):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.RailCar(
                name="rail " + str(i),
                lane_id=lane_provider.random_lane,
                longitudinal_position=12. * (i / 3) + 6.,  # m
                lateral_offset=0.,  # m
                speed=1.   # m/s
            )
        )

    return scenario_subtree


@benchmark
def dragway(args):
    """
    Sets up a simulation with `args.num_cars` MOBIL cars on a dragway
    road with four (4) lanes.
    """

    scenario_subtree = delphyne.behaviours.roads.Dragway(
        name="dragway",
        num_lanes=4,
        length=100.0,  # m
        lane_width=3.7,  # m
        shoulder_width=3.0,  # m
        maximum_height=5.0  # m
    )

    # Adds the N MOBIL cars to the dragway.
    for i in range(args.num_cars):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.MobilCar(
                name="mobil" + str(i),
                initial_pose=(
                     12. * (i / 4),  # m
                    -5.5 + 3.7 * (i % 4),  # m
                     0.  # rads
                ),
                speed=1.,  # m/s
            )
        )

    # Adds the N*T rail cars to the multilane.
    num_traffic = int(args.traffic_density * args.num_cars)
    lane_provider = delphyne.blackboard.providers.LaneLocationProvider(distance_between_agents=6.)
    for i in range(num_traffic):
        scenario_subtree.add_child(
            delphyne.behaviours.agents.RailCar(
                name="rail " + str(i),
                lane_id=lane_provider.random_lane,
                longitudinal_position=12. * (i / 4) + 6.,  # m
                lateral_offset=0.,  # m
                speed=1.   # m/s
            )
        )

    return scenario_subtree


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "MOBIL Performance Check",
        "\nCPU hungry MOBIL cars on common roads.\n",
        default_duration=5.0)
    available_benchmarks = benchmark.register.keys()
    parser.add_argument(
        "benchmark", choices=available_benchmarks,
        help="Benchmark to be run."
    )
    parser.add_argument(
        "-t", "--traffic-density", default=0, type=float,
        help=("The number of rail cars per "
              "MOBIL car on scene (default: 0).")
    )
    parser.add_argument(
        "-n", "--num-cars", default=20, type=int,
        help="The number of MOBIL cars on scene (default: 20)."
    )
    return parser.parse_args()


##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    simulation_tree = delphyne.trees.BehaviourTree(
        root=benchmark.register[args.benchmark](args)
    )

    simulation_tree.setup(
        realtime_rate=args.realtime_rate,
        start_paused=args.paused,
        logfile_name=args.logfile_name
    )

    tree_time_step = 0.02
    with launch_interactive_simulation(
        simulation_tree.runner, bare=args.bare
    ) as launcher:
        if args.duration < 0:
            # run indefinitely
            print("Running simulation indefinitely.")
            simulation_tree.tick_tock(period=tree_time_step)
        else:
            # run for a finite amount of time
            print("Running simulation for {0} seconds.".format(args.duration))
            simulation_tree.tick_tock(
                period=tree_time_step, number_of_iterations=int(args.duration / tree_time_step)
            )
        # stop simulation if it's necessary
        if simulation_tree.runner.is_interactive_loop_running():
            simulation_tree.runner.stop()
        # print simulation stats
        print("Simulation ended. I'm happy, you should be too.")
        delphyne_gui.utilities.print_simulation_stats(simulation_tree.runner)
        launcher.terminate()
