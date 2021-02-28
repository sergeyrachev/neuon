if not exist "prereq/deps/ffmpeg.marker " (
    mkdir prereq/deps
    pushd prereq

    wget -P ./ https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2020-11-10-12-44/ffmpeg-n4.3.1-25-g1936413eda-win64-gpl-shared-4.3.zip
    7z x -y ffmpeg-n4.3.1-25-g1936413eda-win64-gpl-shared-4.3.zip
    mv ffmpeg-n4.3.1-25-g1936413eda-win64-gpl-shared-4.3 deps/ffmpeg

    copy /y nul deps/ffmpeg.marker
    popd
)

if not exist "prereq/deps/tensorflow.marker " (
    mkdir prereq/deps
    pushd prereq

    wget -P ./ https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-windows-x86_64-1.15.0.zip
    7z x -y -odeps/tensorflow libtensorflow-cpu-windows-x86_64-1.15.0.zip

    copy /y nul deps/tensorflow.marker
    popd
)

if not exist "prereq/deps/boost.marker " (
    mkdir prereq/deps
    pushd prereq

    wget -P ./ https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.zip
    7z x -y boost_1_72_0.zip

    pushd boost_1_72_0
    call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    bootstrap.bat
    b2 -j4 link=static threading=multi variant=release toolset=msvc-16.0 --build-type=minimal runtime-link=static --prefix=../deps/boost --build-dir=./boost.build/ --layout=tagged architecture=x86 address-model=64 install
    popd

    copy /y nul deps/boost.marker
    popd
)

if not exist "prereq/build.env" (
    mkdir prereq
    pushd prereq

    python3 -m venv build.env
    call build.env/bin/activate
    python3 -m pip install --upgrade setuptools wheel

    popd
)

if not exist "prereq/deps/depends.marker" (
    mkdir prereq/deps
    pushd prereq

    git clone https://github.com/davisking/dlib.git
    cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_POSITION_INDEPENDENT_CODE=On -DCMAKE_INSTALL_PREFIX=./deps/dlib -DCMAKE_BUILD_TYPE=Release -DUSE_SSE2_INSTRUCTIONS=1 -DUSE_SSE4_INSTRUCTIONS=1 -DUSE_AVX_INSTRUCTIONS=1 -DDLIB_NO_GUI_SUPPORT=1 -Sdlib -Bdlib.build
    cmake --build dlib.build  --config Release --target install

    git clone https://github.com/sergeyrachev/aquila.git -b mfec
    cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_POSITION_INDEPENDENT_CODE=On -DCMAKE_INSTALL_PREFIX=./deps/aquila -DCMAKE_BUILD_TYPE=Release -Saquila -Baquila.build
    cmake --build aquila.build  --config Release --target install

    git clone https://github.com/gabime/spdlog.git
    cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_POSITION_INDEPENDENT_CODE=On -DCMAKE_INSTALL_PREFIX=./deps/spdlog -DCMAKE_BUILD_TYPE=Release -Sspdlog -Bspdlog.build
    cmake --build spdlog.build  --config Release --target install

    copy /y nul  deps/depends.marker
)
