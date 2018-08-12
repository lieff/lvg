#!/bin/sh
set -e
UPXVER=3.94
if ! [ `which upx` ]; then
  if [ ! -d "upx-${UPXVER}-amd64_linux" ]; then
    wget https://github.com/upx/upx/releases/download/v${UPXVER}/upx-${UPXVER}-amd64_linux.tar.xz
    tar xf ./upx-${UPXVER}-amd64_linux.tar.xz
    rm ./upx-${UPXVER}-amd64_linux.tar.xz
  fi
  upx-${UPXVER}-amd64_linux/upx --best --ultra-brute $1
else
  upx --best --ultra-brute $1
fi
