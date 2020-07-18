#!/bin/sh
podman build -t ctbot-viewer-deploy -f ./Dockerfile --ulimit nofile=8192:8192

