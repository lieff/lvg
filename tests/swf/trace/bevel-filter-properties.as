// makeswf -v 7 -r 1 -o bevel-filter-properties-7.swf bevel-filter-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.filters.BevelFilter ();

trace_properties (_global.flash.filters.BevelFilter, "_global.flash.filters",
    "BevelFilter");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
