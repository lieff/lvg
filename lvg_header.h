#ifdef EMSCRIPTEN
#include <stdint.h>
#endif
#include <math.h>

void *lvgGetFileContents(const char *fname, uint32_t *size);
void lvgFree(void *buf);
void lvgDrawSVG(NSVGimage *image);
NSVGimage *lvgLoadSVG(const char *file);
NSVGimage *lvgLoadSVGB(const char *file);

extern NVGcontext *vg;
extern NVGcolor g_bgColor;
extern int winWidth;
extern int winHeight;
extern int width;
extern int height;
extern int mkeys;
extern double mx;
extern double my;
extern double g_time;
