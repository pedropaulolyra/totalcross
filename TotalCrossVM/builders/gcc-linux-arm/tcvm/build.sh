#!/bin/bash -x
BASEDIR=$(dirname $0)
WORKDIR=$(cd $BASEDIR; pwd)

# reset multi-arch
sudo docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

# execute docker run
sudo docker run \
-v $WORKDIR:/build \
-v $WORKDIR/../../../deps/skia:/skia \
-v $WORKDIR/../../../src:/src \
-e SRCDIR=/../../../src \
-t totalcross/cross-compile \
bash -c "make  -j$(($(nproc) + 2)) -f /build/Makefile"
