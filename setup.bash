BASEDIR=$( realpath $( pwd ) )

export PKG_CONFIG_PATH=$BASEDIR/install/lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=$BASEDIR/install/bin:$PATH
export LD_LIBRARY_PATH=$BASEDIR/install/lib:$LD_LIBRARY_PATH
export DELPHYNE_PACKAGE_PATH=$BASEDIR/src/delphyne-gui:$BASEDIR/src/delphyne:$DELPHYNE_PACKAGE_PATH
