1. Install [nvidia-docker2](https://github.com/nvidia/nvidia-docker/wiki/Installation-(version-2.0)).

2. Create docker file and shell script (you can run with -p to specify workspace path and -u to specify user name. Use -h to see help):

```sh
python create_docker_files.py -p /path/to/workspace
```

3. Build Docker image (Use delphyne:18.04 as image name):

```sh
cd /path/to/workspace && docker build -t delphyne:18.04 .
```

4. Create a directory to use for Bazel caching:

```sh
sudo mkdir -p /var/cache/docker
```

5. Jump into the container:

```sh
./start_ws.sh
```

6. Bootstrap Delphyne's workspace (some prompting involved):

```sh
make workspace
```

7. Detach terminal from container using Ctrl-P, Ctrl-Q.

8. Commit latest image changes on top of the initial one:

```sh
docker commit `docker ps -q | head -n 1` delphyne:18.04
```

9. To build dependencies, go to your workspace folder in your docker container and run

```sh
make drake ignition d dg
```
NOTE: To be able to run the script properly, you need to have in the same folder create_docker_files.py and Makefile.
