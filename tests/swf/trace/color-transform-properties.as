// makeswf -v 7 -r 1 -o bitmap-data-properties-7.swf bitmap-data-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.geom.ColorTransform ();

trace_properties (_global.flash.geom.ColorTransform, "_global.flash.geom",
    "ColorTransform");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
