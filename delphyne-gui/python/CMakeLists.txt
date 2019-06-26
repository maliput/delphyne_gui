# Pin down python 3.6
find_package(PythonLibs 3.6 REQUIRED)
find_package(PythonInterp 3.6 REQUIRED)

# Unfortunately no easy way to get site-packages / dist-packages
set(PYTHON_INSTALL_DIR lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)

add_subdirectory(toolkit)
add_subdirectory(scripts)

message(STATUS ${PYTHON_INSTALL_DIR})

install(
  FILES
    launcher.py
    utilities.py
  DESTINATION
    ${PYTHON_INSTALL_DIR}/delphyne_gui
)
