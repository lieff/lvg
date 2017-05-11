. ~/Public/emsdk_portable/emsdk_set_env.sh
emcc -O3 nanovg/nanovg.c adpcm.c svg.c svgb.c swf.c lunzip.c swftools/lib/*.c swftools/lib/modules/*.c swftools/lib/action/*.c mp3/*.c -I. -Inanovg -Iswftools/lib -Imp3 -DNDEBUG -o lvg_player.html -s USE_GLFW=3 -s USE_SDL=2 -s FULL_ES3=1 -s TOTAL_MEMORY=268435456 -s MAIN_MODULE=1
#emcc -Os test.c -o function.js -s EXPORTED_FUNCTIONS="['onFrame', 'onInit']" -s SIDE_MODULE=1
#EXPORTED_FUNCTIONS="['onFrame']" 
#-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE="['_int_sqrt']"

#http://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html
#https://github.com/kripken/emscripten/wiki/Linking

patch lvg_player.html lvg_player.patch
