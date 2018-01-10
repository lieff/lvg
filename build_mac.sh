set -e

gcc -Os -flto -std=gnu99 -Wall -fno-stack-protector -ffunction-sections -fdata-sections -Wl,-dead_strip nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/*.c \
render/*.c \
platform/*.c \
scripting/tcc/script_tcc.c \
swf/*.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c audio/mp3/minimp3.c \
video/ffmpeg/ffmpeg_dec.c \
-I/usr/local/include -L/usr/local/lib \
-I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib -DNDEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE -o lvg_macos -lm -lglfw -ltcc -ldl -lSDL2 -lavcodec -lavutil \
`pkg-config --static --libs glfw3`
upx --best --ultra-brute ./lvg_macos
if [ "$TRAVIS" = "true" ]; then
    zip -9 -u lvg_macos.zip lvg_macos
fi
