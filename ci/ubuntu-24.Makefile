mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
project_dir := $(abspath $(mkfile_path)/..)

scratch_dir := $(project_dir)/scratch/ubuntu-24
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

aquila: folders $(download_dir)/aquila.marker $(build_dir)/aquila.marker;

$(download_dir)/tensorflow.marker:
	wget -O $(download_dir)/libtensorflow-cpu-linux-x86_64-1.15.0.tar.gz https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-1.15.0.tar.gz
	touch $(download_dir)/tensorflow.marker

$(build_dir)/tensorflow.marker:
	tar --one-top-level=$(download_dir)/libtensorflow-cpu-linux-x86_64-1.15.0 -xzf $(download_dir)/libtensorflow-cpu-linux-x86_64-1.15.0.tar.gz
	cp -r $(download_dir)/libtensorflow-cpu-linux-x86_64-1.15.0/* $(deps_dir)/
	touch $(build_dir)/tensorflow.marker

tensorflow: folders $(download_dir)/tensorflow.marker $(build_dir)/tensorflow.marker;

get-packages:
	DEBIAN_FRONTEND=noninteractive apt install -y \
	git \
	build-essential \
	g++ \
	python3 \
	python3-pip \
	python3-venv \
	python3-wheel \
	pkgconf \
	libblas-dev \
	liblapack-dev \
	libopenblas-dev \
	libavformat-dev \
	libavcodec-dev \
	libswscale-dev \
	libswresample-dev \
	libavutil-dev \
	libjsoncpp-dev \
	libboost-all-dev \
	cmake \
	wget \
	ninja-build \
	libgtest-dev \
	libgmock-dev \
	libspdlog-dev \
	libdlib-dev \
	libjpeg-dev \
	libcurl4-openssl-dev \
	ffmpeg


battery: aquila tensorflow;

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
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
		-DBoost_USE_STATIC_LIBS=OFF \
		-DNEUON_PREFIX_PATH="$(deps_dir)" \
		-B "$(build_dir)/neuon" \
		-S "$(project_dir)"
	cmake --build "$(build_dir)/neuon" --target install -- DESTDIR="$(dist_dir)"
	ctest --test-dir "$(build_dir)/neuon"
	(cd $(build_dir)/neuon && cpack --trace-expand -G "DEB;ZIP")

$(build_dir)/build-image.marker:
	docker build -t neuon-build-image -f ci/ubuntu-24.Dockerfile $(project_dir)
	touch $(build_dir)/build-image.marker

build-image: folders $(build_dir)/build-image.marker;

docker: folders build-image
	-rm -f $(build_dir)/BUILD_CONTAINER
	docker volume create project
	docker run --rm -v $(project_dir):/source -v project:/target alpine cp -r -v /source/. /target

	docker run --cidfile=$(build_dir)/BUILD_CONTAINER -v project:/project --workdir /project neuon-build-image make -f ci/ubuntu-24.Makefile battery artifact
	docker cp $$(cat $(build_dir)/BUILD_CONTAINER):/project/scratch/. $(project_dir)/scratch
	docker rm $$(cat $(build_dir)/BUILD_CONTAINER)
	docker volume rm project

demo:
	docker build -t neuon-demo:$$(cat $(build_dir)/SEMVER_VERSIONCORE) -t neuon-demo:latest -f $(project_dir)/scratch/ubuntu-24/build/neuon/examples/demo.Dockerfile $(project_dir)/scratch/ubuntu-24/build/neuon
	docker save neuon-demo:$$(cat $(build_dir)/SEMVER_VERSIONCORE) | gzip >$(build_dir)/neuon-demo-$$(cat $(build_dir)/SEMVER_VERSIONCORE).docker.tar.gz

demo-run:
	docker run -t --rm neuon-demo:$$(cat $(build_dir)/SEMVER_VERSIONCORE) /opt/neuon/share/neuon/examples/res/unsync.mp4
	docker run -t --rm neuon-demo:$$(cat $(build_dir)/SEMVER_VERSIONCORE) /opt/neuon/share/neuon/examples/res/sync.mpg
