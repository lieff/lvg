
if [ ! -d "SDL" ]; then
  hg clone http://hg.libsdl.org/SDL
fi
SDL_DIR=SDL/build-$1
if [ "$1" = "win" ]; then
TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=../../windows/mingw64.cmake
fi
if [ ! -d "$SDL_DIR" ]; then
  mkdir $SDL_DIR
  cd $SDL_DIR
  cmake $TOOLCHAIN -DVIDEO_OPENGLES=0 -DSDL_SHARED=OFF -DPTHREADS_SEM=OFF -DOSS=OFF -DSNDIO=OFF -DVIDEO_WAYLAND=OFF ..
  make
  cd ../../
fi
if [ ! -d "SDL/include/SDL2" ]; then
  mkdir SDL/include/SDL2
  cp SDL/include/*.h SDL/include/SDL2
fi
