gcc -Os -flto -std=gnu99 -Wall -fno-stack-protector -ffunction-sections -fdata-sections -Wl,-dead_strip nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/sdl/lvg_audio.c \
swf/swf.c swf/adpcm.c swf/avm1.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
video/ffmpeg/ffmpeg_dec.c \
-I/usr/local/include -L/usr/local/lib \
-I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib -Imp3 -DNDEBUG -D_GNU_SOURCE -o lvg_macos -lm -lglfw -ltcc -ldl -lSDL2 -lavcodec -lavutil \
-framework OpenGL `pkg-config --static --libs glfw3`
scripts/compress.sh ./lvg_macos
zip -9 -u lvg_macos.zip lvg_macos
