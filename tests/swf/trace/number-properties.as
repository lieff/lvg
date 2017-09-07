// makeswf -v 7 -s 200x150 -r 1 -o number-properties-7.swf number-properties.as

#include "trace_properties.as"

var a = new Number(10);
var b = 42;

trace_properties (_global.Number, "_global", "Number");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");
