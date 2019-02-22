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
	git clone git@github.com:ToyotaResearchInstitute/delphyne-gui.git src/delphyne_gui
	sudo ./src/delphyne_gui/tools/install_prereqs.sh
	vcs import src < src/delphyne_gui/delphyne.repos
	sudo ./src/drake/setup/ubuntu/18.04/install_prereqs.sh

update_sources:
	vcs pull src/delphyne_gui
	vcs import src < src/delphyne_gui/delphyne.repos

update_dependencies:
	echo "**************************************************************************************"; \
	echo "                              Drake Dependencies"; \
	echo "**************************************************************************************"; \
	sudo ./src/drake/setup/ubuntu/18.04/install_prereqs.sh
	echo "**************************************************************************************"; \
	echo "                            Delphyne Dependencies"; \
	echo "**************************************************************************************"; \
	sudo ./src/delphyne_gui/tools/install_prereqs.sh

clean:
	rm -rf build
	rm -rf install
	cd src/drake && bazel clean

ignition:
	mkdir -p build
	mkdir -p install
	for i in ${IGNITION_REPOS}; do \
	  echo "**************************************************************************************"; \
	  echo "                                  $${i}"; \
	  echo "**************************************************************************************"; \
	  mkdir -p build/$${i}; \
	  cd build/$${i}; \
	  cmake ${CMAKE_FLAGS} -DCMAKE_CXX_STANDARD=11 -DCMAKE_INSTALL_PREFIX=../../install ../../src/$${i} || exit $$?; \
	  make -j$$(expr $$(getconf _NPROCESSORS_ONLN) - 2) VERBOSE=1 install || exit $$?; \
	  cd ../..; \
	done

drake:
	cd src/drake && bazel run ${BAZEL_FLAGS} //:install `pwd`/../../install || exit $$?

delphyne:
	for i in ${DELPHYNE_REPOS}; do \
	  mkdir -p build/$${i}; \
	  cd build/$${i}; \
	  cmake ${CMAKE_FLAGS} -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=../../install ../../src/$${i} || exit $$?; \
	  make -j$$(expr $$(getconf _NPROCESSORS_ONLN) - 2) install || exit $$?; \
	  cd ../..; \
	done

tests:
	cd build/delphyne && CTEST_OUTPUT_ON_FAILURE=1 make test

d:
	mkdir -p build/delphyne; 
	cd build/delphyne && cmake ${CMAKE_FLAGS} -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=../../install ../../src/delphyne || exit $$?; 
	cd build/delphyne && make VERBOSE=1 -j$$(expr $$(getconf _NPROCESSORS_ONLN) - 1)  install || exit $$?; 

dg:
	mkdir -p build/delphyne_gui
	cd build/delphyne_gui && cmake ${CMAKE_FLAGS} -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=../../install ../../src/delphyne_gui || exit $$? 
	cd build/delphyne_gui && make -j$$(expr $$(getconf _NPROCESSORS_ONLN) - 1) install || exit $$? 


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
