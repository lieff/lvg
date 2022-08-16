
LVGShapeCollection *g_image;
int width;
int height;

void onInit()
{
    g_image = lvgShapeLoad("main.svg");
    double bounds[4];
    lvgShapeGetBounds(g_image, bounds);
    width = (bounds[2] - bounds[0]);
    height = (bounds[3] - bounds[1]);
}

void onFrame()
{
    lvgViewport(width, height);
    platform_params *p = lvgGetParams();
    float scale = 1.0 + sin(p->time);
    if (p->mkeys)
    {
        lvgScale(p->mx/width, p->my/height);
    } else
        lvgScale(scale, scale);
    lvgShapeDraw(g_image);
}
