load("@drakaina//tools:ignition_math_external.bzl", "IGN_MATH3_COPTS")

cc_binary(
    name = "lcm-to-ign-trans-bridge",
    srcs = ["lcm-to-ign-trans-bridge.cc"],
    copts = IGN_MATH3_COPTS,
    linkopts = [
        "-lignition-math3",
    ],
)
