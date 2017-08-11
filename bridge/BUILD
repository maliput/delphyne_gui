load(
    "//tools:drake.bzl",
    "drake_cc_googletest",
)

cc_binary(
    name = "lcm-to-ign-transport-bridge",
    srcs = [
        "lcm_to_ign_transport_bridge.cc",
        "lcm_to_ign_translation.cc",
        "lcm_to_ign_translation.hh",
        "lcm_channel_repeater.hh",
        "drake/lcmt_viewer_command.hpp",
        "drake/lcmt_viewer_draw.hpp",
        "drake/lcmt_viewer_geometry_data.hpp",
        "drake/lcmt_viewer_link_data.hpp",
        "drake/lcmt_viewer_load_robot.hpp"
        ],
    deps = [
        "@ignition-transport3",
        "@lcm",
        "@ignition_common//:ignition_common_shared_library",
    ],
    data = [
        "drake/car/car.vtp",
        "drake/package.xml",
        "drake/car/package.xml"
    ],
)

cc_binary(
    name = "lcm-mock-robot-publisher",
    srcs = [
        "lcm_mock_robot_publisher.cc",
        "drake/lcmt_viewer_geometry_data.hpp",
        "drake/lcmt_viewer_link_data.hpp",
        "drake/lcmt_viewer_load_robot.hpp",
        ],
    deps = [
        "@lcm",
        "@ignition_common//:ignition_common_shared_library",
    ],
)

py_binary(
    name = "mocked-robot-demo",
    srcs = ["mocked_robot_demo.py"],
    data = [
        ":lcm-mock-robot-publisher",
        ":lcm-to-ign-transport-bridge"
    ],
    main = "mocked_robot_demo.py"
)

drake_cc_googletest(
    name = "dummy_test",
)
