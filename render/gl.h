#ifdef __MINGW32__
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <render/render_nvpr_apple.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
