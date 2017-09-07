// makeswf -v 7 -r 1 -o drop-shadow-filter-properties-7.swf drop-shadow-filter-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.filters.DropShadowFilter ();

trace_properties (_global.flash.filters.DropShadowFilter,
    "_global.flash.filters", "DropShadowFilter");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
