#!/bin/sh
SRCDIR=$(readlink -f $1)
rm -rf $SRCDIR/deploy
podman run --rm -it -v $SRCDIR:/root/ctbot-viewer --security-opt=label=type:container_runtime_t ctbot-viewer-deploy
