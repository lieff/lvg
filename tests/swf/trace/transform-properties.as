// makeswf -v 7 -r 1 -o transform-properties-7.swf transform-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.geom.Transform (this);

trace_properties (_global.flash.geom.Transform, "_global.flash.geom",
    "Transform");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
