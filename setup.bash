# The setup script for Delphyne.  This script must be sourced from somewhere
# within the Delphyne workspace.

# First we determine where the script is, and based on that we assume the
# workspace is setup as specified in the README.md to find the workspace.
DELPHYNE_GUI_SCRIPT=$( realpath ${BASH_SOURCE[0]} )
DELPHYNE_GUI_DIR=$( dirname $DELPHYNE_GUI_SCRIPT )
SRC_DIR=$( dirname $DELPHYNE_GUI_DIR )
WS_DIR=$( dirname $SRC_DIR )

export PKG_CONFIG_PATH=$WS_DIR/install/lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=$WS_DIR/install/bin:$PATH
export DRAKE_INSTALL_PATH=$WS_DIR/install_drake
export LD_LIBRARY_PATH=$WS_DIR/install/lib:$DRAKE_INSTALL_PATH/lib:$LD_LIBRARY_PATH
export DELPHYNE_PACKAGE_PATH=$WS_DIR/src/drake/drake/automotive/models:$WS_DIR/src/delphyne/bridge:$WS_DIR/src/delphyne:$WS_DIR/src/delphyne_gui:$DELPHYNE_PACKAGE_PATH
