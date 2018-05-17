#!/bin/sh

set -ex

apt-get update -qq
apt-get install -qq -y git mercurial cmake meson yasm libglfw3-dev libsdl2-dev libavcodec-dev mingw-w64 zip wget
cd /lvg
bash -c ./build.sh
bash -c ./build_win.sh
