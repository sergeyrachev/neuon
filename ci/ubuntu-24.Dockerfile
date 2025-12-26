FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN apt-get install -y build-essential gcc g++ make wget

RUN --mount=type=bind,source=.,target=/tmp/project make -f /tmp/project/ci/ubuntu-24.Makefile get-packages

VOLUME /project
