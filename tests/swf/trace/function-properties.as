// makeswf -v 7 -r 1 -o function-properties-7.swf function-properties.as

#include "trace_properties.as"

var a = new_empty_object ();
a.myFunction = function () { };

trace_properties (_global.Function, "_global", "Function");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
