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

sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116

apt update

echo ""
echo "***********************************************************************************"
echo "* Deb Installs"
echo "***********************************************************************************"
echo ""

apt install --no-install-recommends $(tr '\n' ' ' <<EOF
mercurial
cmake
pkg-config
python
python-pip
ruby-ronn
libprotoc-dev
libprotobuf-dev
protobuf-compiler
uuid-dev
libzmq3-dev
git
libogre-1.9-dev
libglew-dev
qtbase5-dev
libicu-dev
libboost-filesystem-dev
libfreeimage-dev
libtinyxml2-dev
libgts-dev
libavdevice-dev
python3-vcstool
mesa-utils
lcov
gcovr
libqt5multimedia5
libqwt-dev
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
