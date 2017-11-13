# The setup script for Delphyne.  This script must be sourced from somewhere
# within the Delphyne workspace.

# First we determine where the script is, and based on that we assume the
# workspace is setup as specified in the README.md to find the workspace.
DELPHYNE_GUI_SCRIPT=$( realpath ${BASH_SOURCE[0]} )
DELPHYNE_GUI_DIR=$( dirname $DELPHYNE_GUI_SCRIPT )
SRC_DIR=$( dirname $DELPHYNE_GUI_DIR )
WS_DIR=$( dirname $SRC_DIR )

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
         export $var=$needle:$( eval echo \$$var)
     fi
}

add_if_not_in_var PATH $WS_DIR/install/bin
add_if_not_in_var PKG_CONFIG_PATH $WS_DIR/install/lib/pkgconfig
add_if_not_in_var DRAKE_INSTALL_PATH $WS_DIR/install_drake
add_if_not_in_var LD_LIBRARY_PATH $DRAKE_INSTALL_PATH/lib
add_if_not_in_var LD_LIBRARY_PATH $WS_DIR/install/lib
add_if_not_in_var DELPHYNE_PACKAGE_PATH $WS_DIR/src/drake/drake/automotive/models
add_if_not_in_var DELPHYNE_PACKAGE_PATH $WS_DIR/src/delphyne/bridge
add_if_not_in_var DELPHYNE_PACKAGE_PATH $WS_DIR/src/delphyne
add_if_not_in_var DELPHYNE_PACKAGE_PATH $WS_DIR/src/delphyne_gui
add_if_not_in_var PYTHONPATH $WS_DIR/install/lib/python2.7/site-packages/launcher
