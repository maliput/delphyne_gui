#!/usr/bin/env python3
#
# Copyright 2017 Toyota Research Institute
#
"""
The dragway demo.
"""
##############################################################################
# Imports
##############################################################################

import delphyne.roads as delphyne_roads
import delphyne.simulation as simulation
import delphyne.utilities as utilities

from delphyne_gui.utilities import launch_interactive_simulation

from . import helpers

##############################################################################
# Supporting Classes & Methods
##############################################################################


def parse_arguments():
    "Argument passing and demo documentation."
    parser = helpers.create_argument_parser(
        "Dragway",
        "\nAll of delphyne's agents on a single dragway.\n",
        default_duration=15.0)
    return parser.parse_args()

##############################################################################
# Main
##############################################################################


def main():
    """Keeping pylint entertained."""
    args = parse_arguments()

    builder = simulation.AgentSimulationBuilder()

    dragway = builder.set_road_geometry(
        delphyne_roads.create_dragway(
            name="Automotive Demo Dragway",
            num_lanes=4,
            length=100.0,
            lane_width=3.7,
            shoulder_width=3.0,
            maximum_height=5.0
        )
    )

    # Adds the different cars.
    simple_car_position_x = 0.0
    simple_car_position_y = 1.5 * 3.7
    car_id = 0
    utilities.add_simple_car(
        builder,
        name=str(car_id),
        position_x=simple_car_position_x,
        position_y=simple_car_position_y
    )

    car_id += 1
    utilities.add_mobil_car(
        builder,
        name=str(car_id),
        scene_x=0.0,
        scene_y=-0.5*3.7,
        heading=0.0,
        speed=1.0,
    )

    car_id += 1
    utilities.add_rail_car(
        builder,
        name=str(car_id),
        lane=dragway.junction(0).segment(0).lane(1),
        position=0.0,
        offset=0.0,
        speed=3.0
    )

    car_id += 1
    times = [0.0, 5.0, 10.0, 15.0, 20.0]
    headings = [0.0, 0.0, 0.0, 0.0, 0.0]
    waypoints = [
        [0.0,   -5.55, 0.0],  # pylint: disable=C0326
        [10.0,  -5.55, 0.0],  # pylint: disable=C0326
        [30.0,  -5.55, 0.0],  # pylint: disable=C0326
        [60.0,  -5.55, 0.0],  # pylint: disable=C0326
        [100.0, -5.55, 0.0]
    ]
    utilities.add_trajectory_agent(
        builder,
        str(car_id),
        times,
        headings,
        waypoints
    )

    runner = simulation.SimulationRunner(
        simulation=builder.build(),
        time_step=0.001,  # (secs)
        realtime_rate=args.realtime_rate,
        paused=args.paused,
        log=args.log,
        logfile_name=args.logfile_name)

    with launch_interactive_simulation(runner, bare=args.bare) as launcher:
        if args.duration < 0:
            # run indefinitely
            runner.start()
        else:
            # run for a finite time
            print("Running simulation for {0} seconds.".format(
                args.duration))
            runner.run_async_for(args.duration, launcher.terminate)
