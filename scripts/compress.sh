#!/bin/sh
if ! [ `which upx` ]; then
  wget https://github.com/upx/upx/releases/download/v3.94/upx-3.94-amd64_linux.tar.xz
  tar xf ./upx-3.94-amd64_linux.tar.xz
  upx-3.94-amd64_linux/upx --best --ultra-brute $1
else
  upx --best --ultra-brute $1
fi
