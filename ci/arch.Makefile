mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
project_dir := $(abspath $(mkfile_path)/..)

scratch_dir := $(project_dir)/scratch/archlinux
build_dir := $(scratch_dir)/build
dist_dir := $(scratch_dir)/dist
download_dir := $(scratch_dir)/download
deps_dir := $(scratch_dir)/deps

project_install_prefix := /opt/neuon
deps_install_prefix := $(scratch_dir)/deps

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
	mkdir -p $(build_dir)/aquila
	cmake -Wno-dev --trace-expand --trace-redirect="$(build_dir)/aquila.log" \
		-DBUILD_SHARED_LIBS=Off \
		-DCMAKE_INSTALL_PREFIX="$(deps_install_prefix)" \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_POSITION_INDEPENDENT_CODE=On \
		-B "$(build_dir)/aquila" \
		-S "$(download_dir)/aquila"
	cmake --build "$(build_dir)/aquila" --config Release --target install
	touch $(build_dir)/aquila.marker

aquila: $(download_dir)/aquila.marker $(build_dir)/aquila.marker;

$(download_dir)/dlib.marker:
	git clone https://github.com/davisking/dlib.git $(download_dir)/dlib
	touch $(download_dir)/dlib.marker

$(build_dir)/dlib.marker:
	cmake -Wno-dev --trace-expand --trace-redirect="$(build_dir)/dlib.log" \
		-DBUILD_SHARED_LIBS=Off \
		-DCMAKE_POSITION_INDEPENDENT_CODE=On \
		-DCMAKE_INSTALL_PREFIX="$(deps_install_prefix)" \
		-DCMAKE_BUILD_TYPE=Release \
		-DUSE_SSE2_INSTRUCTIONS=1 \
		-DUSE_SSE4_INSTRUCTIONS=1 \
		-DUSE_AVX_INSTRUCTIONS=1 \
		-DDLIB_NO_GUI_SUPPORT=1 \
		-DDLIB_JXL_SUPPORT=0 \
		-B "$(build_dir)/dlib" \
		-S "$(download_dir)/dlib"
	cmake --build "$(build_dir)/dlib" --config Release --target install
	touch $(build_dir)/dlib.marker

dlib: $(download_dir)/dlib.marker $(build_dir)/dlib.marker;

get-packages:
	pacman -Sy --noconfirm \
	git \
	make \
	gcc \
	python \
	python-pip \
	blas \
	lapack \
	cblas \
	jsoncpp \
	libffi \
	curl \
	boost \
	openssl \
	pkgconf \
	cmake \
	wget \
	ninja \
	gtest \
	spdlog \
	ffmpeg \
	tensorflow

battery: aquila dlib;

version: semver FORCE
	(cd $(build_dir) && bash $(download_dir)/semver/semver.sh)

$(build_dir)/SEMVER_TIMESTAMP: version;

artifact: $(build_dir)/SEMVER_TIMESTAMP
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
	cmake -Wno-dev --trace-expand --trace-redirect="$(build_dir)/neuon.log" \
		-DCMAKE_INSTALL_PREFIX="$(project_install_prefix)" \
		-DCMAKE_PREFIX_PATH="$(deps_dir)" \
		-DCMAKE_BUILD_TYPE=Release \
		-DBoost_USE_STATIC_LIBS=OFF \
		-B "$(build_dir)/neuon" \
		-S "$(project_dir)"
	cmake --build "$(build_dir)/neuon" --target install -- DESTDIR="$(dist_dir)"
	ctest --test-dir "$(build_dir)/neuon"
	(cd $(build_dir)/neuon && cpack --trace-expand -G "ZIP;TGZ")
