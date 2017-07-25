workspace(name = "drakaina")

load("//tools:github.bzl", "github_archive")

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
    name = "glib",
    modname = "glib-2.0",
)

github_archive(
    name = "lcm",
    repository = "lcm-proj/lcm",
    commit = "c0a0093a950fc83e12e8d5918a0319b590356e7e",
    sha256 = "d5bb1a0153b9c1526590e7d65be8ca79e4f5e9bf4ce58178c992eaca49d17fb0",
    build_file = "tools/lcm.BUILD",
)
