// makeswf -v 7 -r 1 -o array-properties-7.swf array-properties.as

#include "trace_properties.as"

var a = new Array ();
var b = [];

trace_properties (_global.Array, "_global", "Array");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");

