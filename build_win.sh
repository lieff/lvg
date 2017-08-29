if [ ! -d "glfw" ]; then
  git clone https://github.com/glfw/glfw
  mkdir glfw/build
  cd glfw/build
  cmake -DCMAKE_TOOLCHAIN_FILE=../../mingw64.cmake ..
  make
  cd ../../
fi

if [ ! -d "SDL" ]; then
  hg clone http://hg.libsdl.org/SDL
  mkdir SDL/build
  cd SDL/build
  cmake -DCMAKE_TOOLCHAIN_FILE=../../mingw64.cmake -DVIDEO_OPENGLES=0 ..
  make
  cd ../../
  mkdir SDL/include/SDL2
  mv SDL/include/*.h SDL/include/SDL2
fi

x86_64-w64-mingw32-gcc-win32 -s -Os -flto -std=gnu99 -fno-stack-protector nanovg/nanovg.c adpcm.c lvg_audio.c lvg.c svgb.c swf.c lunzip.c \
swftools/lib/*.c swftools/lib/modules/*.c swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
windows/mman.c \
video/ffmpeg/ffmpeg_dec.c \
-Iglfw/include -ISDL/include -Ivideo/ffmpeg/FFmpeg -Lglfw/build/src -LSDL/build -Lvideo/ffmpeg/FFmpeg/libavcodec -Lvideo/ffmpeg/FFmpeg/libavutil \
-L. -I. -Inanovg -Iswftools/lib -Imp3 -DNDEBUG -D_GNU_SOURCE -o lvg -lm -lopengl32 -lglfw3 -lSDL2-static -lavcodec -lavutil
