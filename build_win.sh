if [ ! -d "glfw" ]; then
  git clone --depth=1 https://github.com/glfw/glfw
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

if [ ! -d "video/ffmpeg/FFmpeg" ]; then
  cd video/ffmpeg
  ./build_win.sh
  cd ../..
fi

#if [ ! -d "tinycc" ]; then
#  git clone http://repo.or.cz/tinycc.git
#  cd tinycc
#  ./configure --cross-prefix=x86_64-w64-mingw32- --enable-static
#  make
#  cd ..
#fi

x86_64-w64-mingw32-gcc -s -Os -flto -std=gnu99 -fno-stack-protector nanovg/nanovg.c adpcm.c lvg_audio.c lvg.c svgb.c swf.c lunzip.c \
swftools/lib/*.c swftools/lib/modules/*.c swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
windows/mman.c \
glfw/deps/glad.c \
video/ffmpeg/ffmpeg_dec.c \
-Iglfw/include -Iglfw/deps -ISDL/include -Ivideo/ffmpeg/FFmpeg -Lglfw/build/src -LSDL/build -Lvideo/ffmpeg/FFmpeg/libavcodec -Lvideo/ffmpeg/FFmpeg/libavutil \
-L. -I. -Inanovg -Iswftools/lib -Imp3 -DNDEBUG -D_GNU_SOURCE -o lvg_win.exe -lm -lopengl32 -lglfw3 -lSDL2-static -lavcodec -lavutil -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion
upx -9 ./lvg_win.exe
zip -9 -u lvg_win.zip lvg_win.exe
