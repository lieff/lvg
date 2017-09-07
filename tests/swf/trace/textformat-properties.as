// makeswf -v 7 -r 1 -o textformat-properties-7.swf textformat-properties.as

#include "trace_properties.as"

trace_properties (_global.TextFormat, "_global", "TextFormat");

var a = new TextFormat ();

trace_properties (_global.TextFormat, "_global", "TextFormat");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
