set -e

scripts/build-sdl.sh linux
scripts/build-ffmpeg-pc.sh linux
. build_src.sh

CFLAGS="-s -Os -flto -std=gnu99 -Wall -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections $SRC \
-Isrc -Inanovg -Iswf/swftools/lib \
-LSDL/build-linux -ISDL/include -I. \
-Ivideo/ffmpeg/FFmpeg -Ivideo/ffmpeg/FFmpeg/build-linux -Lvideo/ffmpeg/FFmpeg/build-linux/libavcodec -Lvideo/ffmpeg/FFmpeg/build-linux/libavutil \
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
