// makeswf -v 7 -r 1 -o global-variable-properties-7.swf global-variable-properties.as

#include "trace_properties.as"

trace_properties (_global.Infinity, "_global", "Infinity");
trace_properties (_global.NaN, "_global", "NaN");
trace_properties (_global.o, "_global", "o");
// Need to unhide it first on version < 8
//ASSetPropFlags (_global, "flash", 0, 5248);
//trace_properties (_global.flash, "_global", "flash");

loadMovie ("FSCommand:quit", "");

