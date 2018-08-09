set -e

#if [ ! -d "glfw" ]; then
#  git clone --depth=1 https://github.com/glfw/glfw
#  mkdir glfw/build
#  cd glfw/build
#  cmake -DCMAKE_TOOLCHAIN_FILE=../../windows/mingw64.cmake -DGLFW_BUILD_EXAMPLES=0 -DGLFW_BUILD_TESTS=0 -DGLFW_BUILD_DOCS=0 ..
#  make
#  cd ../../
#fi

scripts/build-sdl.sh win
scripts/build-ffmpeg-win.sh

#if [ ! -d "tinycc" ]; then
#  git clone http://repo.or.cz/tinycc.git
#  cd tinycc
#  ./configure --cross-prefix=x86_64-w64-mingw32- --enable-static
#  make
#  cd ..
#fi

. build_src.sh
SRC="$SRC windows/mman.c"

x86_64-w64-mingw32-gcc -s -Os -flto -std=gnu99 -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections $SRC \
-I. -Isrc -Inanovg -Iswf/swftools/lib \
-ISDL/include -Ivideo/ffmpeg/FFmpeg -Ivideo/ffmpeg/FFmpeg/build-win \
-LSDL/build-win -Lvideo/ffmpeg/FFmpeg/build-win/libavcodec -Lvideo/ffmpeg/FFmpeg/build-win/libavutil \
-DNDEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE -o lvg_win.exe -Wl,-Map=lvg.map -lm -lopengl32 -lSDL2 -lavcodec -lavutil -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lbcrypt -lversion
scripts/compress.sh ./lvg_win.exe
if [ "$TRAVIS" = "true" ]; then
    zip -9 -u lvg_win.zip lvg_win.exe
fi
