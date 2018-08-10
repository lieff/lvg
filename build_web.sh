set -e

. build_src.sh

. ~/Public/emsdk_portable/emsdk_set_env.sh
emcc -O3 $SRC \
-I. -Isrc -Inanovg -Iswf/swftools/lib \
-Ivideo/ffmpeg/FFmpeg -Ivideo/ffmpeg/FFmpeg/build-web -Lvideo/ffmpeg/FFmpeg/build-web/libavcodec -Lvideo/ffmpeg/FFmpeg/build-web/libavutil -lavcodec -lavutil \
-DNDEBUG -DLVG_INTERPOLATE -o lvg_player.html -s WASM=1 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s NO_EXIT_RUNTIME=1 -s USE_GLFW=3 -s USE_SDL=2 -s FULL_ES3=1 -s TOTAL_MEMORY=268435456
#emcc -Os test.c -o function.js -s EXPORTED_FUNCTIONS="['onFrame', 'onInit']" -s SIDE_MODULE=1
#EXPORTED_FUNCTIONS="['onFrame']" 
#-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE="['_int_sqrt']"

#http://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html
#https://github.com/kripken/emscripten/wiki/Linking

patch lvg_player.html lvg_player.patch
rm lvg_player.html.orig
rm lvg_player.html.rej