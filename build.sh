set -e

. scripts/build-sdl.sh linux

gcc -s -Os -flto -std=gnu99 -Wall -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/*.c \
render/*.c \
platform/*.c \
scripting/tcc/script_tcc.c \
swf/*.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c audio/mp3_keyj/minimp3.c \
video/ffmpeg/ffmpeg_dec.c \
-LSDL/build-linux -ISDL/include -Lscripting/tcc -I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib \
-DNDEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE -o lvg -Wl,-Map=lvg.map -lm -ltcc2 -ldl -lSDL2 -lavcodec -lavutil -lpthread
objcopy --remove-section=.comment --remove-section=.note* --remove-section=.gnu.version --remove-section=.eh_frame* --remove-section=.jcr ./lvg
scripts/compress.sh ./lvg
if [ "$TRAVIS" = "true" ]; then
    zip -9 -u lvg_linux.zip lvg
    cd tests/swf
    ./test.sh
fi
