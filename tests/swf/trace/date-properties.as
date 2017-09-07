// makeswf -v 7 -r 1 -o properties-7.swf properties.as

#include "trace_properties.as"

var a = new Date ();
var b = Date.UTC (1983, 11, 9);

trace_properties (_global.Date, "_global", "Date");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");
