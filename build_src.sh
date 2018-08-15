
SRC="nanovg/nanovg.c src/lvg.c src/lunzip.c \
audio/*.c \
render/*.c \
render/jfes/*.c \
platform/*.c \
scripting/picoc/*.c -DUNIX_HOST \
scripting/picoc/cstdlib/*.c \
scripting/picoc/platform/platform_unix.c \
swf/*.c swf/swftools/lib/*.c swf/swftools/lib/modules/*.c swf/swftools/lib/as3/*.c \
video/ffmpeg/ffmpeg_dec.c"
