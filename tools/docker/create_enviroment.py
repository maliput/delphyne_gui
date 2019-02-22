#!/usr/bin/env python

import argparse
import create_docker_files
import os
import pwd
import shutil
import subprocess

BASH_FILE_NAME = 'prepare_environment'

BASH_SCRIPT = """#! /bin/bash
echo 'Building {DOCKER_IMAGE_NAME}'
docker build -t {DOCKER_IMAGE_NAME} .;
sudo mkdir -p /var/cache/docker;
cd {WORKSPACE_PATH};
{WORKSPACE_PATH}start_ws.sh;
docker exec -ti `docker ps -q --filter ancestor={DOCKER_IMAGE_NAME}` sh -c "wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -";
docker exec -ti `docker ps -q --filter ancestor={DOCKER_IMAGE_NAME}` sh -c "cd {WORKSPACE_PATH} && make workspace";
docker commit `docker ps -q | head -n 1` {DOCKER_IMAGE_NAME};
docker stop delphyne;
"""

def create_bash_profile(workspace_path, docker_file_name):
    with open(os.path.join(path, BASH_FILE_NAME), 'w') as bash_file:
        script = BASH_SCRIPT.replace('{WORKSPACE_PATH}', path)
        bash_file.write(script.replace('{DOCKER_IMAGE_NAME}', docker_file_name))
        os.chmod(path + BASH_FILE_NAME, 0775)
    shell_script_data = ''
    workspace_shell_path = os.path.join(path, 'start_ws.sh')
    with open(workspace_shell_path, 'r') as shell_script:
        shell_script_data = shell_script.read()
    shell_script_data = shell_script_data.replace('-it', '-idt')
    with open(workspace_shell_path, 'w') as shell_script:
        shell_script.write(shell_script_data)
    shell_script_data = shell_script_data.replace('-idt', '-it')
    subprocess.call(os.path.join(path, BASH_FILE_NAME), shell=True)
    with open(workspace_shell_path, 'w') as shell_script:
        shell_script.write(shell_script_data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create development enviroment.')
    parser.add_argument('-p', help='Path to workspace. Will use current dir by default')
    parser.add_argument('-d', help='Docker image name. Will use delphyne:18.04 by default')
    args = parser.parse_args()
    path = os.getcwd()
    docker_file_name = 'delphyne:18.04'
    user_name = pwd.getpwuid(os.getuid())[0]
    if args.p is not None:
        path = args.p
        if path[-1] is not '/':
            path += '/'
    if args.d is not None:
        docker_file_name = args.d
    shutil.copy2(os.path.dirname(os.path.realpath(__file__)) + '/Makefile', path)
    create_docker_files.create_docker_file(user_name, path, docker_file_name)
    os.chdir(path)
    create_bash_profile(path, docker_file_name)
