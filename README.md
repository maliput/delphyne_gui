# Delphyne-gui

This is the repository for Delphyne GUI, a new front-end visualizer for the
Drake simulator. 

# Instructions

Instructions for setting up a delphyne workspace can be found in [Delphyne Guide#Installation](https://docs.google.com/document/d/1tQ9vDp084pMuHjYmtScLB3F1tdr4iP9w7_OTcoSM1zQ/edit#heading=h.e45b0m13gxl4).

# Quick Test

```
source install/setup.bash
demo_launcher.py --demo=simple
```

# Instructions for the clang-format tool
In order to get all the C++ code in the project compliant with a single style, we strongly recommend you using the auto-formatting tool called clang-format.

You can execute it against your source code by doing:
```
/usr/bin/clang-format-3.9 -i -style=file <path/to/file.cpp>
```
This will automatically apply the code conventions specified in the .clang-format file, found on the root of the repository.

There is also an automated script that looks for all the C++ src/header files and then calls clang-format accordingly. You can invoque it by doing:

```
./tools/reformat_code.sh
```

This script must be run from the top-level of the repository in order to find
all of the files. It is recommended to run this before opening any pull request.
