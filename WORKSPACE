workspace(name = "drakaina")

load("//tools:github.bzl", "github_archive")
load("//tools:bitbucket.bzl", "bitbucket_archive")

local_repository(
    name = "kythe",
    path = "tools/third_party/kythe",
)

load("@kythe//tools/build_rules/config:pkg_config.bzl", "pkg_config_package")

pkg_config_package(
    name = "ignition-transport3",
    modname = "ignition-transport3",
)

pkg_config_package(
    name = "ignition-common0",
    modname = "ignition-common0",
)

pkg_config_package(
    name = "glib",
    modname = "glib-2.0",
)

pkg_config_package(
    name = "Qt5Core",
    modname = "Qt5Core",
)

github_archive(
    name = "lcm",
    repository = "lcm-proj/lcm",
    commit = "c0a0093a950fc83e12e8d5918a0319b590356e7e",
    sha256 = "d5bb1a0153b9c1526590e7d65be8ca79e4f5e9bf4ce58178c992eaca49d17fb0",
    build_file = "tools/lcm.BUILD",
)

bitbucket_archive(
    name = "ignition_gui",
    repository = "ignitionrobotics/ign-gui",
    commit = "039f8877d173",
    sha256 = "7454bf848b5df3d758bc6aace1deb53886cda224248bd97f8bff4ffeef09352d",
    strip_prefix = "ignitionrobotics-ign-gui-039f8877d173",
    build_file = "tools/ignition_gui.BUILD",
)
