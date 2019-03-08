# How to create the workspace
To simplify and start to work as soon as possible, **create_environment.py** script wll be your friend.
You can add the following flags:
* -p <path> : Path to create the workspace. By default it uses the current directory.
* -d <name> : Docker image name. By default it uses delphyne:18.04
* --use_drake_binary : By default it will use source files
* --use_ignition_source : By default it will use binaries.

**Example:** `./create_environment.py -p /home/foo/bar -d delph --use_drake_binary`

You **MUST** install [nvidia-docker2](https://github.com/nvidia/nvidia-docker/wiki/Installation-(version-2.0)).

1. Run `create_environment.py` with the flags you want (remember to install [nvidia-docker2](https://github.com/nvidia/nvidia-docker/wiki/Installation-(version-2.0)) before)

# How to build delphyne

1. Go to your workspace folder and run `./start_ws.sh`. You should be on your container now sitting on your home directory. `cd` to your workspace directory.
2. Run `colcon build --packages-up-to delphyne-gui`
3. Once it finishes, run `source install/setup.bash`. **DON'T FORGET TO DO THIS, OTHERWISE NO DEMO WILL WORK.**

You can checkout our examples by typing delphyne on your shell and press **TAB** to display the options.
**Example**: `delphyne-gazoo`

Everything is ready for you to start developing! :)

**Note:** Everytime you stop the container and run it again, you'll have to run `source install/setup.bash` again