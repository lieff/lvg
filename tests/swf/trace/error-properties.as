// makeswf -v 7 -r 1 -o error-properties-7.swf error-properties.as

#include "trace_properties.as"

var a = new Error ();
var b = new Error ("test");

trace_properties (_global.Error, "_global", "Error");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");
