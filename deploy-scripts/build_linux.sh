#!/bin/sh
SRCDIR=$(readlink -f $1)
rm -rf $SRCDIR/deploy
podman run --rm -it -v $SRCDIR:/root/ctbot-viewer --security-opt=label=type:container_runtime_t ctbot-viewer-deploy
mv $SRCDIR/deploy/ctbot-viewer.tbz2 .
~/Applications/appimagetool-x86_64.AppImage $SRCDIR/deploy 
rm -rf $SRCDIR/deploy/*
mv ctbot-viewer.tbz2 $SRCDIR/deploy/
mv ct-Bot_Viewer-x86_64.AppImage $SRCDIR/deploy/ 

