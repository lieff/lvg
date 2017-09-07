// makeswf -v 7 -r 1 -o matrix-properties-7.swf matrix-properties.as

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 4096);

#include "trace_properties.as"

var a = new flash.geom.Matrix ();

trace_properties (_global.flash.geom.Matrix, "_global.flash.geom", "Matrix");
trace_properties (a, "local", "a");

getURL ("FSCommand:quit", "");
