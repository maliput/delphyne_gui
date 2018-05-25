#!/bin/bash
# The setup script for Delphyne.  This script must be sourced from somewhere
# within the Delphyne workspace.

# First we determine where the script is, and based on that we assume the
# workspace is setup as specified in the README.md to find the workspace.
SETUP_BASH_SCRIPT=$( realpath ${BASH_SOURCE[0]} )
INSTALL_PREFIX=$( dirname $SETUP_BASH_SCRIPT )

# A function to add a new item (needle) to the passed environment variable, only
# if the new item is not already in the environment variable.  The function
# expects the elements of the environment variable to be separated via ':'.
add_if_not_in_var() {
    var=$1
    needle=$2

    # Get the current list of items in the variable using the "two-pass"
    # bash trick.  In the first pass (before the eval), $var (using the inner
    # dollar) is expanded to the name of the environment variable.  Then the
    # eval is run, and the outer dollar expands to the contents of the named
    # variable.  Note that we have to escape the outer one so it doesn't get
    # expanded by the first shell.
    current=$( eval echo \$$var )

    # Iterate over every item in the environment variable, looking to see
    # if the needle is already in there.  Note that we do it this way instead
    # of a substring because the substring can have some false positives (think
    # about the case where "foobar" is already there while trying to add
    # "foo").
    while IFS=':' read -ra ADDR; do
        found=0
        for i in "${ADDR[@]}"; do
            if [ "$i" == "$needle" ]; then
                found=1
                break
            fi
        done
    done <<< "$current"

    # If we didn't find the needle in the list, add it here.
    if [ $found -eq 0 ]; then
        if [ "$current" == "" ]; then
            export $var=$needle
        else
            export $var=$( eval echo \$$var ):$needle
        fi
    fi
}

# Path to various resources used by shell/python scripts and demos
#   e.g. configuration files, protos, road geometries
export DELPHYNE_RESOURCE_ROOT=${INSTALL_PREFIX}/share/delphyne

# Paths to resources used for rendering (see RenderWidget.hh/.cc)
#   e.g. sdf/urdf/obj/dae/textures
add_if_not_in_var DELPHYNE_PACKAGE_PATH $INSTALL_PREFIX/share/drake/automotive/models

# Path to descriptors of custom ignition messages (nec. for topic introspection)
add_if_not_in_var IGN_DESCRIPTOR_PATH $INSTALL_PREFIX/include/delphyne0/delphyne/protobuf

# Path to c++ libraries
add_if_not_in_var LD_LIBRARY_PATH $INSTALL_PREFIX/lib

# Path to executables
add_if_not_in_var PATH $INSTALL_PREFIX/bin

# Path to python modules
add_if_not_in_var PYTHONPATH $INSTALL_PREFIX/lib/python2.7/site-packages

# Path to python bindings
add_if_not_in_var PYTHONPATH $INSTALL_PREFIX/lib
