gcc -Os -flto -std=gnu99 -Wall -fno-stack-protector nanovg/nanovg.c adpcm.c lvg_audio.c lvg.c svgb.c swf.c lunzip.c \
swftools/lib/*.c swftools/lib/modules/*.c swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
video/ffmpeg/ffmpeg_dec.c \
-I/usr/local/include -L/usr/local/lib \
-L. -I. -Inanovg -Iswftools/lib -Imp3 -DNDEBUG -D_GNU_SOURCE -o lvg_macos -lm -lglfw3 -ltcc -ldl -lSDL2 -lavcodec -lavutil \
-framework OpenGL `pkg-config --static --libs glfw3`
upx -9 ./lvg_macos
zip -9 lvg_macos.zip lvg_macos
