// makeswf -v 7 -r 1 -o selection-properties-7.swf selection-properties.as

#include "trace_properties.as"

var a = new Selection ();

trace_properties (_global.Selection, "_global", "Selection");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
