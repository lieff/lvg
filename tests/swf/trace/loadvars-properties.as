// makeswf -v 7 -r 1 -o loadvars-properties-7.swf loadvars-properties.as

#include "trace_properties.as"

var a = new LoadVars ();

trace_properties (_global.LoadVars, "_global", "LoadVars");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
