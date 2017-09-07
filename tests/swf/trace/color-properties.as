// makeswf -v 7 -s 200x150 -r 1 -o color-properties-7.swf color-properties.as

#include "trace_properties.as"

var a = new Color();
var b = new Color(this);
var c = new Color("moi");

trace_properties (_global.Color, "_global", "Color");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");
trace_properties (c, "local", "c");

loadMovie ("FSCommand:quit", "");
