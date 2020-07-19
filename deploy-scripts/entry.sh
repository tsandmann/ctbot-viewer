#!/bin/bash
source /opt/qt512/bin/qt512-env.sh

if [ ! -d "/root/ctbot-viewer/build" ]; then
    mkdir /root/ctbot-viewer/build
fi
cd /root/ctbot-viewer/build && rm -rf /root/ctbot-viewer/build/*

qmake -config release .. && make -j8

if [ ! -d "/root/ctbot-viewer/deploy" ]; then
    mkdir /root/ctbot-viewer/deploy
fi
cp /root/ctbot-viewer/build/ctbot-viewer /root/ctbot-viewer/deploy/
rm -rf /root/ctbot-viewer/build

/root/linuxdeployqt/bin/linuxdeployqt /root/ctbot-viewer/deploy/ctbot-viewer -qmldir=/root/ctbot-viewer -extra-plugins=iconengines,platformthemes/libqgtk3.so -always-overwrite -verbose=1 -no-translations -bundle-non-qt-libs

cd /root/ctbot-viewer/deploy
tar cvjf /root/ctbot-viewer.tbz2 *
rm -rf /root/ctbot-viewer/deploy/*
mv /root/ctbot-viewer.tbz2 /root/ctbot-viewer/deploy/

