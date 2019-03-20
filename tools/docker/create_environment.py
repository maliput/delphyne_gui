#!/usr/bin/env python

import argparse
import create_docker_files
import os
import pwd
import shutil
import subprocess

BASH_FILE_NAME = 'prepare_environment'
DRAKE_BINARY_BASH_SCRIPT_FILE_NAME = 'get_drake_binary'

BASH_SCRIPT = """#! /bin/bash
echo 'Building {DOCKER_IMAGE_NAME}'
docker build -t {DOCKER_IMAGE_NAME} .;
sudo mkdir -p /var/cache/docker;
cd {WORKSPACE_PATH};
{WORKSPACE_PATH}/start_ws.sh;
docker exec -ti `docker ps -q --filter ancestor={DOCKER_IMAGE_NAME}` sh -c "wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -";
docker exec -ti `docker ps -q --filter ancestor={DOCKER_IMAGE_NAME}` sh -c "cd {WORKSPACE_PATH} && make workspace";

docker commit `docker ps -q | head -n 1` {DOCKER_IMAGE_NAME};
docker stop delphyne;
"""

GET_DRAKE_BINARY_BASH_SCRIPT = """#! /bin/bash
RELEASE_URL="https://drake-packages.csail.mit.edu/drake/nightly/drake-latest-bionic.tar.gz"

cd {WORKSPACE_PATH}
mkdir -p install
mkdir -p /opt/drake
curl $RELEASE_URL | sudo tar xvz -C /opt/drake --strip 1
"""


def create_bash_profile(workspace_path, docker_file_name, use_drake_binary):
    with open(os.path.join(workspace_path, BASH_FILE_NAME), 'w') as bash_file:
        script = BASH_SCRIPT.replace('{WORKSPACE_PATH}', workspace_path)
        bash_file.write(script.replace('{DOCKER_IMAGE_NAME}', docker_file_name))
        os.chmod(os.path.join(workspace_path, BASH_FILE_NAME), 0775)
    workspace_shell_path = os.path.join(workspace_path, 'start_ws.sh')
    with open(workspace_shell_path, 'r') as shell_script:
        shell_script_data = shell_script.read()
    shell_script_data = shell_script_data.replace('-it', '-idt')
    with open(workspace_shell_path, 'w') as shell_script:
        shell_script.write(shell_script_data)
    shell_script_data = shell_script_data.replace('-idt', '-it')
    if use_drake_binary:
        get_drake_binary(workspace_path)
    subprocess.call(os.path.join(workspace_path, BASH_FILE_NAME), shell=True)
    with open(workspace_shell_path, 'w') as shell_script:
        shell_script.write(shell_script_data)

def get_drake_binary(workspace_path):
    with open(os.path.join(workspace_path,
        DRAKE_BINARY_BASH_SCRIPT_FILE_NAME), 'w') as bash_file:
        bash_file.write(GET_DRAKE_BINARY_BASH_SCRIPT.replace(
            '{WORKSPACE_PATH}', workspace_path))
        os.chmod(os.path.join(
            workspace_path, DRAKE_BINARY_BASH_SCRIPT_FILE_NAME), 0775)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create development enviroment.')
    parser.add_argument('-p', help='Path to workspace.'
        'Will use current dir by default', default=os.getcwd())
    parser.add_argument('-d', help='Docker image name.'
        'Will use delphyne:18.04 by default', default='delphyne:18.04')
    parser.add_argument('--use_ignition_source', help='Use ignition sources.'
        'Default value will be False.',
        action='store_true')
    parser.add_argument('--use_drake_binary', help='Use drake binary night release.'
        'Default value will be False.',
        action='store_true')
    args = parser.parse_args()
    path = os.path.normpath(args.p)
    docker_file_name = args.d
    user_name = pwd.getpwuid(os.getuid())[0]
    modify_makefile = args.use_ignition_source or not args.use_drake_binary
    if modify_makefile:
        data = ''
        with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Makefile'), 'r') as makefile:
            data = makefile.read()
            if args.use_ignition_source:
                data = data.replace('{IGNITION_BINARY_PLACEHOLDER}', '')
            if not args.use_drake_binary:
                data = data.replace('{DRAKE_BINARY_PLACEHOLDER}', '')
        with open(os.path.join(path, 'Makefile'), 'w') as makefile:
            makefile.write(data)
            makefile.truncate()
    else:
        shutil.copy2(os.path.dirname(os.path.realpath(__file__)) + '/Makefile', path)
    create_docker_files.create_docker_file(
        user_name, path, docker_file_name, not args.use_ignition_source, args.use_drake_binary)
    os.chdir(path)
    create_bash_profile(path, docker_file_name, args.use_drake_binary)
