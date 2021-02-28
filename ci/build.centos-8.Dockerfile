FROM centos:8

RUN yum -y clean all
RUN yum -y install epel-release

RUN yum -y clean all
RUN yum -y install wget
RUN yum -y install git
RUN yum -y install gcc gcc-c++
RUN yum -y install make
RUN yum -y install which
RUN yum -y install rpm-build
RUN yum -y install python3 python3-pip
RUN yum -y install pkgconfig
RUN yum -y install jsoncpp-devel
RUN yum -y install --enablerepo=PowerTools blas-devel lapack-devel atlas-devel

RUN rpm -Uvh https://download1.rpmfusion.org/free/el/rpmfusion-free-release-8.noarch.rpm
RUN yum -y install --enablerepo=PowerTools ffmpeg-devel ffmpeg-libs

RUN useradd -m -g users -s /bin/bash user
USER user

ENTRYPOINT /bin/bash
