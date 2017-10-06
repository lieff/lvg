#ifdef __MINGW32__
#include <glad/glad.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <render/render_nvpr_apple.h>
#else
#ifndef __MINGW32__
#include <GL/gl.h>
#endif
#include <GL/glext.h>
#endif
