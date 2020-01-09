FROM ubuntu:xenial
RUN apt -y update
RUN apt install -y --allow-unauthenticated git vim build-essential wget patchelf software-properties-common && add-apt-repository -y ppa:beineri/opt-qt-5.12.6-xenial && apt update -qq && apt install -y libgl1-mesa-dev qt512-meta-full
RUN . /opt/qt512/bin/qt512-env.sh && cd /root && git clone https://github.com/probonopd/linuxdeployqt.git && cd linuxdeployqt && qmake -config release && make
COPY entry.sh /opt/
ENTRYPOINT /opt/entry.sh

