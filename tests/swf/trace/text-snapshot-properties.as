// makeswf -v 7 -r 1 -o text-snapshot-properties-7.swf text-snapshot-properties.as

#include "trace_properties.as"

var a = new TextSnapshot ();

trace_properties (_global.TextSnapshot, "_global", "TextSnapshot");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
