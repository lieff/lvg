// makeswf -v 7 -r 1 -o boolean-properties-7.swf boolean-properties.as

#include "trace_properties.as"

var a = new Boolean(false);
var b = true;

trace_properties (_global.Boolean, "_global", "Boolean");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");
