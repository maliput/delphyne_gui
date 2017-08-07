cc_binary(
    name = "lcm-to-ign-trans-bridge",
    srcs = [
        "lcm-to-ign-trans-bridge.cc",
        "lcm_to_ign_translator.cc",
        "lcm_to_ign_translator.h",
        "lcm_channel_repeater.h",
        "drake/lcmt_viewer_command.hpp",
        "drake/lcmt_viewer_draw.hpp",
        "drake/lcmt_viewer_geometry_data.hpp",
        "drake/lcmt_viewer_link_data.hpp",
        "drake/lcmt_viewer_load_robot.hpp"
        ],
    deps = [
        "@ignition-transport3",
        "@lcm",
    ],
)

cc_binary(
    name = "lcm-robot-publisher",
    srcs = [
        "lcm_robot_publisher.cc",
        "drake/lcmt_viewer_geometry_data.hpp",
        "drake/lcmt_viewer_link_data.hpp",
        "drake/lcmt_viewer_load_robot.hpp",
        ],
    deps = [
        "@lcm",
        "@ignition_common",
    ],
)
