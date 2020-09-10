FROM ubuntu:18.04

RUN apt-get update -y
RUN apt-get install -y build-essential
RUN apt-get install -y wget
RUN apt-get install -y git
RUN apt-get install -y cmake
RUN apt-get install -y gcc g++
RUN apt-get install -y make
RUN apt-get install -y python3 python3-pip python3-venv python3-wheel
RUN apt-get install -y pkgconf
RUN apt-get install -y libblas-dev liblapack-dev libopenblas-dev
RUN apt-get install -y libavformat-dev libavcodec-dev libswscale-dev libswresample-dev libavutil-dev
RUN apt-get install -y libjsoncpp-dev

RUN useradd -m -g users -s /bin/bash user
USER user

ENTRYPOINT /bin/bash
