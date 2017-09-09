#include "avm1.h"

ASMember g_stage_members[] =
{
    // properties
    { "align",        { { "" }, ASVAL_STRING } },
    { "displayState", { { "" }, ASVAL_STRING } },
    { "fullScreenSourceRect", { { .str = 0 }, ASVAL_CLASS } },
    { "height",       { { .ui32 = 150 }, ASVAL_INT } },
    { "scaleMode",    { { "showAll" }, ASVAL_STRING } },
    { "showMenu",     { { .boolean = 0 }, ASVAL_BOOL } },
    { "width",        { { .ui32 = 200 }, ASVAL_INT } },
    // events
    { "onFullScreen",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onResize",       { { .str = 0 }, ASVAL_FUNCTION } },
    // static methods
    { "addListener",    { { .str = 0 }, ASVAL_NATIVE_FN } },
    { "removeListener", { { .str = 0 }, ASVAL_NATIVE_FN } },
};

ASClass g_stage =
{
    "Stage", &g_stage_members[0], sizeof(g_stage_members)/sizeof(g_stage_members[0])
};

ASClass *g_classes[] =
{
    &g_stage
};

int g_num_classes = sizeof(g_classes)/sizeof(g_classes[0]);

void createEmptyMovieClip(void *ctx, uint8_t *a, int nargs);

ASMember g_movieclip_members[] =
{
    // properties
    { "_alpha",        { { "" }, ASVAL_STRING } }, //Number
    { "blendMode",     { { "" }, ASVAL_STRING } }, //Object
    { "cacheAsBitmap", { { "" }, ASVAL_STRING } }, //Boolean
    { "_currentframe", { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_droptarget",   { { "" }, ASVAL_STRING } }, //String [read-only]
    { "enabled",       { { "" }, ASVAL_STRING } }, //Boolean
    { "filters",       { { "" }, ASVAL_STRING } }, //Array
    { "focusEnabled",  { { "" }, ASVAL_STRING } }, //Boolean
    { "_focusrect",    { { "" }, ASVAL_STRING } }, //Boolean
    { "forceSmoothing", { { "" }, ASVAL_STRING } }, //Boolean
    { "_framesloaded", { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_height",       { { "" }, ASVAL_STRING } }, //Number
    { "_highquality",  { { "" }, ASVAL_STRING } }, //Number
    { "hitArea",       { { "" }, ASVAL_STRING } }, //Object
    { "_lockroot",     { { "" }, ASVAL_STRING } }, //Boolean
    { "menu",          { { "" }, ASVAL_STRING } }, //ContextMenu
    { "_name",         { { "" }, ASVAL_STRING } }, //String
    { "opaqueBackground", { { "" }, ASVAL_STRING } }, //Number
    { "_parent",       { { "" }, ASVAL_STRING } }, //MovieClip
    { "_quality",      { { "" }, ASVAL_STRING } }, //String
    { "_rotation",     { { "" }, ASVAL_STRING } }, //Number
    { "scale9Grid",    { { "" }, ASVAL_STRING } }, //Rectangle
    { "scrollRect",    { { "" }, ASVAL_STRING } }, //Object
    { "_soundbuftime", { { "" }, ASVAL_STRING } }, //Number
    { "tabChildren",   { { "" }, ASVAL_STRING } }, //Boolean
    { "tabEnabled",    { { "" }, ASVAL_STRING } }, //Boolean
    { "tabIndex",      { { "" }, ASVAL_STRING } }, //Number
    { "_target",       { { "" }, ASVAL_STRING } }, //String [read-only]
    { "_totalframes",  { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "trackAsMenu",   { { "" }, ASVAL_STRING } }, //Boolean
    { "transform" ,    { { "" }, ASVAL_STRING } }, //Transform
    { "_url",          { { "" }, ASVAL_STRING } }, //String [read-only]
    { "useHandCursor", { { "" }, ASVAL_STRING } }, //Boolean
    { "_visible",      { { "" }, ASVAL_STRING } }, //Boolean
    { "_width",        { { "" }, ASVAL_STRING } }, //Number
    { "_x",            { { "" }, ASVAL_STRING } }, //Number
    { "_xmouse",       { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_xscale",       { { "" }, ASVAL_STRING } }, //Number
    { "_y",            { { "" }, ASVAL_STRING } }, //Number
    { "_ymouse",       { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_yscale",       { { "" }, ASVAL_STRING } }, //Number
    // events
    { "onData",        { { .str = 0 }, ASVAL_FUNCTION } },
    { "onDragOut",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onDragOver",    { { .str = 0 }, ASVAL_FUNCTION } },
    { "onEnterFrame",  { { .str = 0 }, ASVAL_FUNCTION } },
    { "onKeyDown",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onKeyUp",       { { .str = 0 }, ASVAL_FUNCTION } },
    { "onKillFocus",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onLoad",        { { .str = 0 }, ASVAL_FUNCTION } },
    { "onMouseDown",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onMouseMove",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onMouseUp",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onPress",       { { .str = 0 }, ASVAL_FUNCTION } },
    { "onRelease",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onReleaseOutside", { { .str = 0 }, ASVAL_FUNCTION } },
    { "onRollOut",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onRollOver",    { { .str = 0 }, ASVAL_FUNCTION } },
    { "onSetFocus",    { { .str = 0 }, ASVAL_FUNCTION } },
    { "onUnload",      { { .str = 0 }, ASVAL_FUNCTION } },
    // methods
    { "attachAudio",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //(id: Object) : Void
    { "attachBitmap",  { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bmp: BitmapData, depth: Number, [pixelSnapping: String], [smoothing: Boolean]) : Void
    { "attachMovie",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //(id: String, name: String, depth: Number, [initObject: Object]) : MovieClip
    { "beginBitmapFill", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bmp: BitmapData, [matrix: Matrix], [repeat: Boolean], [smoothing: Boolean]) : Void
    { "beginFill",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(rgb: Number, [alpha: Number]) : Void
    { "beginGradientFill", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(fillType: String, colors: Array, alphas: Array, ratios: Array, matrix: Object, [spreadMethod: String], [interpolationMethod: String], [focalPointRatio: Number]) : Void
    { "clear",         { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "createEmptyMovieClip", { { .str = (const char *)createEmptyMovieClip }, ASVAL_NATIVE_FN } }, //(name: String, depth: Number) : MovieClip
    { "createTextField", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(instanceName: String, depth: Number, x: Number, y: Number, width: Number, height: Number) : TextField
    { "curveTo",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //(controlX: Number, controlY: Number, anchorX: Number, anchorY: Number) : Void
    { "duplicateMovieClip", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(name: String, depth: Number, [initObject: Object]) : MovieClip
    { "endFill",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "getBounds",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bounds: Object) : Object
    { "getBytesLoaded", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getBytesTotal", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getDepth",      { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getInstanceAtDepth", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(depth: Number) : MovieClip
    { "getNextHighestDepth", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getRect",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bounds: Object) : Object
    { "getSWFVersion", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getTextSnapshot", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : TextSnapshot
    { "getURL",        { { .str = 0 }, ASVAL_NATIVE_FN } }, //(url: String, [window: String], [method: String]) : Void
    { "globalToLocal", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(pt: Object) : Void
    { "gotoAndPlay",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //(frame: Object) : Void
    { "gotoAndStop",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //(frame: Object) : Void
    { "hitTest",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Boolean
    { "lineGradientStyle", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(fillType: String, colors: Array, alphas: Array, ratios: Array, matrix: Object, [spreadMethod: String], [interpolationMethod: String], [focalPointRatio: Number]) : Void
    { "lineStyle",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(thickness: Number, rgb: Number, alpha: Number, pixelHinting: Boolean, noScale: String, capsStyle: String, jointStyle: String, miterLimit: Number) : Void
    { "lineTo",        { { .str = 0 }, ASVAL_NATIVE_FN } }, //(x: Number, y: Number) : Void
    { "loadMovie",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(url: String, [method: String]) : Void
    { "loadVariables", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(url: String, [method: String]) : Void
    { "localToGlobal", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(pt: Object) : Void
    { "moveTo",        { { .str = 0 }, ASVAL_NATIVE_FN } }, //(x: Number, y: Number) : Void
    { "nextFrame",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "play",          { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "prevFrame",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "removeMovieClip", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "setMask",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //(mc: Object) : Void
    { "startDrag",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //([lockCenter: Boolean], [left: Number], [top: Number], [right: Number], [bottom: Number]) : Void
    { "stop",          { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "stopDrag",      { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "swapDepths",    { { .str = 0 }, ASVAL_NATIVE_FN } }, //(target: Object) : Void
    { "unloadMovie",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
};

ASClass g_movieclip =
{
    "MovieClip", &g_movieclip_members[0], sizeof(g_movieclip_members)/sizeof(g_movieclip_members[0])
};

ASMember g_properties[] =
{
    { "this", { { .str = (void*)&g_movieclip }, ASVAL_CLASS } },
    { "_root", { { .str = (void*)&g_movieclip }, ASVAL_CLASS } },
    { "_accProps", { { .str = 0 }, ASVAL_CLASS } },
    { "_focusrect", { { .str = 0 }, ASVAL_CLASS } },
    { "_global", { { .str = 0 }, ASVAL_CLASS } },
    { "_highquality", { { .str = 0 }, ASVAL_CLASS } },
    { "_level", { { .str = 0 }, ASVAL_CLASS } },
    { "maxscroll", { { .str = 0 }, ASVAL_CLASS } },
    { "_parent", { { .str = 0 }, ASVAL_CLASS } },
    { "_quality", { { .str = 0 }, ASVAL_CLASS } },
    { "scroll", { { .str = 0 }, ASVAL_CLASS } },
    { "_soundbuftime", { { .str = 0 }, ASVAL_CLASS } }
};

int g_num_properties = sizeof(g_properties)/sizeof(g_properties[0]);
