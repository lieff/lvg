// makeswf -v 7 -s 200x150 -r 1 -o string-properties-7.swf string-properties.as

#include "trace_properties.as"

var a = new String("hello");
var b = "hello";
var c = new String();

trace_properties (_global.String, "_global", "String");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");
trace_properties (c, "local", "c");

loadMovie ("FSCommand:quit", "");
