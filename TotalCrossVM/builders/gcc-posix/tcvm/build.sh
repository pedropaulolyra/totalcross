#!/bin/bash
BASEDIR=$(dirname $0)
WORKDIR=$(cd $BASEDIR; pwd)

sudo rm -Rf bin

# execute docker run
sudo docker run \
-v $WORKDIR:/build \
-v $WORKDIR/../../../deps/skia:/skia \
-v $WORKDIR/../../../src:/src \
-e SRCDIR=/../../../src \
-t totalcross/amd64-cross-compile:bionic \
bash -c "make  -j$(($(nproc) + 2)) -f /build/Makefile"

# bash -c "make  -j$(($(nproc) + 2)) -f ${WORKDIR}/Makefile"