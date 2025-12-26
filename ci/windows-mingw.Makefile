mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
project_dir := $(abspath $(mkfile_path)/..)

scratch_dir := $(project_dir)/scratch/windows-mingw
build_dir := $(scratch_dir)/build
dist_dir := $(scratch_dir)/dist
download_dir := $(scratch_dir)/download
deps_dir := $(scratch_dir)/deps

project_install_prefix := /opt/neuon
deps_install_prefix := /

project_install_prefix_winpath := $(shell cygpath -w $(project_install_prefix))
deps_install_prefix_winpath := $(shell cygpath -w $(deps_install_prefix))

project_dir_winpath := $(shell cygpath -w $(project_dir))
build_dir_winpath := $(shell cygpath -w $(build_dir))
dist_dir_winpath := $(shell cygpath -w $(dist_dir))
download_dir_winpath := $(shell cygpath -w $(download_dir))
deps_dir_winpath := $(shell cygpath -w $(deps_dir))

mingw_prefix_winpath := $(shell cygpath -w $$MINGW_PREFIX)

FORCE:

clean:
	-rm -rf $(build_dir) $(dist_dir) $(download_dir) $(deps_dir)

folders:
	mkdir -p $(build_dir) $(dist_dir) $(download_dir) $(deps_dir)

$(download_dir)/semver.marker:
	git clone https://gitlab.com/sergeyrachev/semver.git $(download_dir)/semver
	touch $(download_dir)/semver.marker

semver: $(download_dir)/semver.marker;

$(download_dir)/aquila.marker:
	git clone --depth 1 --branch mfec https://gitlab.com/sergeyrachev/aquila.git $(download_dir)/aquila
	touch $(download_dir)/aquila.marker

$(build_dir)/aquila.marker:
	cmake -Wno-dev --trace-expand --trace-redirect="$(build_dir_winpath)/aquila.log" \
		-DCMAKE_INSTALL_PREFIX="$(deps_dir_winpath)" \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_PREFIX_PATH="$(mingw_prefix_winpath)" \
		-DCMAKE_POSITION_INDEPENDENT_CODE=On \
		-B "$(build_dir_winpath)/aquila" \
		-S "$(download_dir_winpath)/aquila"
	cmake --build "$(build_dir_winpath)/aquila" --config Release --target install
	touch $(build_dir)/aquila.marker

aquila: $(download_dir)/aquila.marker $(build_dir)/aquila.marker;

$(download_dir)/tensorflow.marker:
	wget -O $(download_dir)/libtensorflow-cpu-windows-x86_64-1.15.0.zip https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-windows-x86_64-1.15.0.zip
	touch $(download_dir)/tensorflow.marker

$(build_dir)/tensorflow.marker:
	7z x -y -o$(download_dir)/libtensorflow-cpu-windows-x86_64-1.15.0 $(download_dir)/libtensorflow-cpu-windows-x86_64-1.15.0.zip
	cp -r $(download_dir)/libtensorflow-cpu-windows-x86_64-1.15.0/* $(deps_dir)/
	touch $(build_dir)/tensorflow.marker

tensorflow: $(download_dir)/tensorflow.marker $(build_dir)/tensorflow.marker;

get-packages:
	pacman -Sy --noconfirm \
		$$MINGW_PACKAGE_PREFIX-curl \
		$$MINGW_PACKAGE_PREFIX-boost \
		$$MINGW_PACKAGE_PREFIX-openssl \
		$$MINGW_PACKAGE_PREFIX-pkgconf \
		$$MINGW_PACKAGE_PREFIX-cmake \
		$$MINGW_PACKAGE_PREFIX-toolchain \
		$$MINGW_PACKAGE_PREFIX-7zip \
		$$MINGW_PACKAGE_PREFIX-wget \
		$$MINGW_PACKAGE_PREFIX-ninja \
		$$MINGW_PACKAGE_PREFIX-dlib \
		$$MINGW_PACKAGE_PREFIX-gtest \
		$$MINGW_PACKAGE_PREFIX-spdlog \
		$$MINGW_PACKAGE_PREFIX-ffmpeg

battery: folders tensorflow aquila;

version: folders semver FORCE
	(cd $(build_dir) && $(download_dir)/semver/semver.sh)

$(build_dir)/SEMVER_TIMESTAMP: version;

artifact: folders $(build_dir)/SEMVER_TIMESTAMP
	SEMVER_MAJOR=$(shell cat $(build_dir)/SEMVER_MAJOR) \
	SEMVER_MINOR=$(shell cat $(build_dir)/SEMVER_MINOR) \
	SEMVER_PATCH=$(shell cat $(build_dir)/SEMVER_PATCH) \
	SEMVER_TWEAK=$(shell cat $(build_dir)/SEMVER_TWEAK) \
	SEMVER_BUILD=$(shell cat $(build_dir)/SEMVER_BUILD) \
	SEMVER_REVISION=$(shell cat $(build_dir)/SEMVER_REVISION) \
	SEMVER_PHASE=$(shell cat $(build_dir)/SEMVER_PHASE) \
	SEMVER_CI=$(shell cat $(build_dir)/SEMVER_CI) \
	SEMVER_TIMESTAMP=$(shell cat $(build_dir)/SEMVER_TIMESTAMP) \
	SEMVER_MNEMONIC=$(shell cat $(build_dir)/SEMVER_MNEMONIC) \
	SEMVER_DIRTY=$(shell cat $(build_dir)/SEMVER_DIRTY) \
	cmake -Wno-dev --trace-expand --trace-redirect="$(build_dir_winpath)/neuon.log" \
		-DCMAKE_INSTALL_PREFIX="$(dist_dir_winpath)" \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_PREFIX_PATH="$(deps_dir_winpath)" \
		-DBoost_USE_STATIC_LIBS=OFF \
		-B "$(build_dir_winpath)/neuon" \
		-S "$(project_dir_winpath)"
	cmake --build "$(build_dir_winpath)/neuon" --target install
	ctest --test-dir "$(build_dir_winpath)/neuon"
	(cd $(build_dir)/neuon && cpack --trace-expand -G "ZIP")
