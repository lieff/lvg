// makeswf -v 7 -r 1 -o rectangle-properties-7.swf rectangle-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.geom.Rectangle ();

trace_properties (_global.flash.geom.Rectangle, "_global.flash.geom", "Rectangle");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
