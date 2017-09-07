// makeswf -v 7 -r 1 -o local-connection-properties-7.swf local-connection-properties.as

#include "trace_properties.as"

var a = new LocalConnection ();

trace_properties (_global.LocalConnection, "_global", "LocalConnection");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
