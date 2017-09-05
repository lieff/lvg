gcc -g -O0 -Wall -fsanitize=address nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/sdl/lvg_audio.c \
swf/swf.c swf/adpcm.c swf/avm1.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
video/ffmpeg/ffmpeg_dec.c \
-Lscripting/tcc -I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib -Imp3 -D_DEBUG -D_GNU_SOURCE -o lvg -lasan -lm -lglfw -lGL -ltcc2 -ldl -lSDL2 -lavcodec -lavutil