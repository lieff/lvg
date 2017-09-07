// makeswf -v 7 -r 1 -o color-matrix-filter-properties-7.swf color-matrix-filter-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.filters.ColorMatrixFilter ();

trace_properties (_global.flash.filters.ColorMatrixFilter,
    "_global.flash.filters", "ColorMatrixFilter");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
