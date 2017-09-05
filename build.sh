set -e

gcc -s -Os -flto -std=gnu99 -Wall -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections nanovg/nanovg.c src/lvg.c src/svgb.c src/lunzip.c \
audio/sdl/lvg_audio.c \
swf/swf.c swf/adpcm.c swf/avm1.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c mp3/minimp3.c \
render/*.c \
video/ffmpeg/ffmpeg_dec.c \
-Lscripting/tcc -I. -Isrc -Iscripting/tcc -Inanovg -Iswf/swftools/lib -Imp3 -DNDEBUG -D_GNU_SOURCE -o lvg -Wl,-Map=lvg.map -lm -lglfw -lGL -ltcc2 -ldl -lSDL2 -lavcodec -lavutil
objcopy --remove-section=.comment --remove-section=.note* --remove-section=.gnu.version --remove-section=.eh_frame* --remove-section=.jcr ./lvg
scripts/compress.sh ./lvg
zip -9 -u lvg_linux.zip lvg
