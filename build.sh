set -e

. scripts/build-sdl.sh linux

if [ ! -d "video/ffmpeg/FFmpeg" ]; then
  cd video/ffmpeg
  ./build_pc.sh
  cd ../..
fi

. build_src.sh

CFLAGS="-s -Os -flto -std=gnu99 -Wall -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections $SRC \
-LSDL/build-linux -ISDL/include -I. -Isrc -Inanovg -Iswf/swftools/lib \
-Ivideo/ffmpeg/FFmpeg -Lvideo/ffmpeg/FFmpeg/libavcodec -Lvideo/ffmpeg/FFmpeg/libavutil \
-DNDEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE"

gcc $CFLAGS -o lvg -Wl,-Map=lvg.map -lm -ldl -lSDL2 -lavcodec -lavutil -lpthread

if [ ! "$CIRCLECI" = "true" ]; then
objcopy --remove-section=.comment --remove-section=.note* --remove-section=.gnu.version --remove-section=.eh_frame* --remove-section=.jcr ./lvg
scripts/compress.sh ./lvg
if [ "$TRAVIS" = "true" ]; then
    zip -9 -u lvg_linux.zip lvg
    cd tests/swf
    ./test.sh
fi
fi
