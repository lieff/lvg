#include <math.h>
#include "nanovg.h"
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "nanosvg.h"

void *lvgGetFileContents(const char *fname, int *size);
void lvgFree(void *buf);
void lvgDrawSVG(NSVGimage *image);
NSVGimage *lvgLoadSVG(const char *file);

extern NVGcontext *vg;
extern int winWidth;
extern int winHeight;
extern int width;
extern int height;
extern int mkeys;
extern double mx;
extern double my;
extern double g_time;
