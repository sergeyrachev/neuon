FROM archlinux:latest

RUN pacman --noconfirm -Syy
RUN pacman --noconfirm -S git
RUN pacman --noconfirm -S make
RUN pacman --noconfirm -S gcc
RUN pacman --noconfirm -S cmake
RUN pacman --noconfirm -S ffmpeg
RUN pacman --noconfirm -S boost
RUN pacman --noconfirm -S python python-pip
RUN pacman --noconfirm -S pkgconf
RUN pacman --noconfirm -S blas lapack cblas
RUN pacman --noconfirm -S tensorflow
RUN pacman --noconfirm -S jsoncpp
RUN pacman --noconfirm -S ninja

RUN useradd -m -g users -s /bin/bash user
USER user

ENTRYPOINT /bin/bash
