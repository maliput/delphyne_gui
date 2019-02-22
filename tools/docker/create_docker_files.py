#!/usr/bin/env python

import argparse
import os
import pwd
import shutil
import sys

DOCKER_FILE = """FROM nvidia/opengl:1.0-glvnd-runtime-ubuntu18.04

# Setup nvidia runtime
ENV NVIDIA_VISIBLE_DEVICES ${NVIDIA_VISIBLE_DEVICES:-all}
ENV NVIDIA_DRIVER_CAPABILITIES ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics

# Setup environment
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y locales
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8

# Install development packages
RUN apt-get update && apt-get install -y sudo tmux git make python-vcstools openssh-server software-properties-common bash-completion debian-keyring debian-archive-keyring

# Create a user with passwordless sudo
RUN adduser --gecos \"Development User\" --disabled-password {USER_NAME}
RUN adduser {USER_NAME} sudo
RUN echo \'%sudo ALL=(ALL) NOPASSWD:ALL\' >> /etc/sudoers
RUN echo \"export QT_X11_NO_MITSHM=1\" >> /home/{USER_NAME}/.bashrc

WORKDIR /home/{USER_NAME}
USER {USER_NAME}"""

SHELL_FILE = """#! /bin/bash
SCRIPT_PATH=$(dirname "$(readlink -f "$0")")
docker run --name delphyne --privileged --rm --net=host -e DISPLAY=${DISPLAY} \\
       --runtime=nvidia -v $SCRIPT_PATH:$SCRIPT_PATH \\
       -v /var/cache/docker:/home/{USER_NAME}/.cache \\
       -v /home/{USER_NAME}/.ssh:/home/{USER_NAME}/.ssh \\
       -v /tmp/.X11-unix:/tmp/.X11-unix:ro $@ \\
       -it {DOCKER_IMAGE_NAME} /bin/bash
"""

def create_docker_file(user_name, path, docker_image_name):
    print('Using user: ' + user_name + " and path: " + path)
    with open(os.path.join(path, 'Dockerfile'), 'w') as docker_file:
        docker_file.write(DOCKER_FILE.replace('{USER_NAME}', user_name))
    create_shell_file(user_name, path, docker_image_name)

def create_shell_file(user_name, path, docker_image_name):
    with open(os.path.join(path, 'start_ws.sh'), 'w') as shell_file:
        sh_replaced = SHELL_FILE.replace('{USER_NAME}', user_name)
        shell_file.write(sh_replaced.replace('{DOCKER_IMAGE_NAME}', docker_image_name))
    os.chmod(path + 'start_ws.sh', 0775)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create docker environment.')
    parser.add_argument('-u', help='User name. Will use current logged in user by default')
    parser.add_argument('-p', help='Path to workspace. Will use current dir by default')
    parser.add_argument('-d', help='Docker image name. Will use delphyne:18.04 by default')
    args = parser.parse_args()
    user_name = pwd.getpwuid(os.getuid())[0]
    path = os.getcwd()
    docker_image_name = 'delphyne:18.04'
    if args.u is not None:
        user_name = args.u
    if args.p is not None:
        path = args.p
        if path[-1] is not '/':
            path += '/'
    if args.d is not None:
        docker_image_name = args.d
    shutil.copy2(os.path.dirname(os.path.realpath(__file__)) + '/Makefile', path)
    create_docker_file(user_name, path, docker_image_name)
