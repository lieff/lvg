
LVGShapeCollection *g_image;

void onInit()
{
    g_image = lvgLoadShape("main.svg");
}

void onFrame()
{
    platform_params *p = lvgGetParams();
    float scale = 1.0 + sin(p->time);
    if (p->mkeys)
    {
        double bounds[4];
        lvgGetShapeBounds(g_image, bounds);
        lvgScale(p->mx/(bounds[1] - bounds[0]), p->my/(bounds[3] - bounds[2]));
    } else
        lvgScale(scale, scale);
    lvgDrawShape(g_image);
}
