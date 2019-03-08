IGNITION_BINARY='{IGNITION_BINARY_PLACEHOLDER}'
DRAKE_BINARY='{DRAKE_BINARY_PLACEHOLDER}'
IGNITION_REPOS="ign_cmake" "ign_math" "ign_tools" "ign_common" "ign_msgs" "ign_transport" "ign_rendering" "ign_gui"
DELPHYNE_REPOS="delphyne" "delphyne_gui"
CMAKE_FLAGS= #-DCMAKE_BUILD_TYPE=Release
BAZEL_FLAGS=-j $$(expr $$(getconf _NPROCESSORS_ONLN) - 2) --config=python3 #-c opt #--compiler=gcc-5

help:
	@echo ""
	@echo "Valid Targets:"
	@echo ""
	@echo "    workspace          : bootstrap the workspace"
	@echo "                         - installs system dependencies"
	@echo "                         - installs sources in ./src"
	@echo ""
	@echo "    update_sources     : update source repositories"
	@echo "    update_dependencies: update system dependencies"
	@echo ""
	@echo "    ignition           : build and install the ignition libraries"
	@echo "    drake              : build and install drake"
	@echo "    delphyne           : build and install delphyne and delphyne-gui"
	@echo ""
	@echo "    tests              : run delphyne tests with verbosity on failure"
	@echo ""
	@echo "    clean              : clean all cmake project build and install directories"
	@echo ""

workspace:
	rm -rf src
	mkdir -p src src/dsim-repos-index
	mkdir -p install
	git clone git@github.com:ToyotaResearchInstitute/dsim-repos-index.git src/dsim-repos-index
	sudo apt-get update && sudo apt install --no-install-recommends python3 python3-setuptools python3-vcstool git
	if [ -z ${IGNITION_BINARY} ] && [ ! -z ${DRAKE_BINARY} ]; then \
		sudo apt install --no-install-recommends mercurial; \
		vcs import src < ./src/dsim-repos-index/delphyne-ignition-no-drake.repos; \
	elif [ -z ${IGNITION_BINARY} ] && [ -z ${DRAKE_BINARY} ]; then \
		sudo apt install --no-install-recommends mercurial; \
		vcs import src < ./src/dsim-repos-index/delphyne-all.repos; \
	elif [ ! -z ${IGNITION_BINARY} ] && [ -z ${DRAKE_BINARY} ]; then \
		vcs import src < ./src/dsim-repos-index/delphyne-drake.repos; \
	else \
		vcs import src < ./src/dsim-repos-index/delphyne-ci.repos; \
	fi
	if [ ! -z ${DRAKE_BINARY} ]; then \
		sudo ./get_drake_binary; \
		yes | sudo /opt/drake/share/drake/setup/install_prereqs; \
	else \
		sudo ./src/drake/setup/ubuntu/18.04/install_prereqs.sh; \
	fi
	sudo ./src/delphyne_gui/tools/install_prereqs.sh

clean:
	rm -rf build
	rm -rf install
	cd src/drake && bazel clean

tests:
	cd build/delphyne && CTEST_OUTPUT_ON_FAILURE=1 make test


# Clean delphyne
cd:
	cd build/delphyne && make uninstall
	rm -rf build/delphyne
cdg:
	cd build/delphyne_gui && make uninstall
	rm -rf build/delphyne_gui

D="src/delphyne"
DG="src/delphyne_gui"

eclipse:
	tar -cvzf delphyne-eclipse.tar.gz $D/.cproject $D/.project $D/.pydevproject ${DG}/.cproject ${DG}/.project

.PHONY: tests clean eclipse
