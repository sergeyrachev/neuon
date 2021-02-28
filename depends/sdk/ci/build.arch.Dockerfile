FROM archlinux:latest

RUN pacman --noconfirm -Syy git
RUN pacman --noconfirm -Syy make
RUN pacman --noconfirm -Syy gcc
RUN pacman --noconfirm -Syy cmake
RUN pacman --noconfirm -Syy ffmpeg
RUN pacman --noconfirm -Syy boost
RUN pacman --noconfirm -Syy python python-pip
RUN pacman --noconfirm -Syy gtest gmock
RUN pacman --noconfirm -Syy libffi
RUN pacman --noconfirm -Syy spdlog

RUN useradd -m -g users -s /bin/bash user
USER user
