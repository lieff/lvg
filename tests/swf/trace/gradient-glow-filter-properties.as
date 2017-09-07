// makeswf -v 7 -r 1 -o gradient-glow-filter-properties-7.swf gradient-glow-filter-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.filters.GradientGlowFilter ();

trace_properties (_global.flash.filters.GradientGlowFilter,
        "_global.flash.filters", "GradientGlowFilter");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
