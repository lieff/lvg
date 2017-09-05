#!/bin/sh
apt-get update -qq
apt-get install -qq -y git mercurial cmake yasm libglfw3-dev libsdl2-dev libavcodec-dev mingw-w64 zip wget
cd /lvg
./build.sh
./build_win.sh
