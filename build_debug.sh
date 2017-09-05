gcc -g -O0 -Wall -fsanitize=address nanovg/nanovg.c lvg_audio.c lvg.c svgb.c lunzip.c \
swf/swf.c swf/adpcm.c swf/avm1.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
video/ffmpeg/ffmpeg_dec.c \
-Lscripting/tcc -I. -Iscripting/tcc -Inanovg -Iswf/swftools/lib -Imp3 -D_DEBUG -D_GNU_SOURCE -o lvg -lasan -lm -lglfw -lGL -ltcc2 -ldl -lSDL2 -lavcodec -lavutil