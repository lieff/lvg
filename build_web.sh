set -e

. ~/Public/emsdk_portable/emsdk_set_env.sh
emcc -O3 nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/sdl/lvg_audio.c \
scripting/tcc/script_tcc.c \
swf/*.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c mp3/minimp3.c \
render/common.c render/render_nanovg.c \
video/ffmpeg/ffmpeg_dec.c \
-I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib -Imp3 \
-Ivideo/ffmpeg/FFmpeg -Lvideo/ffmpeg/web -lavcodec -lavutil \
-DNDEBUG -o lvg_player.html -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s USE_GLFW=3 -s USE_SDL=2 -s FULL_ES3=1 -s TOTAL_MEMORY=268435456 -s MAIN_MODULE=1
#emcc -Os test.c -o function.js -s EXPORTED_FUNCTIONS="['onFrame', 'onInit']" -s SIDE_MODULE=1
#EXPORTED_FUNCTIONS="['onFrame']" 
#-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE="['_int_sqrt']"

#http://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html
#https://github.com/kripken/emscripten/wiki/Linking

patch lvg_player.html lvg_player.patch
rm lvg_player.html.orig
rm lvg_player.html.rej