set -e

. build_src.sh

gcc -Os -flto -std=gnu99 -Wall -fno-stack-protector -ffunction-sections -fdata-sections -Wl,-dead_strip $SRC \
-I/usr/local/include -L/usr/local/lib \
-I. -Isrc -Inanovg -Iswf/swftools/lib -DNDEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE -o lvg_macos -lm -ldl -lSDL2 -lavcodec -lavutil
upx --best --ultra-brute ./lvg_macos
if [ "$TRAVIS" = "true" ]; then
    zip -9 -u lvg_macos.zip lvg_macos
fi
