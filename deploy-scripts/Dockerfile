FROM ubuntu:xenial
ENV DEBIAN_FRONTEND=noninteractive
RUN apt -y update && apt -y upgrade
RUN apt install -y --allow-unauthenticated git vim build-essential wget patchelf software-properties-common && add-apt-repository -y ppa:beineri/opt-qt-5.12.6-xenial && apt update -qq && apt install -y libgl1-mesa-dev qt512-meta-full qt512charts-no-lgpl qt512datavis3d-no-lgpl qt512networkauth-no-lgpl qt512virtualkeyboard-no-lgpl qt512webglplugin-no-lgpl 
RUN . /opt/qt512/bin/qt512-env.sh && cd /root && git clone https://github.com/probonopd/linuxdeployqt.git && cd linuxdeployqt && qmake -config release && make
COPY entry.sh /opt/
ENTRYPOINT /opt/entry.sh

