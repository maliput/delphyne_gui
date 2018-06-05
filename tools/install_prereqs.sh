#!/bin/bash
#
# Install development prerequisites for source distributions of Delphyne on
# Ubuntu 16.04.

me='The Delphyne source distribution prerequisite setup script'

die () {
    echo "$@" 1>&2
    trap : EXIT  # Disable line number reporting; the "$@" message is enough.
    exit 1
}

at_exit () {
    echo "${me} has experienced an error on line ${LINENO}" \
        "while running the command ${BASH_COMMAND}"
}

trap at_exit EXIT

[[ "${EUID}" -eq 0 ]] || die "${me} must run as root. Please use sudo."

sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable xenial main" > /etc/apt/sources.list.d/gazebo-stable.list'
apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys D2486D2DD83DB69272AFE98867170598AF249743
sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116

apt update

echo ""
echo "***********************************************************************************"
echo "* Deb Installs"
echo "***********************************************************************************"
echo ""

apt install --no-install-recommends $(tr '\n' ' ' <<EOF
cmake
coreutils
gcovr
git
lcov
libavdevice-dev
libboost-filesystem-dev
libfreeimage-dev
libglew-dev
libgts-dev
libicu-dev
libogre-1.9-dev
libprotobuf-dev
libprotoc-dev
libqt5multimedia5
libqwt-qt5-dev
libtinyxml2-dev
libzmq3-dev
mercurial
mesa-utils
pkg-config
protobuf-compiler
python
python-pip
python-setuptools
python3-vcstool
qtbase5-dev
ruby-ronn
uuid-dev
EOF
)

echo ""
echo "***********************************************************************************"
echo "* Local Pip Installs for $SUDO_USER"
echo "***********************************************************************************"
echo ""

sudo -u $SUDO_USER pip install pydoc-markdown --user

echo ""
echo "***********************************************************************************"
echo "* Success"
echo "***********************************************************************************"
echo ""

trap : EXIT  # Disable exit reporting.
echo "I'm happy, you should be too"
echo ""
