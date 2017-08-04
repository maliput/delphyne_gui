cc_binary(
    name = "lcm-to-ign-trans-bridge",
    srcs = ["lcm-to-ign-trans-bridge.cc"],
    deps = [
        "@ignition-transport3",
        "@lcm",
    ],
)

cc_binary(
    name = "lcm-robot-publisher",
    srcs = [
        "lcm_robot_publisher.cc",
        "lcmtypes/lcmt_viewer_geometry_data.hpp",
        "lcmtypes/lcmt_viewer_link_data.hpp",
        "lcmtypes/lcmt_viewer_load_robot.hpp",
        ],
    deps = [
        "@lcm",
    ],
)
