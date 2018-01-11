set -e

. scripts/build-sdl.sh linux

gcc -g -O0 -Wall -fsanitize=address nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/*.c \
render/*.c \
platform/*.c \
scripting/tcc/script_tcc.c \
swf/*.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c \
video/ffmpeg/ffmpeg_dec.c \
-LSDL/build-linux -ISDL/include -Lscripting/tcc -I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib \
-D_DEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE -o lvg -lasan -lm -lglfw -lGL -ltcc2 -ldl -lSDL2 -lavcodec -lavutil -lpthread
