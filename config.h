#pragma once

#define VERSION_STR "0.0.5"
#define RENDER_NVPR 1
#define RENDER_NANOVG 1
#define AUDIO_SDL 1
#define VIDEO_FFMPEG 1
#if defined(__MINGW32__) || defined(EMSCRIPTEN)
#define SCRIPT_TCC 0
#define ENABLE_SCRIPT 0
#else
#define SCRIPT_TCC 1
#define ENABLE_SCRIPT 1
#endif
#define PLATFORM_SDL 1
//#define PLATFORM_GLFW 1
#define ENABLE_AUDIO 1
#define ENABLE_VIDEO 1
