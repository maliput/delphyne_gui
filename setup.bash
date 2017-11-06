BASEDIR=$( realpath $( pwd ) )

export PKG_CONFIG_PATH=$BASEDIR/install/lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=$BASEDIR/install/bin:$PATH
export DRAKE_INSTALL_PATH=$BASEDIR/install_drake
export LD_LIBRARY_PATH=$BASEDIR/install/lib:$DRAKE_INSTALL_PATH/lib:$LD_LIBRARY_PATH
export DELPHYNE_PACKAGE_PATH=$BASEDIR/src/drake/drake/automotive/models:$BASEDIR/src/delphyne/bridge:$BASEDIR/src/delphyne:$BASEDIR/src/delphyne-gui:$DELPHYNE_PACKAGE_PATH
