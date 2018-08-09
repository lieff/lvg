set -e

scripts/build-sdl.sh linux
scripts/build-ffmpeg-pc.sh linux
. build_src.sh

gcc -g -O0 -Wall -fsanitize=address $SRC \
-LSDL/build-linux -ISDL/include -I. -Isrc -Inanovg -Iswf/swftools/lib \
-D_DEBUG -D_GNU_SOURCE -DLVG_INTERPOLATE -o lvg -lasan -lm -lglfw -lGL -ldl -lSDL2 -lavcodec -lavutil -lpthread
