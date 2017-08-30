#!/bin/sh
apt-get update -qq
apt-get install -qq -y libglfw3-dev libsdl2-dev libavcodec-dev upx-ucl
cd /lvg
./build.sh
