1. Install [nvidia-docker2](https://github.com/nvidia/nvidia-docker/wiki/Installation-(version-2.0)).

2. Run the following python script

```sh
python create_enviroment.py -p /path/to/workspace -d <docker_image_name>
```

3. If you want to build all dependencies, run the container if it's not running (./start_ws.sh) and do the following:

```sh
make drake ignition d dg
```

NOTE: To be able to run the script properly, you need to have in the same folder create_enviroment.py, create_docker_files.py and Makefile.